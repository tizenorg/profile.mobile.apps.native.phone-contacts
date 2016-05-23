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
#include "Contacts/List/ListSection.h"
#include "Contacts/List/ManageFavoritesPopup.h"

#include "Contacts/List/Model/FavoritesProvider.h"
#include "Contacts/List/Model/MfcProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/Model/PersonProvider.h"
#include "Contacts/List/Model/PersonSearchData.h"

#include "Contacts/List/MyProfileGroup.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/List/PersonSearchItem.h"
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

ListView::ListView(Model::PersonProvider *provider)
	: m_Box(nullptr), m_NoContent(nullptr), m_Genlist(nullptr),
	  m_Index(nullptr), m_AddButton(nullptr), m_IsCurrentView(false), m_IsSearching(false),
	  m_SearchItem(nullptr), m_Sections{ nullptr },
	  m_PersonProvider(provider), m_SearchProvider(*m_PersonProvider)
{
	auto strings = Common::getSelectViewStrings();
	strings.titleDefault = "IDS_PB_TAB_CONTACTS";
	strings.titleSingle = "IDS_PB_HEADER_SELECT_CONTACT_ABB2";
	setStrings(strings);
}

ListView::ListView(int filterType)
	: ListView(new PersonProvider(filterType))
{
}

ListView::~ListView()
{
	contacts_setting_remove_name_sorting_order_changed_cb(
			makeCallbackWithLastParam(&ListView::onSortOrderChanged), this);
	delete m_PersonProvider;
}

Evas_Object *ListView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Box = elm_box_add(layout);
	m_NoContent = createEmptyLayout(m_Box);
	m_Genlist = createGenlist(m_Box);
	elm_box_pack_end(m_Box, m_Genlist->getEvasObject());

	elm_object_part_content_set(layout, "elm.swallow.content", m_Box);
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	m_SearchItem = createSearchItem();
	m_AddButton = createAddButton(layout);

	return layout;
}

void ListView::onCreated()
{
	updateSectionsMode();
	fillPersonList();

	m_SearchProvider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, _1));
	contacts_setting_add_name_sorting_order_changed_cb(
			makeCallbackWithLastParam(&ListView::onSortOrderChanged), this);
}

void ListView::onDestroy()
{
	for (auto &&section : m_Sections) {
		if (section && !section->isInserted()) {
			delete section;
		}
	}
}

void ListView::onNavigation(bool isCurrent)
{
	m_IsCurrentView = isCurrent;
	updateAddButton();
	m_PersonProvider->setUpdateMode(isCurrent);
}

bool ListView::onBackPressed()
{
	if (m_IsSearching) {
		m_SearchItem->clear();
		return false;
	}

	return true;
}

void ListView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	if (!m_PersonGroups.empty()) {
		menu->addItem("IDS_LOGS_OPT_DELETE", std::bind(&ListView::onDeleteSelected, this));
		menu->addItem("IDS_PB_OPT_SHARE", std::bind(&ListView::onShareSelected, this));
		menu->addItem("IDS_PB_OPT_MANAGE_FAVOURITES_ABB", [this] {
			auto manageFavPopup = new ManageFavoritesPopup(getNavigator());
			manageFavPopup->create(getEvasObject());
		});
	}

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

void ListView::onSortOrderChanged(contacts_name_sorting_order_e order)
{
	if (!m_PersonGroups.empty()) {
		for (auto &personGroup : m_PersonGroups) {
			delete personGroup.second;
		}

		m_PersonGroups.clear();
		m_SearchProvider.clearDataList();

		elm_index_item_clear(m_Index);
		elm_index_level_go(m_Index, 0);

		fillPersonList();
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

Ui::GenlistGroupItem *ListView::createMyProfileSection()
{
	return new MyProfileGroup();
}

Ui::GenlistGroupItem *ListView::createFavoritesSection()
{
	ListSection *favoritesSection = new ListSection("IDS_PB_HEADER_FAVOURITES", new FavoritesProvider());
	favoritesSection->setUpdateCallback(std::bind(&ListView::onSectionUpdated, this, _1, SectionFavorites));
	return favoritesSection;
}

Ui::GenlistGroupItem *ListView::createMfcSection()
{
	ListSection *mfcSection = new ListSection("IDS_PB_HEADER_MOST_FREQUENT_CONTACTS_ABB2", new MfcProvider());
	mfcSection->setUpdateCallback(std::bind(&ListView::onSectionUpdated, this, _1, SectionMfc));
	return mfcSection;
}

void ListView::fillPersonList()
{
	if (m_PersonGroups.empty()) {
		PersonGroupItem *group = nullptr;

		for (auto &&contactData : m_SearchProvider.getDataList()) {
			PersonSearchData &searchData = static_cast<PersonSearchData &>(*contactData);
			Person &person = searchData.getPerson();

			const UniString &nextLetter = person.getIndexLetter();
			if (!group || group->getTitle() != nextLetter) {
				group = getPersonGroupItem(nextLetter);
			}

			PersonItem *item = createPersonItem(searchData);
			m_Genlist->insert(item, group);
			onItemInserted(item);
		}

		if (m_PersonGroups.empty()) {
			setEmptyState(true);
		}
	}
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

Evas_Object *ListView::createEmptyLayout(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "nocontents", "default");

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_translatable_part_text_set(layout, "elm.text", "IDS_PB_NPBODY_NO_CONTACTS");
	elm_object_translatable_part_text_set(layout, "elm.help.text", "IDS_PB_BODY_AFTER_YOU_CREATE_CONTACTS_THEY_WILL_BE_SHOWN_HERE");

	elm_layout_signal_emit(layout, "text,disabled", "");
	elm_layout_signal_emit(layout, "align.center", "elm");

	return layout;
}

void ListView::setEmptyState(bool isEmpty)
{
	setIndexState(!isEmpty);

	Evas_Object *genlist = m_Genlist->getEvasObject();
	if (isEmpty) {
		elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);
		evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, 0.0);

		evas_object_show(m_NoContent);
		elm_box_pack_end(m_Box, m_NoContent);
	} else {
		elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_FALSE);
		evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

		evas_object_hide(m_NoContent);
		elm_box_unpack(m_Box, m_NoContent);
	}
}

void ListView::setIndexState(bool isVisible)
{
	const char *signal = isVisible ? "elm,state,fastscroll,show" : "elm,state,fastscroll,hide";
	elm_layout_signal_emit(getEvasObject(), signal, "");
}

void ListView::addSection(SectionId sectionId)
{
	Ui::GenlistGroupItem *group = m_Sections[sectionId];
	if (!group) {
		switch (sectionId) {
			case SectionMyProfile: group = createMyProfileSection(); break;
			case SectionFavorites: group = createFavoritesSection(); break;
			case SectionMfc:       group = createMfcSection();       break;
			default: break;
		}

		m_Sections[sectionId] = group;
	}

	if (group && !group->empty() && !group->isInserted()) {
		m_Genlist->insert(group, nullptr, getNextSectionItem(sectionId));
		elm_genlist_item_select_mode_set(group->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	}
}

void ListView::removeSection(SectionId sectionId)
{
	if (m_Sections[sectionId]) {
		m_Sections[sectionId]->pop();
	}
}

Ui::GenlistItem *ListView::getNextSectionItem(SectionId sectionId)
{
	for (unsigned section = sectionId + 1; section < SectionMax; ++section) {
		if (m_Sections[section] && m_Sections[section]->isInserted()) {
			return m_Sections[section];
		}
	}

	return !m_PersonGroups.empty() ? m_PersonGroups.begin()->second : nullptr;
}

bool ListView::getSectionVisibility(SelectMode selectMode, SectionId sectionId)
{
	static bool sectionVisibility[][ListView::SectionMax] = {
		/* SelectNone   = */ {
			/* SectionMyProfile = */ true,
			/* SectionFavorites = */ true,
			/* SectionMfc       = */ true
		},
		/* SelectSingle = */ {
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ false, /* FIXME: Enable when sections will be supported in selection mode */
			/* SectionMfc       = */ false
		},
		/* SelectMulti  = */ {
			/* SectionMyProfile = */ false,
			/* SectionFavorites = */ false, /* FIXME: Enable when sections will be supported in selection mode */
			/* SectionMfc       = */ false
		}
	};

	return !m_IsSearching ? sectionVisibility[selectMode][sectionId] : false;
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
	item->setChangeCallback(std::bind(&ListView::onSearchChanged, this, _1));

	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
	elm_genlist_item_select_mode_set(item->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	Elm_Object_Item *indexItem = insertIndexItem(SYMBOL_MAGNIFIER, nullptr);
	elm_object_item_data_set(indexItem, static_cast<Ui::GenlistItem *>(item));
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
	Ui::Window *window = findParent<Ui::Window>();
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
}

PersonItem *ListView::createPersonItem(PersonSearchData &searchData)
{
	PersonSearchItem *item = new PersonSearchItem(searchData.getPerson(), &searchData);
	searchData.setUpdateCallback(std::bind(&ListView::updatePersonItem, this, item, _1));
	searchData.setDeleteCallback(std::bind(&ListView::deletePersonItem, this, item));
	return item;
}

void ListView::insertPersonItem(PersonItem *item)
{
	if (m_PersonGroups.empty()) {
		setEmptyState(false);
	}

	Person &person = item->getPerson();
	PersonGroupItem *groupItem = getPersonGroupItem(person.getIndexLetter());
	PersonItem *nextItem = getNextPersonItem(groupItem, person);

	m_Genlist->insert(item, groupItem, nextItem);
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
	delete item;

	if (oldGroup->empty()) {
		deletePersonGroupItem(oldGroup);
	}

	if (m_PersonGroups.empty()) {
		setEmptyState(true);
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

void ListView::onAddPressed(Evas_Object *button, void *eventInfo)
{
	getNavigator()->navigateTo(new Input::InputView());
}

void ListView::onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem)
{
	Ui::GenlistItem *item = (Ui::GenlistItem *) elm_object_item_data_get(indexItem);
	item->scrollTo(ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void ListView::onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem)
{
	onIndexChanged(index, indexItem);
	elm_index_item_selected_set(indexItem, EINA_FALSE);
}

void ListView::onPersonInserted(ContactData &contactData)
{
	auto item = createPersonItem(static_cast<PersonSearchData &>(contactData));
	insertPersonItem(item);
	onItemInserted(item);
}

void ListView::onSectionUpdated(bool isEmpty, SectionId sectionId)
{
	Ui::GenlistItem *section = m_Sections[sectionId];
	if (isEmpty) {
		section->pop();
	} else {
		m_Genlist->insert(section, nullptr, getNextSectionItem(sectionId));
		elm_genlist_item_select_mode_set(section->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
	}
}

void ListView::onSearchChanged(const char *str)
{
	bool isSearching = str && *str;
	if (isSearching != m_IsSearching) {
		m_IsSearching = isSearching;
		updateSectionsMode();
	}

	elm_genlist_filter_set(m_Genlist->getEvasObject(), (void *) str);
}
