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
#include "Contacts/List/SelectAllItem.h"
#include "Contacts/List/MyProfileItem.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Details/DetailsView.h"
#include "Contacts/Settings/MainView.h"

#include "Ui/Genlist.h"
#include "Ui/GenlistItem.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"

#include <app_i18n.h>
#include <functional>

#define TITLE_SIZE          32
#define PART_TITLE_RIGHT    "title_right_btn"
#define PART_TITLE_LEFT     "title_left_btn"

using namespace Contacts;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace Utils;

ListView::ListView(PersonProvider::FilterType personFilter)
	: m_Genlist(nullptr), m_Index(nullptr),
	  m_CancelButton(nullptr), m_DoneButton(nullptr),
	  m_Sections{ nullptr },
	  m_Provider(PersonProvider(personFilter)),
	  m_Mode(ModeDefault),
	  m_PersonCount(0), m_CheckedCount(0)
{
}

ListView::~ListView()
{
	m_Provider.unsetInsertCallback();
	contacts_db_remove_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::setMode(Mode mode)
{
	if (m_Mode != mode) {
		m_Mode = mode;

		updatePageMode();
		updateSectionsMode();
	}
}

void ListView::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

void ListView::unsetResultCallback()
{
	m_OnResult = nullptr;
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
	updatePageMode();
}

void ListView::onCreated()
{
	fillList();

	m_Provider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, std::placeholders::_1));
	contacts_db_add_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		ListView *deleteView = new ListView();
		getNavigator()->navigateTo(deleteView);
		deleteView->setMode(ModeMultiPick);
		deleteView->setResultCallback([](PersonIds ids) {
			contacts_db_delete_records(_contacts_person._uri, ids.data(), ids.size());
		});
	});

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	menu->show();
}

void ListView::fillList()
{
	for (size_t i = SectionMyProfile; i < SectionMax; ++i) {
		addSection(static_cast<SectionId>(i));
	}
}

void ListView::fillSearchItem()
{
	//Todo
}

void ListView::fillSelectAllItem()
{
	if (!m_Sections[SectionSelectAll]) {
		auto item = new SelectAllItem();
		m_Genlist->insert(item, nullptr, getNextSectionItem(SectionSelectAll));
		item->setSelectedCallback(std::bind(&ListView::onSelectAllChecked, this));

		m_Sections[SectionSelectAll] = item;
	}
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
		setFavouriteItemsMode(getItemMode(m_Mode));
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

			m_Genlist->insert(createPersonItem(std::move(person)), group);

			++m_PersonCount;
		}
	} else {
		setPersonItemsMode(getItemMode(m_Mode));
	}
}

void ListView::setFavouriteItemsMode(PersonItem::Mode mode)
{
	auto group = dynamic_cast<Ui::GenlistGroupItem *>(m_Sections[SectionFavorites]);
	if (m_Sections[SectionFavorites]) {
		for (auto &&favoriteItem : *group) {
			static_cast<PersonItem*>(favoriteItem)->setMode(mode);
		}
	}
}

void ListView::setPersonItemsMode(PersonItem::Mode mode)
{
	for (auto &&group : m_PersonGroups) {
		for (auto &&personItem : *group.second) {
			static_cast<PersonItem*>(personItem)->setMode(mode);
		}
	}
}

void ListView::addSection(SectionId sectionId)
{
	switch (sectionId) {
		case SectionSearch:    fillSearchItem();    break;
		case SectionSelectAll: fillSelectAllItem(); break;
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

bool ListView::getSectionVisibility(Mode mode, SectionId sectionId)
{
	static bool sectionVisibility[ListView::ModeMax][ListView::SectionMax] = {
		/* ModeDefault    = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ false,
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ true,
			/* SectionPerson    = */ true
		},
		/* ModeSinglePick = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ false,
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		},
		/* ModeMultiPick  = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ true,
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		}
	};

	return sectionVisibility[mode][sectionId];
}

PersonItem::Mode ListView::getItemMode(Mode viewMode)
{
	if (viewMode == ModeMultiPick) {
		return PersonItem::ModePick;
	} else {
		return PersonItem::ModeDefault;
	}
}

void ListView::updateTitle()
{
	switch (m_Mode) {
		case ModeDefault:
			getPage()->setTitle("IDS_PB_TAB_CONTACTS");
			break;
		case ModeSinglePick:
			getPage()->setTitle("IDS_PB_HEADER_SELECT_CONTACT_ABB2");
			break;
		case ModeMultiPick:
		{
			char title[TITLE_SIZE];
			snprintf(title, TITLE_SIZE, _("IDS_PB_HEADER_PD_SELECTED_ABB"), m_CheckedCount);
			getPage()->setTitle(title);
			break;
		}
		default:
			break;
	}
}

void ListView::updateSelectAll()
{
	auto selectAllItem = static_cast<SelectAllItem *>(m_Sections[SectionSelectAll]);
	selectAllItem->setChecked(m_CheckedCount == m_PersonCount);
}

void ListView::updatePageMode()
{
	updateTitle();

	switch (m_Mode) {
		case ModeDefault:
			deleteCancelButton();
			deleteDoneButton();
			createNewContactButton();
			break;
		case ModeSinglePick:
			deleteCancelButton();
			deleteDoneButton();
			deleteNewContactButton();
			break;
		case ModeMultiPick:
			createCancelButton();
			createDoneButton();
			deleteNewContactButton();
			break;
		default:
			break;
	}
}

void ListView::updateSectionsMode()
{
	for (size_t i = SectionFirst; i < SectionMax; ++i) {
		SectionId sectionId = static_cast<SectionId>(i);
		if (getSectionVisibility(m_Mode, sectionId)) {
			addSection(sectionId);
		} else {
			removeSection(sectionId);
		}
	}
}

void ListView::createNewContactButton()
{
	Evas_Object *button = elm_button_add(getEvasObject());
	elm_object_style_set(button, "bottom");
	elm_object_translatable_text_set(button, "IDS_PB_OPT_CREATE");
	evas_object_smart_callback_add(button, "clicked",
			(Evas_Smart_Cb) makeCallback(&ListView::onCreatePressed), this);

	getPage()->setContent("toolbar", button);
}

void ListView::deleteNewContactButton()
{
	//Todo
}

void ListView::createCancelButton()
{
	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, "IDS_PB_BUTTON_CANCEL");
	evas_object_smart_callback_add(m_CancelButton, "clicked",
			[](void *self, Evas_Object *obj, void *eventInfo) {
				delete static_cast<ListView *>(self);
			},
			this);

	getPage()->setContent(PART_TITLE_LEFT, m_CancelButton);
}

void ListView::deleteCancelButton()
{
	evas_object_del(m_CancelButton);
	m_CancelButton = nullptr;
}

void ListView::createDoneButton()
{
	Evas_Object *m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&ListView::onDonePressed), this);

	getPage()->setContent(PART_TITLE_RIGHT, m_DoneButton);
}

void ListView::deleteDoneButton()
{
	evas_object_del(m_DoneButton);
	m_DoneButton = nullptr;
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
	m_Genlist->insert(item, dynamic_cast<Ui::GenlistGroupItem *>(m_Sections[SectionMyProfile]));

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
	using namespace std::placeholders;

	PersonItem *item = new PersonItem(std::move(person), getItemMode(m_Mode));

	m_Provider.setChangeCallback(item->getPerson(),
			std::bind(&ListView::onPersonChanged, this, _1, _2, item));
	item->setSelectedCallback(std::bind(&ListView::onItemSelected, this, item));

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

ListView::PersonIds ListView::getCheckedPersonIds()
{
	PersonIds ids;
	for (auto &&group : m_PersonGroups) {
		for (auto &&item : *group.second) {
			PersonItem *personItem = static_cast<PersonItem *>(item);
			if (personItem->isChecked()) {
				ids.push_back(personItem->getPerson().getId());
			}
		}
	}

	return ids;
}

void ListView::launchPersonDetail(PersonItem *item)
{
	int id = item->getPerson().getDisplayContactId();
	getNavigator()->navigateTo(new Details::DetailsView(id));
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

void ListView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	if (m_OnResult) {
		m_OnResult(getCheckedPersonIds());
	}

	delete this;
}

void ListView::onPersonItemInserted(PersonItem *item)
{
	++m_PersonCount;

	updateSelectAll();
	updateTitle();
}

void ListView::onPersonItemDelete(PersonItem *item)
{
	--m_PersonCount;
	if (item->isChecked()) {
		--m_CheckedCount;
	}

	updateSelectAll();
	updateTitle();
}

void ListView::onPersonInserted(PersonPtr person)
{
	auto item = createPersonItem(std::move(person));
	insertPersonItem(item);
	onPersonItemInserted(item);
}

void ListView::onPersonChanged(PersonPtr person, contacts_changed_e changeType, PersonItem *item)
{
	if (changeType == CONTACTS_CHANGE_UPDATED) {
		updatePersonItem(item, std::move(person));
	} else if (changeType == CONTACTS_CHANGE_DELETED) {
		onPersonItemDelete(item);
		deletePersonItem(item);
	}
}

void ListView::onPersonSelected(const Model::Person &person)
{
	if (m_OnResult) {
		m_OnResult({ person.getId() });
	}
}

void ListView::onSelectAllChecked()
{
	bool checked = static_cast<SelectAllItem *>(m_Sections[SectionSelectAll])->isChecked();

	for (auto &&group : m_PersonGroups) {
		for (auto &&personItem : *group.second) {
			static_cast<PersonItem*>(personItem)->setChecked(checked);
		}
	}

	m_CheckedCount = checked ? m_PersonCount : 0;
	updateTitle();
}

void ListView::onItemChecked(PersonItem *item)
{
	size_t checkCount = item->isChecked() ? ++m_CheckedCount : m_CheckedCount--;

	if (checkCount == m_PersonCount) {
		updateSelectAll();
	}

	updateTitle();
}

void ListView::onItemSelected(PersonItem *item)
{
	switch (m_Mode) {
		case ModeDefault:    launchPersonDetail(item); break;
		case ModeSinglePick: onPersonSelected(item->getPerson()); break;
		case ModeMultiPick:  onItemChecked(item); break;
		default: break;
	}
}
