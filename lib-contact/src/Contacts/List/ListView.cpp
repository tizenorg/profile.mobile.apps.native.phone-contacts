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
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/MyProfileItem.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/List/PersonItem.h"
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
using namespace std::placeholders;

ListView::ListView(int filterType)
	: m_Genlist(nullptr), m_Index(nullptr),
	  m_AddButton(nullptr), m_DoneButton(nullptr), m_CancelButton(nullptr),
	  m_Sections{ nullptr },
	  m_Provider(filterType),
	  m_PersonCount(0), m_SelectCount(0), m_SelectLimit(0),
	  m_SelectMode(SelectNone)
{
}

ListView::~ListView()
{
	m_Provider.unsetInsertCallback();
	contacts_db_remove_changed_cb(_contacts_my_profile._uri,
			makeCallbackWithLastParam(&ListView::updateMyProfileItem), this);
}

void ListView::setSelectMode(SelectMode selectMode)
{
	if (m_SelectMode != selectMode) {
		m_SelectMode = selectMode;

		updatePageMode();
		updateSectionsMode();
	}
}

void ListView::setSelectLimit(size_t count)
{
	if (m_SelectLimit != count) {
		m_SelectLimit = count;

		updateTitle();
		updateSectionsMode();
	}
}

void ListView::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
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
	if (m_SelectMode != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		ListView *deleteView = new ListView();
		getNavigator()->navigateTo(deleteView);
		deleteView->setSelectMode(SelectMulti);
		deleteView->setSelectCallback([](SelectResults results) {
			std::vector<int> ids;
			for (auto &&result : results) {
				ids.push_back(result.itemId);
			}

			contacts_db_delete_records(_contacts_person._uri, ids.data(), ids.size());
			return true;
		});
	});

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	menu->show();
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
		item->setCheckCallback(std::bind(&ListView::onSelectAllChecked, this, _1));

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
		setFavouriteItemsMode(m_SelectMode);
	}
}

void ListView::fillMfc()
{
	//Todo
}

void ListView::fillPersonList()
{
	if (m_PersonGroups.empty()) {
		ContactDataList list = m_Provider.getContactDataList();
		PersonGroupItem *group = nullptr;

		for (auto &&contactData : list) {
			auto person = static_cast<Person *>(contactData);
			const UniString &nextLetter = person->getIndexLetter();

			if (!group || group->getTitle() != nextLetter) {
				group = insertPersonGroupItem(nextLetter);
			}

			m_Genlist->insert(createPersonItem(*person), group);

			++m_PersonCount;
		}
	} else {
		setPersonItemsMode(m_SelectMode);
	}
}

void ListView::setFavouriteItemsMode(SelectMode selectMode)
{
	if (m_Sections[SectionFavorites]) {
		auto group = dynamic_cast<Ui::GenlistGroupItem *>(m_Sections[SectionFavorites]);
		for (auto &&favoriteItem : *group) {
			static_cast<PersonItem *>(favoriteItem)->setSelectMode(selectMode);
		}
	}
}

void ListView::setPersonItemsMode(SelectMode selectMode)
{
	for (auto &&group : m_PersonGroups) {
		for (auto &&personItem : *group.second) {
			static_cast<PersonItem *>(personItem)->setSelectMode(selectMode);
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

bool ListView::getSectionVisibility(SelectMode selectMode, SectionId sectionId)
{
	static bool sectionVisibility[][ListView::SectionMax] = {
		/* SelectNone   = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ false,
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ true,
			/* SectionPerson    = */ true
		},
		/* SelectSingle = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ false,
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		},
		/* SelectMulti  = */ {
			/* SectionSearch    = */ true,
			/* SectionSelectAll = */ true,
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ false,
			/* SectionPerson    = */ true
		}
	};

	if (m_SelectLimit && selectMode == SelectMulti && sectionId == SectionSelectAll) {
		return false;
	}

	return sectionVisibility[selectMode][sectionId];
}

void ListView::updateTitle()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	switch (m_SelectMode) {
		case SelectNone:
			page->setTitle("IDS_PB_TAB_CONTACTS");
			break;

		case SelectSingle:
			page->setTitle("IDS_PB_HEADER_SELECT_CONTACT_ABB2");
			break;

		case SelectMulti:
		{
			char title[TITLE_SIZE];
			if (m_SelectLimit) {
				snprintf(title, TITLE_SIZE, "%zu/%zu", m_SelectCount, m_SelectLimit);
			} else {
				snprintf(title, TITLE_SIZE, _("IDS_PB_HEADER_PD_SELECTED_ABB"), m_SelectCount);
			}
			page->setTitle(title);
		}
			break;
	}
}

void ListView::updateSelectAllState()
{
	auto selectAllItem = static_cast<SelectAllItem *>(m_Sections[SectionSelectAll]);
	if (selectAllItem) {
		selectAllItem->setChecked(m_SelectCount == m_PersonCount);
	}
}

void ListView::updatePageMode()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	updateTitle();

	switch (m_SelectMode) {
		case SelectNone:
			deletePageButtons();
			createAddButton();
			page->setContent("toolbar", m_AddButton);
			break;

		case SelectSingle:
			deletePageButtons();
			deleteAddButton();
			break;

		case SelectMulti:
			createPageButtons();
			deleteAddButton();
			page->setContent(PART_TITLE_RIGHT, m_DoneButton);
			page->setContent(PART_TITLE_LEFT, m_CancelButton);
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
		if (getSectionVisibility(m_SelectMode, sectionId)) {
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

void ListView::createPageButtons()
{
	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&ListView::onDonePressed), this);

	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, "IDS_PB_BUTTON_CANCEL");
	evas_object_smart_callback_add(m_CancelButton, "clicked",
			makeCallback(&ListView::onCancelPressed), this);
}

void ListView::deletePageButtons()
{
	evas_object_del(m_DoneButton);
	evas_object_del(m_CancelButton);

	m_DoneButton = nullptr;
	m_CancelButton = nullptr;
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

PersonItem *ListView::createPersonItem(Person &person)
{
	PersonItem *item = new PersonItem(person);
	item->setSelectMode(m_SelectMode);

	person.setUpdateCallback(std::bind(&ListView::onPersonUpdated, this, item, _1));
	person.setDeleteCallback(std::bind(&ListView::onDeleted, this, item));

	item->setSelectCallback(std::bind(&ListView::onItemSelected, this, item));
	item->setCheckCallback(std::bind(&ListView::onItemChecked, this, item, _1));

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

void ListView::updatePersonItem(PersonItem *item, int changes)
{
	if (changes & ContactData::ChangedName) {
		PersonGroupItem *oldGroup = static_cast<PersonGroupItem *>(item->getParentItem());

		item->pop();
		insertPersonItem(item);

		if (oldGroup->empty()) {
			deletePersonGroupItem(oldGroup);
		}
	} else if (changes & ContactData::ChangedImage) {
		elm_genlist_item_fields_update(item->getObjectItem(),
				PART_PERSON_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
	}
}

void ListView::deletePersonItem(PersonItem *item)
{
	PersonGroupItem *oldGroup = static_cast<PersonGroupItem *>(item->getParentItem());

	item->getPerson().unsetUpdateCallback();
	item->getPerson().unsetDeleteCallback();
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

void ListView::onAddPressed(Evas_Object *button, void *eventInfo)
{
	getNavigator()->navigateTo(new Input::InputView());
}

void ListView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	std::vector<SelectResult> results;
	for (auto &&group : m_PersonGroups) {
		for (auto &&item : *group.second) {
			PersonItem *personItem = static_cast<PersonItem *>(item);
			if (personItem->isChecked()) {
				results.push_back({ ResultPerson, personItem->getPerson().getId() });
			}
		}
	}

	if (m_OnSelected && m_OnSelected({ results.data(), results.size() })) {
		delete this;
	}
}

void ListView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	delete this;
}

void ListView::onPersonItemInserted(PersonItem *item)
{
	++m_PersonCount;

	updateSelectAllState();
	updateTitle();
}

void ListView::onPersonItemDelete(PersonItem *item)
{
	--m_PersonCount;
	if (item->isChecked()) {
		--m_SelectCount;
	}

	updateSelectAllState();
	updateTitle();
}

void ListView::onPersonInserted(ContactData &person)
{
	auto item = createPersonItem(static_cast<Person &>(person));
	insertPersonItem(item);
	onPersonItemInserted(item);
}

void ListView::onPersonUpdated(PersonItem *item, int changes)
{
	updatePersonItem(item, changes);
}

void ListView::onDeleted(PersonItem *item)
{
	onPersonItemDelete(item);
	deletePersonItem(item);
}

void ListView::onPersonSelected(const Model::Person &person)
{
	SelectResult result = { ResultPerson, person.getId() };
	if (m_OnSelected && m_OnSelected({ &result, 1 })) {
		delete this;
	}
}

void ListView::onSelectAllChecked(bool isChecked)
{
	for (auto &&group : m_PersonGroups) {
		for (auto &&personItem : *group.second) {
			static_cast<PersonItem *>(personItem)->setChecked(isChecked);
		}
	}

	m_SelectCount = isChecked ? m_PersonCount : 0;
	updateTitle();
}

void ListView::onItemChecked(PersonItem *item, bool isChecked)
{
	if (m_SelectLimit && m_SelectCount == m_SelectLimit && isChecked) {
		item->setChecked(false);
		return;
	}

	size_t checkCount = isChecked ? ++m_SelectCount : m_SelectCount--;
	if (checkCount == m_PersonCount) {
		updateSelectAllState();
	}

	updateTitle();
}

void ListView::onItemSelected(PersonItem *item)
{
	switch (m_SelectMode) {
		case SelectNone:    launchPersonDetail(item); break;
		case SelectSingle:  onPersonSelected(item->getPerson()); break;
		default: break;
	}
}
