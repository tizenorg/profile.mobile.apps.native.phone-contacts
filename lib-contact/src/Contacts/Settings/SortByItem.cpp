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

#include <app_i18n.h>
#include <contacts_setting.h>
#include "Contacts/Settings/SortByItem.h"
#include "Contacts/Settings/SortByPopup.h"
#include "Utils/Logger.h"

using namespace Ui;
using namespace Contacts::Settings;

char *SortByItem::getText(Evas_Object *parent, const char *part) {
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_MBODY_SORT_BY"));
	} else if (strcmp(part, "elm.text.sub") == 0) {
		int type = 0;
		contacts_setting_get_name_sorting_order((contacts_name_sorting_order_e *) &type);
		if (CONTACTS_NAME_SORTING_ORDER_FIRSTLAST == type) {
			return strdup(_("IDS_PB_OPT_FIRST_NAME"));
		} else if (CONTACTS_NAME_SORTING_ORDER_LASTFIRST == type) {
			return strdup(_("IDS_PB_BODY_LAST_NAME_ABB"));
		}
	}
	return nullptr;
}

void SortByItem::onSortingOrderChanged(
		contacts_name_sorting_order_e name_sorting_order, void *user_data) {
	elm_genlist_item_update((Elm_Genlist_Item *) user_data);
}

void SortByItem::onSelected() {
	Ui::Popup *popup = new SortByPopup();
	popup->create(elm_object_item_widget_get(getObjectItem()));
	contacts_setting_add_name_sorting_order_changed_cb(onSortingOrderChanged, getObjectItem());
}
