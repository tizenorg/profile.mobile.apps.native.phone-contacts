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

#include "Contacts/Details/DetailsView.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Settings/MainView.h"

#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"

using namespace Contacts;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace Utils;
using namespace std::placeholders;

ListView::ListView(int filterType)
	: m_Genlist(nullptr), m_Index(nullptr), m_AddButton(nullptr),
	  m_Sections{ nullptr },
	  m_Provider(PersonProvider::ModeAll, filterType)
{
}

ListView::~ListView()
{
	m_Provider.unsetInsertCallback();
	contacts_db_remove_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

Evas_Object *ListView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Genlist = new Ui::Genlist();
	m_Genlist->create(layout);

	elm_object_part_content_set(layout, "elm.swallow.content", m_Genlist->getEvasObject());
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	return layout;
}

void ListView::onPageAttached()
{
	SelectView::onPageAttached();
	updatePageMode();
}

void ListView::onCreated()
{
	updateSectionsMode();

	m_Provider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, _1));
	contacts_db_add_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		ListView *view = new ListView();
		view->setSelectMode(SelectMulti);
		view->setSelectCallback([](SelectResults results) {
			std::vector<int> ids;
			for (auto &&result : results) {
				ids.push_back(result.itemId);
			}

			contacts_db_delete_records(_contacts_person._uri, ids.data(), ids.size());
			return true;
		});
		getNavigator()->navigateTo(view);
	});

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	menu->show();
}

const char *ListView::getPageTitle() const
{
	if (getSelectMode() == SelectNone) {
		return "IDS_PB_TAB_CONTACTS";
	}

	return SelectView::getPageTitle();
}

void ListView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
}

void ListView::onSelectModeChanged(SelectMode selectMode)
{
	updatePageMode();
	updateSectionsMode();
}

void ListView::fillMyProfile()
{
	if (!m_Sections[SectionMyProfile]) {
		insertMyProfileGroupItem();
	}
	updateMyProfileItem(nullptr);
}

void ListView::fillFavorites()
{
	if (!m_Sections[SectionFavorites]) {
		//Todo Create here Favorite group
	} else {
		setFavouriteItemsMode(getSelectMode());
	}
}

void ListView::fillMfc()
{
	//Todo
}

void ListView::fillPersonList()
{
	if (m_PersonGroups.empty()) {
		PersonList list = m_Provider.getPersonList();
		PersonGroupItem *group = nullptr;

		for (auto &&person : list) {
			const UniString &nextLetter = person->getIndexLetter();

			if (!group || group->getTitle() != nextLetter) {
				group = insertPersonGroupItem(nextLetter);
			}

			auto item = createPersonItem(std::move(person));
			m_Genlist->insert(item, group);
			onItemInserted(item);
		}
	}
}

void ListView::setFavouriteItemsMode(SelectMode selectMode)
{
	if (m_Sections[SectionFavorites]) {
		for (auto &&favoriteItem : *m_Sections[SectionFavorites]) {
			static_cast<PersonItem *>(favoriteItem)->setSelectMode(selectMode);
		}
	}
}

void ListView::addSection(SectionId sectionId)
{
	switch (sectionId) {
		case SectionMyProfile: fillMyProfile();     break;
		case SectionFavorites: fillFavorites();     break;
		case SectionMfc:       fillMfc();           break;
		case SectionPerson:    fillPersonList();    break;
		default: break;
	}
}

void ListView::removeSection(SectionId sectionId)
{
	delete m_Sections[sectionId];
	m_Sections[sectionId] = nullptr;
}

Ui::GenlistItem *ListView::getNextSectionItem(SectionId currentSection)
{
	for (size_t nextSection = currentSection + 1; nextSection < SectionMax; ++nextSection) {
		if (m_Sections[nextSection]) {
			return m_Sections[nextSection];
		}
	}

	return nullptr;
}

bool ListView::getSectionVisibility(SelectMode selectMode, SectionId sectionId)
{
	static bool sectionVisibility[][ListView::SectionMax] = {
		/* SelectNone   = */ {
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ true,
			/* SectionPerson    = */ true
		},
		/* SelectSingle = */ {
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		},
		/* SelectMulti  = */ {
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		}
	};

	return sectionVisibility[selectMode][sectionId];
}

void ListView::updatePageMode()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	switch (getSelectMode()) {
		case SelectNone:
			createAddButton();
			page->setContent("toolbar", m_AddButton);
			break;

		case SelectSingle:
			deleteAddButton();
			page->setContent("toolbar", nullptr);
			break;

		case SelectMulti:
			deleteAddButton();
			page->setContent("toolbar", nullptr);
			break;
	}
}

void ListView::updateSectionsMode()
{
	if (!m_Genlist) {
		return;
	}

	for (size_t i = SectionFirst; i < SectionMax; ++i) {
		SectionId sectionId = static_cast<SectionId>(i);
		if (getSectionVisibility(getSelectMode(), sectionId)) {
			addSection(sectionId);
		} else {
			removeSection(sectionId);
		}
	}
}

void ListView::createAddButton()
{
	m_AddButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_AddButton, "bottom");
	elm_object_translatable_text_set(m_AddButton, "IDS_PB_OPT_CREATE");
	evas_object_smart_callback_add(m_AddButton, "clicked",
			makeCallback(&ListView::onAddPressed), this);
}

void ListView::deleteAddButton()
{
	evas_object_del(m_AddButton);
	m_AddButton = nullptr;
}

void ListView::insertMyProfileGroupItem()
{
	auto group = new GroupItem("IDS_PB_HEADER_ME");
	m_Genlist->insert(group, nullptr, getNextSectionItem(SectionMyProfile));
	elm_genlist_item_select_mode_set(group->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	m_Sections[SectionMyProfile] = group;
}

void ListView::updateMyProfileItem(const char *view_uri)
{
	elm_genlist_item_subitems_clear(m_Sections[SectionMyProfile]->getObjectItem());

	MyProfileItem *item = new MyProfileItem(MyProfilePtr(new MyProfile()));
	m_Genlist->insert(item, m_Sections[SectionMyProfile]);

	item->setSelectedCallback([this, item]() {
		int id = item->getMyProfile().getId();
		if (id > 0) {
			getNavigator()->navigateTo(new Details::DetailsView(id, Details::DetailsView::TypeMyProfile));
		} else {
			getNavigator()->navigateTo(new Input::InputView(id, Input::InputView::TypeMyProfile));
		}
	});
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
	m_Sections[SectionPerson] = m_PersonGroups.begin()->second;

	return item;
}

void ListView::deletePersonGroupItem(PersonGroupItem *group)
{
	elm_object_item_del(group->getIndexItem());
	elm_index_level_go(m_Index, 0);

	m_PersonGroups.erase(group->getTitle());
	m_Sections[SectionPerson] = !m_PersonGroups.empty() ? m_PersonGroups.begin()->second : nullptr;
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
	PersonItem *item = new PersonItem(std::move(person));
	m_Provider.setChangeCallback(item->getPerson(),
			std::bind(&ListView::onPersonChanged, this, _1, _2, item));

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

void ListView::onItemPressed(SelectItem *item)
{
	PersonItem *personItem = static_cast<PersonItem *>(item);
	int id = personItem->getPerson().getDisplayContactId();
	getNavigator()->navigateTo(new Details::DetailsView(id));
}

void ListView::onAddPressed(Evas_Object *button, void *eventInfo)
{
	getNavigator()->navigateTo(new Input::InputView());
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

void ListView::onPersonInserted(PersonPtr person)
{
	auto item = createPersonItem(std::move(person));
	insertPersonItem(item);
	onItemInserted(item);
}

void ListView::onPersonChanged(PersonPtr person, contacts_changed_e changeType, PersonItem *item)
{
	if (changeType == CONTACTS_CHANGE_UPDATED) {
		updatePersonItem(item, std::move(person));
	} else if (changeType == CONTACTS_CHANGE_DELETED) {
		onItemRemove(item);
		deletePersonItem(item);
	}
}
