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
#include "Contacts/List/ManageFavoritesPopup.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/MyProfileItem.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/List/PersonItem.h"
#include "Contacts/List/SearchItem.h"

#include "Contacts/Details/DetailsView.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Settings/MainView.h"
#include "Common/Strings.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "ListPath.h"
#include <efl_extension.h>

using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List;
using namespace Contacts::List::Model;

using namespace Utils;
using namespace Ux;
using namespace std::placeholders;

#define SYMBOL_MAGNIFIER "\U0001f50d"

ListView::ListView(int filterType)
	: m_Box(nullptr), m_NoContent(nullptr), m_Genlist(nullptr),
	  m_Index(nullptr), m_AddButton(nullptr), m_IsCurrentView(false),
	  m_SearchItem(nullptr),
	  m_Sections{ nullptr }, m_Provider(filterType)
{
	auto strings = Common::getSelectViewStrings();
	strings.titleDefault = "IDS_PB_TAB_CONTACTS";
	strings.titleSingle = "IDS_PB_HEADER_SELECT_CONTACT_ABB2";
	setStrings(strings);
}

ListView::~ListView()
{
	contacts_setting_remove_name_sorting_order_changed_cb(onNameSortingOrderChanged, this);
}

Evas_Object *ListView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Box = elm_box_add(layout);
	m_NoContent = createNoContentLayout(m_Box);
	m_Genlist = createGenlist(m_Box);
	elm_box_pack_end(m_Box, m_Genlist->getEvasObject());

	elm_object_part_content_set(layout, "elm.swallow.content", m_Box);
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	m_SearchItem = createSearchItem();
	m_AddButton = createAddButton(layout);

	return layout;
}

void ListView::onNavigation(bool isCurrent)
{
	m_IsCurrentView = isCurrent;
	updateAddButton();
}

void ListView::onCreated()
{
	updateSectionsMode();

	m_Provider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, _1));
	contacts_setting_add_name_sorting_order_changed_cb(onNameSortingOrderChanged, this);
}

void ListView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());
	menu->addItem("IDS_LOGS_OPT_DELETE", std::bind(&ListView::onDeleteSelected, this));
	menu->addItem("IDS_PB_OPT_SHARE", std::bind(&ListView::onShareSelected, this));
	menu->addItem("IDS_PB_OPT_MANAGE_FAVOURITES_ABB", [this] {
		auto manageFavPopup = new ManageFavoritesPopup(getNavigator());
		manageFavPopup->create(getEvasObject());
	});

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});

	menu->show();
}

void ListView::onDeleteSelected()
{
	auto strings = Common::getSelectViewStrings();
	strings.buttonDone = "IDS_LOGS_OPT_DELETE";

	ListView *view = new ListView();
	view->setStrings(strings);
	view->setSelectMode(SelectMulti);
	view->setSelectCallback([](SelectResults results) {
		std::vector<int> ids;
		ids.reserve(results.count());

		for (auto &&result : results) {
			ids.push_back(result.value.id);
		}

		contacts_db_delete_records(_contacts_person._uri, ids.data(), ids.size());
		return true;
	});
	getNavigator()->navigateTo(view);
}

void ListView::onShareSelected()
{
	auto strings = Common::getSelectViewStrings();
	strings.buttonDone = "IDS_PB_OPT_SHARE";

	ListView *view = new ListView();
	view->setStrings(strings);
	view->setSelectMode(SelectMulti);
	view->setSelectCallback([](SelectResults results) {
		size_t count = results.count();
		std::vector<std::string> idString(count);
		std::vector<const char *> ids(count);

		for (size_t i = 0; i < count; ++i) {
			idString[i] = std::to_string(results[i].value.id);
			ids[i] = idString[i].c_str();
		}

		App::AppControl request = App::requestMultiShareContacts(ids.data(), count);
		request.launch();
		request.detach();

		return true;
	});
	getNavigator()->navigateTo(view);
}

void ListView::onNameSortingOrderChanged(contacts_name_sorting_order_e sortingOrder, void *data)
{
	RETM_IF(!data, "invalid data");
	ListView *view = (ListView *)data;

	if (view->m_Sections[SectionPerson]) {
		for (auto &personGroup : view->m_PersonGroups) {
			delete personGroup.second;
		}
		view->m_PersonGroups.clear();
		view->m_Provider.clearContactDataList();

		elm_index_item_clear(view->m_Index);
		elm_index_level_go(view->m_Index, 0);

		view->m_Sections[SectionPerson] = nullptr;

		view->fillPersonList();
	}
}

void ListView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, m_SearchItem, Ui::Genlist::After);
}

void ListView::onSelectModeChanged(SelectMode selectMode)
{
	updateAddButton();
	updateSectionsMode();
}

void ListView::fillMyProfile()
{
	if (!m_Sections[SectionMyProfile]) {
		insertMyProfileGroupItem();
		insertMyProfileItem();
	}
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
		PersonGroupItem *group = nullptr;

		for (auto &&contactData : m_Provider.getContactDataList()) {
			Person &person = static_cast<Person &>(*contactData);

			const UniString &nextLetter = person.getIndexLetter();
			if (!group || group->getTitle() != nextLetter) {
				group = getPersonGroupItem(nextLetter);
			}

			PersonItem *item = createPersonItem(person);
			m_Genlist->insert(item, group);
			onItemInserted(item);
		}

		updateNoContentLayout();
	}
}

void ListView::setIndexState(Evas_Object *layout, bool state)
{
	const char *signal = state ? "elm,state,fastscroll,show" : "elm,state,fastscroll,hide";
	elm_layout_signal_emit(layout, signal, "");
}

Ui::Genlist *ListView::createGenlist(Evas_Object *parent)
{
	Ui::Genlist *genlist = new Ui::Genlist();
	genlist->create(parent);

	evas_object_size_hint_weight_set(genlist->getEvasObject(), EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist->getEvasObject(), EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(genlist->getEvasObject());

	return genlist;
}

Evas_Object *ListView::createNoContentLayout(Evas_Object *parent)
{
	Evas_Object *noContent = elm_layout_add(parent);
	elm_layout_theme_set(noContent, "layout", "nocontents", "default");

	evas_object_size_hint_weight_set(noContent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContent, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_translatable_part_text_set(noContent, "elm.text", "IDS_PB_NPBODY_NO_CONTACTS");
	elm_object_translatable_part_text_set(noContent, "elm.help.text", "IDS_PB_BODY_AFTER_YOU_CREATE_CONTACTS_THEY_WILL_BE_SHOWN_HERE");

	elm_layout_signal_emit(noContent, "text,disabled", "");
	elm_layout_signal_emit(noContent, "align.center", "elm");

	return noContent;
}

void ListView::showNoContentLayout()
{
	if (!evas_object_visible_get(m_NoContent)) {
		setIndexState(getEvasObject(), false);

		elm_scroller_content_min_limit(m_Genlist->getEvasObject(), EINA_FALSE, EINA_TRUE);
		evas_object_size_hint_weight_set(m_Genlist->getEvasObject(), EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(m_Genlist->getEvasObject(), EVAS_HINT_FILL, 0.0);

		evas_object_show(m_NoContent);
		elm_box_pack_end(m_Box, m_NoContent);
	}
}

void ListView::hideNoContentLayout()
{
	if (evas_object_visible_get(m_NoContent)) {
		setIndexState(getEvasObject(), true);

		elm_scroller_content_min_limit(m_Genlist->getEvasObject(), EINA_FALSE, EINA_FALSE);
		evas_object_size_hint_weight_set(m_Genlist->getEvasObject(), EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(m_Genlist->getEvasObject(), EVAS_HINT_FILL, EVAS_HINT_FILL);

		evas_object_hide(m_NoContent);
		elm_box_unpack(m_Box, m_NoContent);
	}
}

void ListView::updateNoContentLayout()
{
	ContactDataList list = m_Provider.getContactDataList();
	list.size() > 0 ? hideNoContentLayout() : showNoContentLayout();
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

SearchItem *ListView::createSearchItem()
{
	SearchItem *item = new SearchItem();
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
	elm_genlist_item_select_mode_set(item->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	Elm_Object_Item *indexItem = insertIndexItem(SYMBOL_MAGNIFIER, nullptr);
	elm_object_item_data_set(indexItem, item->getObjectItem());
	return item;
}

Evas_Object *ListView::createAddButton(Evas_Object *parent)
{
	Evas_Object *floatButton = eext_floatingbutton_add(parent);
	Evas_Object *button = elm_button_add(floatButton);
	elm_object_part_content_set(floatButton, "button1", button);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&ListView::onAddPressed), this);

	Evas_Object *image = elm_image_add(button);
	elm_image_file_set(image, App::getResourcePath(LIST_ADD_BUTTON_ICON).c_str(), nullptr);
	elm_object_part_content_set(button, "icon", image);

	return floatButton;
}

void ListView::updateAddButton()
{
	Ui::Window *window = getWindow();
	if (!window) {
		return;
	}

	if (m_IsCurrentView && getSelectMode() == SelectNone) {
		elm_object_part_content_set(window->getBaseLayout(), "elm.swallow.floatingbutton", m_AddButton);
	} else {
		elm_object_part_content_unset(window->getBaseLayout(), "elm.swallow.floatingbutton");
		evas_object_hide(m_AddButton);
	}
}

void ListView::insertMyProfileGroupItem()
{
	auto group = new GroupItem("IDS_PB_HEADER_ME");
	m_Genlist->insert(group, nullptr, getNextSectionItem(SectionMyProfile));
	elm_genlist_item_select_mode_set(group->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	m_Sections[SectionMyProfile] = group;
}

void ListView::insertMyProfileItem()
{
	MyProfileItem *item = new MyProfileItem(MyProfilePtr(new MyProfile()));
	m_Genlist->insert(item, m_Sections[SectionMyProfile]);

	item->setSelectCallback([this, item]() {
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
	Elm_Object_Item *indexItem = nextItem
			? elm_index_item_insert_before(m_Index, nextItem, indexLetter, nullptr, nullptr)
			: elm_index_item_append(m_Index, indexLetter, nullptr, nullptr);
	elm_index_level_go(m_Index, 0);
	return indexItem;
}

PersonGroupItem *ListView::getPersonGroupItem(const Utils::UniString &indexLetter)
{
	auto insertResult = m_PersonGroups.insert({ indexLetter, nullptr });
	auto groupIt = insertResult.first;

	/* Whether insert was successful (i.e. there is no such group yet) */
	if (insertResult.second) {
		PersonGroupItem *nextGroupItem = nullptr;
		auto nextGroupIt = Utils::advance(groupIt, 1);
		if (nextGroupIt != m_PersonGroups.end()) {
			nextGroupItem = nextGroupIt->second;
		}

		groupIt->second = insertPersonGroupItem(indexLetter, nextGroupItem);
		m_Sections[SectionPerson] = m_PersonGroups.begin()->second;
	}

	return groupIt->second;
}

PersonGroupItem *ListView::insertPersonGroupItem(const Utils::UniString &indexLetter,
		PersonGroupItem *nextGroupItem)
{
	Elm_Object_Item *nextIndexItem = nextGroupItem ? nextGroupItem->getIndexItem() : nullptr;
	Elm_Object_Item *indexItem = insertIndexItem(indexLetter.getUtf8Str().c_str(), nextIndexItem);

	PersonGroupItem *item = new PersonGroupItem(indexLetter);
	item->setIndexItem(indexItem);
	m_Genlist->insert(item, nullptr, nextGroupItem);
	elm_genlist_item_select_mode_set(item->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	return item;
}

void ListView::deletePersonGroupItem(PersonGroupItem *group)
{
	m_PersonGroups.erase(group->getTitle());
	delete group;
	elm_index_level_go(m_Index, 0);
	m_Sections[SectionPerson] = !m_PersonGroups.empty() ? m_PersonGroups.begin()->second : nullptr;
}

PersonItem *ListView::createPersonItem(Person &person)
{
	PersonItem *item = new PersonItem(person);
	person.setUpdateCallback(std::bind(&ListView::onPersonUpdated, this, item, _1));
	person.setDeleteCallback(std::bind(&ListView::onPersonDeleted, this, item));
	return item;
}

void ListView::insertPersonItem(PersonItem *item)
{
	Person &person = item->getPerson();
	PersonGroupItem *groupItem = getPersonGroupItem(person.getIndexLetter());
	PersonItem *nextItem = getNextPersonItem(groupItem, person);

	m_Genlist->insert(item, groupItem, nextItem);
	updateNoContentLayout();
}

void ListView::updatePersonItem(PersonItem *item, int changes)
{
	if (changes & Person::ChangedSortValue) {
		PersonGroupItem *oldGroup = static_cast<PersonGroupItem *>(item->getParentItem());

		item->pop();
		insertPersonItem(item);

		if (oldGroup->empty()) {
			deletePersonGroupItem(oldGroup);
		}
	} else {
		item->update(changes);
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

	updateNoContentLayout();
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
	PersonGroupItem *groupItem = (PersonGroupItem *) elm_object_item_data_get(indexItem);
	groupItem->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void ListView::onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem)
{
	onIndexChanged(index, indexItem);
	elm_index_item_selected_set(indexItem, EINA_FALSE);
}

void ListView::onPersonInserted(ContactData &person)
{
	auto item = createPersonItem(static_cast<Person &>(person));
	insertPersonItem(item);
	onItemInserted(item);
}

void ListView::onPersonUpdated(PersonItem *item, int changes)
{
	updatePersonItem(item, changes);
}

void ListView::onPersonDeleted(PersonItem *item)
{
	deletePersonItem(item);
}
