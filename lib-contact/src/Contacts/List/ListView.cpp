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
#include "Contacts/List/PersonItem.h"
#include "Contacts/List/PersonGroupItem.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/Settings/MainView.h"
#include "Contacts/Utils.h"

#include "Ui/Genlist.h"
#include "Ui/GenlistItem.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Utils/Callback.h"

using namespace Contacts;
using namespace Contacts::List;
using namespace Contacts::List::Model;
using namespace Utils;

ListView::ListView()
	: m_Genlist(nullptr), m_Index(nullptr),
	  m_Provider(PersonProvider(PersonProvider::FilterNone))
{
}

Evas_Object *ListView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Genlist = new Ui::Genlist();
	elm_object_part_content_set(layout, "elm.swallow.content", m_Genlist->create(layout));
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	evas_object_smart_callback_add(m_Genlist->getEvasObject(), "selected",
			(Evas_Smart_Cb) makeCallback(&ListView::onItemSelected), this);

	return layout;
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

void ListView::onPageAttached()
{
	Evas_Object *button = elm_button_add(getEvasObject());
	elm_object_style_set(button, "bottom");
	elm_object_translatable_text_set(button, "IDS_PB_OPT_CREATE");
	evas_object_smart_callback_add(button, "clicked",
			(Evas_Smart_Cb) makeCallback(&ListView::onCreatePressed), this);

	getPage()->setTitle("IDS_PB_TAB_CONTACTS");
	getPage()->setContent("toolbar", button);
}

void ListView::onCreated()
{
	fillList();

	m_Provider.setInsertCallback(std::bind(&ListView::onPersonInserted, this, std::placeholders::_1));
}

void ListView::fillList()
{
	PersonList list = m_Provider.getPersonList();
	PersonGroupItem *group = nullptr;

	for (auto &&person : list) {
		const UniString &nextLetter = person->getIndexLetter();

		if (!group || group->getTitle() != nextLetter) {
			group = insertGroupItem(nextLetter);
		}

		m_Genlist->insert(createPersonItem(std::move(person)), group);
	}
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

PersonGroupItem *ListView::insertGroupItem(UniString indexLetter, PersonGroupItem *nextGroup)
{
	Elm_Object_Item *indexItem = insertIndexItem(indexLetter.getUtf8Str().c_str(),
			nextGroup ? nextGroup->getIndexItem() : nullptr);

	PersonGroupItem *item = new PersonGroupItem(std::move(indexLetter), indexItem);
	m_Genlist->insert(item, nullptr, nextGroup);
	elm_genlist_item_select_mode_set(item->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);

	elm_object_item_data_set(indexItem, item->getObjectItem());
	m_Groups.insert({ item->getTitle(), item });

	return item;
}

void ListView::deleteGroupItem(PersonGroupItem *group)
{
	elm_object_item_del(group->getIndexItem());
	elm_index_level_go(m_Index, 0);

	m_Groups.erase(group->getTitle());
	delete group;
}

PersonGroupItem *ListView::getNextGroupItem(const Utils::UniString &indexLetter)
{
	auto it = m_Groups.lower_bound(indexLetter);
	if (it != m_Groups.end()) {
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

	return item;
}

void ListView::insertPersonItem(PersonItem *item)
{
	PersonGroupItem *group = nullptr;
	PersonItem *nextItem = nullptr;
	const UniString &indexLetter = item->getPerson().getIndexLetter();

	auto it = m_Groups.find(indexLetter);
	if (it != m_Groups.end()) {
		group = it->second;
		nextItem = getNextPersonItem(it->second, item->getPerson());
	} else {
		PersonGroupItem *nextGroup = getNextGroupItem(indexLetter);
		group = insertGroupItem(indexLetter, nextGroup);
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
			deleteGroupItem(oldGroup);
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
		deleteGroupItem(oldGroup);
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

void ListView::onItemSelected(Evas_Object *genlist, Elm_Object_Item *genlistItem)
{
	PersonItem *item = (PersonItem *) elm_object_item_data_get(genlistItem);

	int id = 0;
	contacts_record_get_int(item->getPerson().getRecord(), _contacts_person.display_contact_id, &id);

	getNavigator()->navigateTo(new Input::InputView(id));
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
