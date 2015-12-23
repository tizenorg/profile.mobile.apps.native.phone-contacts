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

	m_Provider.setInsertCallback(std::bind(&ListView::onContactInserted, this, std::placeholders::_1));
}

void ListView::fillList()
{
	ContactList list = m_Provider.getContactList();
	ContactGroupItem *group = nullptr;

	for (auto &&contact : list) {
		const UniString &nextLetter = contact->getIndexLetter();

		if (!group || group->getTitle() != nextLetter) {
			group = insertGroupItem(nextLetter);
		}

		m_Genlist->insert(createContactItem(std::move(contact)), group);
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

ContactGroupItem *ListView::insertGroupItem(UniString indexLetter, ContactGroupItem *nextGroup)
{
	Elm_Object_Item *indexItem = insertIndexItem(indexLetter.getUtf8Str().c_str(),
			nextGroup ? nextGroup->getIndexItem() : nullptr);

	ContactGroupItem *item = new ContactGroupItem(std::move(indexLetter), indexItem);
	m_Genlist->insert(item, nullptr, nextGroup);

	elm_object_item_data_set(indexItem, item->getObjectItem());
	m_Groups.insert({ item->getTitle(), item });

	return item;
}

void ListView::deleteGroupItem(ContactGroupItem *group)
{
	m_Groups.erase(group->getTitle());
	delete group;
}

ContactGroupItem *ListView::getNextGroupItem(const Utils::UniString &indexLetter)
{
	auto it = m_Groups.lower_bound(indexLetter);
	if (it != m_Groups.end()) {
		return it->second;
	}

	return nullptr;
}

ContactItem *ListView::createContactItem(ContactPtr contact)
{
	using namespace std::placeholders;

	ContactItem *item = new ContactItem(std::move(contact));
	m_Provider.setChangeCallback(item->getContact().getPersonId(),
			std::bind(&ListView::onContactChanged, this, _1, _2, item));

	return item;
}

void ListView::insertContactItem(ContactItem *item)
{
	ContactGroupItem *group = nullptr;
	ContactItem *nextItem = nullptr;
	UniString indexLetter = item->getContact().getIndexLetter();

	auto it = m_Groups.find(indexLetter);
	if (it != m_Groups.end()) {
		group = it->second;
		nextItem = getNextContactItem(it->second, item->getContact());
	} else {
		ContactGroupItem *nextGroup = getNextGroupItem(indexLetter);
		group = insertGroupItem(indexLetter, nextGroup);
	}

	m_Genlist->insert(item, group, nextItem);
}

void ListView::updateContactItem(ContactItem *item, Model::ContactPtr contact)
{
	if (item->getContact() != *contact) {
		ContactGroupItem *oldGroup = static_cast<ContactGroupItem *>(item->getParentItem());

		item->pop();

		item->setContact(std::move(contact));
		insertContactItem(item);

		if (oldGroup->empty()) {
			deleteGroupItem(oldGroup);
		}
	} else {
		item->setContact(std::move(contact));
		if (strcmp(item->getContact().getImagePath(), contact->getImagePath()) != 0) {
			elm_genlist_item_fields_update(item->getObjectItem(),
					PART_CONTACT_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

ContactItem *ListView::getNextContactItem(ContactGroupItem *group, const Contact &contact)
{
	for (auto &&item : *group) {
		ContactItem *contactItem = static_cast<ContactItem *>(item);
		if (contact < contactItem->getContact()) {
			return contactItem;
		}
	}

	return nullptr;
}

void ListView::onItemSelected(Evas_Object *genlist, Elm_Object_Item *genlistItem)
{
	ContactItem *item = (ContactItem *) elm_object_item_data_get(genlistItem);

	int id = 0;
	contacts_record_get_int(item->getContact().getRecord(), _contacts_person.display_contact_id, &id);

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

void ListView::onContactInserted(ContactPtr contact)
{
	insertContactItem(createContactItem(std::move(contact)));
}

void ListView::onContactChanged(ContactPtr contact, contacts_changed_e changeType, ContactItem *item)
{
	if (changeType == CONTACTS_CHANGE_DELETED) {
//		Todo: Implement delete contact functionality
	} else if (changeType == CONTACTS_CHANGE_UPDATED) {
		updateContactItem(item, std::move(contact));
	}
}
