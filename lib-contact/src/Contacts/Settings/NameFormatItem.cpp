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

#include "Contacts/Settings/NameFormatItem.h"
#include "Contacts/Settings/NameFormatPopup.h"
#include <app_i18n.h>

using namespace Contacts::Settings;

NameFormatItem::NameFormatItem()
{
	contacts_setting_add_name_display_order_changed_cb(onNameOrderChanged, this);
}

NameFormatItem::~NameFormatItem()
{
	contacts_setting_remove_name_display_order_changed_cb(onNameOrderChanged, this);
}

char *NameFormatItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_TMBODY_NAME_FORMAT"));
	} else if (strcmp(part, "elm.text.sub") == 0) {
		contacts_name_display_order_e type = CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST;
		contacts_setting_get_name_display_order(&type);

		if (CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST == type) {
			return strdup(_("IDS_PB_OPT_FIRST_NAME_FIRST_ABB"));
		} else if (CONTACTS_NAME_DISPLAY_ORDER_LASTFIRST == type) {
			return strdup(_("IDS_PB_OPT_LAST_NAME_FIRST_ABB"));
		}
	}

	return nullptr;
}

void NameFormatItem::onNameOrderChanged(contacts_name_display_order_e name_sorting_order, void *user_data)
{
	NameFormatItem *item = (NameFormatItem *)user_data;
	elm_genlist_item_update(item->getObjectItem());
}

void NameFormatItem::onSelected()
{
	Ui::Popup *popup = new NameFormatPopup();
	popup->create(elm_object_item_widget_get(getObjectItem()));
}
