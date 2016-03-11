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

#include "Phone/SpeedDial/SettingsView.h"
#include "Phone/SpeedDial/SettingsItem.h"

#include "App/Path.h"
#include "Contacts/Utils.h"
#include "Ui/Scale.h"
#include "Utils/Callback.h"

#include "SpeedDialLayout.h"
#include "SpeedDialLayoutMetrics.h"

#define ITEM_ROWS 3
#define ITEM_COLS SPEED_DIAL_ITEM_COUNT / ITEM_ROWS

namespace
{
	const std::string layoutPath = App::getResourcePath(SPEED_DIAL_LAYOUT_EDJ);
}

using namespace Phone::SpeedDial;

SettingsView::SettingsView()
	: m_Items{nullptr}
{
	contacts_db_add_changed_cb(_contacts_speeddial._uri,
			&SettingsView::onDbChanged, this);
	contacts_db_add_changed_cb(_contacts_contact._uri,
			&SettingsView::onDbChanged, this);
}

SettingsView::~SettingsView()
{
	contacts_db_remove_changed_cb(_contacts_speeddial._uri,
			&SettingsView::onDbChanged, this);
	contacts_db_remove_changed_cb(_contacts_contact._uri,
			&SettingsView::onDbChanged, this);
}

Evas_Object *SettingsView::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_MAIN_LAYOUT);

	Evas_Object *table = elm_table_add(layout);
	elm_object_part_content_set(layout, PART_CONTENT, table);
	elm_table_homogeneous_set(table, EINA_TRUE);
	elm_table_padding_set(table, Ui::getScaledValue(ITEM_PAD_X), Ui::getScaledValue(ITEM_PAD_Y));

	for (int i = 0, k = 0; i < ITEM_ROWS; ++i) {
		for (int j = 0; j < ITEM_COLS; ++j, ++k) {
			m_Items[k] = new SettingsItem(k + 1);
			Evas_Object *item = m_Items[k]->create(table);

			evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(item, EVAS_HINT_FILL, EVAS_HINT_FILL);
			elm_table_pack(table, item, j, i, 1, 1);
			evas_object_show(item);
		}
	}

	fetchItemsData();

	return layout;
}

void SettingsView::onPageAttached(Ui::NavigatorPage *page)
{
	page->setTitle("IDS_KPD_OPT_SPEED_DIAL_SETTINGS_ABB2");
}

void SettingsView::fetchItemsData()
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_speeddial._uri, 0, 0, &list);

	contacts_record_h records[SPEED_DIAL_ITEM_COUNT] = { nullptr };
	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		int number = 0;
		contacts_record_get_int(record, _contacts_speeddial.speeddial_number, &number);
		if (number > 0 && number <= SPEED_DIAL_ITEM_COUNT) {
			records[number - 1] = record;
		}
	}

	for (int i = 0; i < SPEED_DIAL_ITEM_COUNT; ++i) {
		m_Items[i]->setData(records[i]);
	}

	contacts_list_destroy(list, true);
}

void SettingsView::onDbChanged(const char *uri, void *data)
{
	SettingsView *view = (SettingsView*) data;
	view->fetchItemsData();
}
