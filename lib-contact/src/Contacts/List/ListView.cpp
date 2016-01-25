/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "Contacts/List/ListView.h"
#include "Contacts/List/GroupItem.h"
#include "Contacts/List/MyProfileItem.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Settings/MainView.h"

#include "Ui/Genlist.h"
#include "Ui/GenlistItem.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"

#include <functional>

using namespace Contacts;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace Utils;

ListView::ListView(PersonProvider::FilterType personFilter)
	: m_Genlist(nullptr), m_Index(nullptr),
	  m_MyProfileGroup(nullptr), m_FavoritesGroup(nullptr), m_MfcGroup(nullptr),
	  m_Provider(PersonProvider(personFilter)),
	  m_ViewMode(DefaultMode)
{
	m_ModeSetters[DefaultMode]     = &ListView::setDefaultMode;
	m_ModeSetters[MultipickMode]   = &ListView::setMultiPickMode;
}

ListView::~ListView()
{
	m_Provider.unsetInsertCallback();
	contacts_db_remove_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::setMode(ViewMode mode)
{
	if (m_ViewMode != mode) {
		if (m_ModeSetters[mode]) {
			m_ModeSetters[mode](*this);
		}

		m_ViewMode = mode;
	}
}

Evas_Object *ListView::createIndex(Evas_Object *parent)
{
	m_Index = elm_index_add(parent);
	elm_index_autohide_disabled_set(m_Index, EINA_TRUE);
	elm_index_omit_enabled_set(m_Index, EINA_TRUE);

	evas_object_smart_callback_add(m_Index, "changed",
			(Evas_Smart_Cb) makeCallback(&ListView::onIndexChanged), this);
	evas_object_smart_callback_add(m_Index, "selected",
			(Evas_Smart_Cb) makeCallback(&ListView::onIndexSelected), this);

	return m_Index;
}

void ListView::createFloatingButton()
{
	Evas_Object *button = elm_button_add(getEvasObject());
	elm_object_style_set(button, "bottom");
	elm_object_translatable_text_set(button, "IDS_PB_OPT_CREATE");
	evas_object_smart_callback_add(button, "clicked",
			(Evas_Smart_Cb) makeCallback(&ListView::onCreatePressed), this);

	getPage()->setContent("toolbar", button);
}

void ListView::deleteFloatingButton()
{
	//Todo
}

void ListView::fillList()
{
	fillMyProfile();
	fillFavorites();
	fillMfc();
	fillPersonList();
}

void ListView::fillSelectAll()
{
	//Todo
}

void ListView::fillMyProfile()
{
	if (!m_MyProfileGroup) {
		insertMyProfileGroupItem();
	}
	updateMyProfileItem(nullptr);
}

void ListView::fillFavorites()
{
	//Todo
}

void ListView::fillMfc()
{
	//Todo
}

void ListView::fillPersonList()
{
	PersonList list = m_Provider.getPersonList();
	PersonGroupItem *group = nullptr;

	for (auto &&person : list) {
		const UniString &nextLetter = person->getIndexLetter();

		if (!group || group->getTitle() != nextLetter) {
			group = insertPersonGroupItem(nextLetter);
		}

		m_Genlist->insert(createPersonItem(std::move(person)), group);
	}
}

void ListView::deleteSelectAll()
{
	//Todo
}

void ListView::deleteMyProfile()
{
	delete m_MyProfileGroup;
	m_MyProfileGroup = nullptr;
}

void ListView::deleteMfc()
{
	delete m_MfcGroup;
	m_MfcGroup = nullptr;
}

void ListView::setDefaultMode()
{
	getPage()->setTitle("IDS_PB_TAB_CONTACTS");
	deleteSelectAll();
	fillMyProfile();
	setFavouriteItemsMode(PersonItem::DefaultMode);
	fillMfc();
	setPersonItemsMode(PersonItem::DefaultMode);
	createFloatingButton();
}

void ListView::setMultiPickMode()
{
	getPage()->setTitle("IDS_PB_HEADER_PD_SELECTED_ABB");
	fillSelectAll();
	deleteMyProfile();
	setFavouriteItemsMode(PersonItem::PickMode);
	deleteMfc();
	setPersonItemsMode(PersonItem::PickMode);
	deleteFloatingButton();
}

void ListView::setFavouriteItemsMode(PersonItem::ItemMode mode)
{
	if (m_FavoritesGroup) {
		for (auto &&favoriteItem : *m_FavoritesGroup) {
			static_cast<PersonItem*>(favoriteItem)->setMode(mode);
		}
	}
}

void ListView::setPersonItemsMode(PersonItem::ItemMode mode)
{
	for (auto &&group : m_PersonGroups) {
		for (auto &&personItem : *group.second) {
			static_cast<PersonItem*>(personItem)->setMode(mode);
		}
	}
}

void ListView::insertMyProfileGroupItem()
{
	m_MyProfileGroup = new GroupItem("IDS_PB_HEADER_ME");
	m_Genlist->insert(m_MyProfileGroup, nullptr, getNextGroupItem(MyProfileSection));
	elm_genlist_item_select_mode_set(m_MyProfileGroup->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

Ui::GenlistGroupItem *ListView::getNextGroupItem(ListSections currentSection)
{
	static Ui::GenlistGroupItem *groups[] = {
		/* SearchSection    = Todo: Add Select all section when it will be implemented */
		/* SelectAllSection = m_MyProfileGroup,*/
		/* MyProfileSection = */ m_FavoritesGroup,
		/* FavoritesSection = */ m_MfcGroup,
		/* MfcSection       = */ nullptr,
		/* PersonSection    = */ nullptr,
		/* SectionMax       = */ nullptr
	};

	if (currentSection < SectionMax) {
		groups[MfcSection] = !m_PersonGroups.empty() ? m_PersonGroups.begin()->second : nullptr;

		if (!groups[currentSection]) {
			return getNextGroupItem(static_cast<ListSections>(currentSection + 1));
		}

		return groups[currentSection];
	}

	return nullptr;
}

void ListView::updateMyProfileItem(const char *view_uri)
{
	elm_genlist_item_subitems_clear(m_MyProfileGroup->getObjectItem());

	MyProfileItem *item = new MyProfileItem(MyProfilePtr(new MyProfile()));
	m_Genlist->insert(item, m_MyProfileGroup);

	item->setSelectedCallback([this, item]() {
		int id = item->getMyProfile().getId();
		getNavigator()->navigateTo(new Input::InputView(id, Input::InputView::TypeMyProfile));
	});
}

Elm_Object_Item *ListView::insertIndexItem(const char *indexLetter, Elm_Object_Item *nextItem)
{
	Elm_Object_Item *indexItem = nullptr;

	if (nextItem) {
		indexItem = elm_index_item_insert_before(m_Index, nextItem, indexLetter, nullptr, nullptr);
	} else {
		indexItem = elm_index_item_append(m_Index, indexLetter, nullptr, nullptr);
	}
	elm_index_level_go(m_Index, 0);

	return indexItem;
}

PersonGroupItem *ListView::insertPersonGroupItem(UniString indexLetter, PersonGroupItem *nextGroup)
{
	Elm_Object_Item *indexItem = insertIndexItem(indexLetter.getUtf8Str().c_str(),
			nextGroup ? nextGroup->getIndexItem() : nullptr);

	PersonGroupItem *item = new PersonGroupItem(std::move(indexLetter), indexItem);
	m_Genlist->insert(item, nullptr, nextGroup);
	elm_genlist_item_select_mode_set(item->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	elm_object_item_data_set(indexItem, item->getObjectItem());
	m_PersonGroups.insert({ item->getTitle(), item });

	return item;
}

void ListView::deletePersonGroupItem(PersonGroupItem *group)
{
	elm_object_item_del(group->getIndexItem());
	elm_index_level_go(m_Index, 0);

	m_PersonGroups.erase(group->getTitle());
	delete group;
}

PersonGroupItem *ListView::getNextPersonGroupItem(const Utils::UniString &indexLetter)
{
	auto it = m_PersonGroups.lower_bound(indexLetter);
	if (it != m_PersonGroups.end()) {
		return it->second;
	}

	return nullptr;
}

PersonItem *ListView::createPersonItem(PersonPtr person)
{
	using namespace std::placeholders;

	PersonItem *item = new PersonItem(std::move(person));

	m_Provider.setChangeCallback(item->getPerson(),
			std::bind(&ListView::onPersonChanged, this, _1, _2, item));

	item->setSelectedCallback([this, item]() {
		switch (m_ViewMode) {
			case DefaultMode: launchPersonDetail(item); break;
			case MultipickMode: /* Todo: Make "Select all" calculations */ break;
			default: break;
		}
	});

	return item;
}

void ListView::insertPersonItem(PersonItem *item)
{
	PersonGroupItem *group = nullptr;
	PersonItem *nextItem = nullptr;
	const UniString &indexLetter = item->getPerson().getIndexLetter();

	auto it = m_PersonGroups.find(indexLetter);
	if (it != m_PersonGroups.end()) {
		group = it->second;
		nextItem = getNextPersonItem(it->second, item->getPerson());
	} else {
		PersonGroupItem *nextGroup = getNextPersonGroupItem(indexLetter);
		group = insertPersonGroupItem(indexLetter, nextGroup);
	}

	m_Genlist->insert(item, group, nextItem);
}

void ListView::updatePersonItem(PersonItem *item, Model::PersonPtr person)
{
	if (item->getPerson() != *person) {
		PersonGroupItem *oldGroup = static_cast<PersonGroupItem *>(item->getParentItem());

		item->pop();

		item->setPerson(std::move(person));
		insertPersonItem(item);

		if (oldGroup->empty()) {
			deletePersonGroupItem(oldGroup);
		}
	} else {
		const char *oldImagePath = item->getPerson().getImagePath();
		const char *newImagePath = person->getImagePath();

		bool equalImages = (!oldImagePath && !newImagePath) ||
				(oldImagePath && newImagePath && strcmp(oldImagePath, newImagePath) == 0);

		item->setPerson(std::move(person));
		if (!equalImages) {
			elm_genlist_item_fields_update(item->getObjectItem(),
					PART_PERSON_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

void ListView::deletePersonItem(PersonItem *item)
{
	PersonGroupItem *oldGroup = static_cast<PersonGroupItem *>(item->getParentItem());

	m_Provider.unsetChangeCallback(item->getPerson());
	delete item;
	if (oldGroup->empty()) {
		deletePersonGroupItem(oldGroup);
	}
}

PersonItem *ListView::getNextPersonItem(PersonGroupItem *group, const Person &person)
{
	for (auto &&item : *group) {
		PersonItem *personItem = static_cast<PersonItem *>(item);
		if (person < personItem->getPerson()) {
			return personItem;
		}
	}

	return nullptr;
}

void ListView::launchPersonDetail(PersonItem *item)
{
	int id = 0;
	contacts_record_get_int(item->getPerson().getRecord(), _contacts_person.display_contact_id, &id);

	getNavigator()->navigateTo(new Input::InputView(id));
}

Evas_Object *ListView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Genlist = new Ui::Genlist();
	elm_object_part_content_set(layout, "elm.swallow.content", m_Genlist->create(layout));
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	return layout;
}

void ListView::onPageAttached()
{
	getPage()->setTitle("IDS_PB_TAB_CONTACTS");
	createFloatingButton();
}

void ListView::onCreated()
{
	fillList();

	m_Provider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, std::placeholders::_1));
	contacts_db_add_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem)
{
	Elm_Object_Item *genlistItem = (Elm_Object_Item *) elm_object_item_data_get(indexItem);
	elm_genlist_item_show(genlistItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void ListView::onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem)
{
	onIndexChanged(index, indexItem);
	elm_index_item_selected_set(indexItem, EINA_FALSE);
}

void ListView::onCreatePressed()
{
	getNavigator()->navigateTo(new Input::InputView());
}

void ListView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		ListView *deleteView = new Contacts::List::ListView();
		getNavigator()->navigateTo(deleteView);
		deleteView->setMode(MultipickMode);
	});

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	menu->show();
}

void ListView::onPersonInserted(PersonPtr person)
{
	insertPersonItem(createPersonItem(std::move(person)));
}

void ListView::onPersonChanged(PersonPtr person, contacts_changed_e changeType, PersonItem *item)
{
	if (changeType == CONTACTS_CHANGE_DELETED) {
		deletePersonItem(item);
	} else if (changeType == CONTACTS_CHANGE_UPDATED) {
		updatePersonItem(item, std::move(person));
	}
}
