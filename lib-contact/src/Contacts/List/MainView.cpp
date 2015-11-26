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

#include "Contacts/List/MainView.h"
#include "Contacts/List/ContactItem.h"
#include "Contacts/List/ContactGroupItem.h"
#include "Contacts/Utils.h"
#include "Ui/Genlist.h"
#include "Ui/GenlistItem.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Contacts/Settings/MainView.h"
#include <contacts.h>

using namespace Contacts;
using namespace Contacts::List;

MainView::MainView()
	: m_Genlist(nullptr), m_Index(nullptr)
{
}

MainView::~MainView()
{
	contacts_db_remove_changed_cb(_contacts_person._uri,
			makeCallbackWithLastParam(&MainView::onDatabaseChanged), this);
}

Evas_Object *MainView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "fastscroll");

	m_Genlist = new Ui::Genlist();
	elm_object_part_content_set(layout, "elm.swallow.content", m_Genlist->create(layout));
	elm_object_part_content_set(layout, "elm.swallow.fastscroll", createIndex(layout));

	return layout;
}

Evas_Object *MainView::createIndex(Evas_Object *parent)
{
	m_Index = elm_index_add(parent);
	elm_index_autohide_disabled_set(m_Index, EINA_TRUE);
	elm_index_omit_enabled_set(m_Index, EINA_TRUE);

	evas_object_smart_callback_add(m_Index, "changed",
			(Evas_Smart_Cb) makeCallback(&MainView::onIndexChanged), this);
	evas_object_smart_callback_add(m_Index, "selected",
			(Evas_Smart_Cb) makeCallback(&MainView::onIndexSelected), this);

	return m_Index;
}

void MainView::onPageAttached()
{
	Evas_Object *button = elm_button_add(getEvasObject());
	elm_object_style_set(button, "bottom");
	elm_object_translatable_text_set(button, "IDS_PB_OPT_CREATE");
	evas_object_smart_callback_add(button, "clicked",
			(Evas_Smart_Cb) makeCallback(&MainView::onCreatePressed), this);

	getPage()->setTitle("IDS_PB_TAB_CONTACTS");
	getPage()->setContent("toolbar", button);
}

void MainView::onCreated()
{
	contacts_db_add_changed_cb(_contacts_person._uri,
			makeCallbackWithLastParam(&MainView::onDatabaseChanged), this);

	fillList();
}

void MainView::fillList()
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_person._uri, 0, 0, &list);

	char *currentLetter = nullptr;
	char *nextLetter = nullptr;
	contacts_record_h record = nullptr;
	Ui::GenlistItem *groupItem = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		contacts_record_get_str_p(record, _contacts_person.display_name_index, &nextLetter);
		if (!currentLetter || strcmp(currentLetter, nextLetter) != 0) {
			groupItem = appendGroupItem(nextLetter);
			currentLetter = nextLetter;
		}

		m_Genlist->insert(new ContactItem(record), groupItem);
	}

	contacts_list_destroy(list, false);
}

Ui::GenlistItem *MainView::appendGroupItem(const char *indexLetter)
{
	ContactGroupItem *item = new ContactGroupItem(indexLetter);
	elm_index_item_append(m_Index, indexLetter, nullptr, m_Genlist->insert(item));
	return item;
}

void MainView::onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem)
{
	Elm_Object_Item *genlistItem = (Elm_Object_Item *) elm_object_item_data_get(indexItem);
	elm_genlist_item_show(genlistItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void MainView::onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem)
{
	onIndexChanged(index, indexItem);
	elm_index_item_selected_set(indexItem, EINA_FALSE);
}

void MainView::onCreatePressed()
{
	//TODO: getNavigator()->navigateTo(new Contacts::Input::MainView());
}

void MainView::onDatabaseChanged(const char *uri)
{
	elm_index_item_clear(m_Index);
	elm_genlist_clear(m_Genlist->getEvasObject());
	fillList();
}

Evas_Object *MainView::onMenuPressed()
{
	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_SETTINGS", [this] {
		getNavigator()->navigateTo(new Settings::MainView());
	});
	return menu->getEvasObject();
}
