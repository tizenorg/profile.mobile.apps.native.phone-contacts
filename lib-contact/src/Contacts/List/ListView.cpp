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
#include "Contacts/List/ContactItem.h"
#include "Contacts/List/ContactGroupItem.h"
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

ListView::ListView()
	: m_Genlist(nullptr), m_Index(nullptr),
	  m_Provider(ContactProvider(ContactProvider::FilterNone))
{
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

	m_Provider.setInsertCallback(std::bind(&ListView::onContactInserted, this, std::placeholders::_1));
}

void ListView::fillList()
{
	ContactList list = m_Provider.getContactList();

	const char *currentLetter = nullptr;

	for (auto &&contact : list) {
		ContactGroupItem *group = nullptr;
		const char *nextLetter = contact->getIndexLetter();

		if (!currentLetter || strcmp(currentLetter, nextLetter) != 0) {
			group = insertGroupItem(nextLetter);
			currentLetter = nextLetter;
		}

		insertItem(std::move(contact), group);
	}
}

ContactGroupItem *ListView::insertGroupItem(const char *indexLetter, ContactGroupItem *nextGroup)
{
	Elm_Object_Item *indexItem = nullptr;

	if (nextGroup) {
		indexItem = elm_index_item_insert_before(m_Index, nextGroup->getIndexItem(), indexLetter, nullptr, nullptr);
	} else {
		indexItem = elm_index_item_append(m_Index, indexLetter, nullptr, nullptr);
	}
	elm_index_level_go(indexItem, 0);

	ContactGroupItem *item = new ContactGroupItem(indexLetter, indexItem);
	auto position = nextGroup ? Ui::Genlist::Before : Ui::Genlist::After;
	m_Genlist->insert(item, nullptr, nextGroup, position);

	elm_object_item_data_set(indexItem, item->getObjectItem());
	m_Groups.insert({ indexLetter, item });

	return item;
}

ContactGroupItem *ListView::getNextGroupItem(const char *indexLetter)
{
	auto it = m_Groups.lower_bound(indexLetter);
	if (it != m_Groups.end()) {
		return it->second;
	}

	return nullptr;
}

ContactItem *ListView::insertItem(Model::ContactPtr contact, ContactGroupItem *group, ContactItem *nextItem)
{
	auto position = nextItem ? Ui::Genlist::Before : Ui::Genlist::After;
	ContactItem *item = new ContactItem(std::move(contact));
	m_Genlist->insert(item, group, nextItem, position);

	return item;
}

ContactItem *ListView::getNextItem(ContactGroupItem *group, const Contact &contact)
{
	ContactItem *item = (ContactItem *)group->getNextItem();
	while (item && item->getParentItem() == group) {
		if (item->getContact() < contact) {
			item = (ContactItem *)item->getNextItem();
		} else {
			break;
		}
	}

	return item;
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
	//TODO: getNavigator()->navigateTo(new Contacts::Input::MainView());
}

Evas_Object *ListView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	return menu->getEvasObject();
}

void ListView::onContactInserted(ContactPtr contact)
{
	const char *indexLetter = contact->getIndexLetter();
	auto it = m_Groups.find(indexLetter);

	if (it != m_Groups.end()) {
		ContactItem *nextItem = getNextItem(it->second, *contact);
		insertItem(std::move(contact), it->second, nextItem);
	} else {
		ContactGroupItem *group = insertGroupItem(indexLetter, getNextGroupItem(indexLetter));
		insertItem(std::move(contact), group);
	}
}
