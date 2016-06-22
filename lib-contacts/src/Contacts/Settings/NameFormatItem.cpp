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

#include "Ui/Genlist.h"
#include "Ui/RadioPopup.h"
#include "Utils/Callback.h"

#include <app_i18n.h>

#define PART_SETTING_NAME "elm.text"
#define PART_SETTING_VALUE "elm.text.sub"

using namespace Contacts::Settings;

NameFormatItem::NameFormatItem()
{
	contacts_setting_get_name_display_order(&m_Order);
	contacts_setting_add_name_display_order_changed_cb(
			makeCallbackWithLastParam(&NameFormatItem::onNameOrderChanged), this);
}

NameFormatItem::~NameFormatItem()
{
	contacts_setting_remove_name_display_order_changed_cb(
			makeCallbackWithLastParam(&NameFormatItem::onNameOrderChanged), this);
}

char *NameFormatItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_SETTING_NAME) == 0) {
		return strdup(_("IDS_PB_TMBODY_NAME_FORMAT"));
	} else if (strcmp(part, PART_SETTING_VALUE) == 0) {
		switch (m_Order) {
			case CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST:
				return strdup(_("IDS_PB_OPT_FIRST_NAME_FIRST_ABB"));
			case CONTACTS_NAME_DISPLAY_ORDER_LASTFIRST:
				return strdup(_("IDS_PB_OPT_LAST_NAME_FIRST_ABB"));
		}
	}

	return nullptr;
}

void NameFormatItem::onSelected()
{
	Ui::RadioPopup *popup = new Ui::RadioPopup();
	popup->create(getParent()->getEvasObject());
	popup->setTitle("IDS_PB_TMBODY_NAME_FORMAT");
	popup->addItem("IDS_PB_OPT_FIRST_NAME_FIRST_ABB", (void *) CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST);
	popup->addItem("IDS_PB_OPT_LAST_NAME_FIRST_ABB", (void *) CONTACTS_NAME_DISPLAY_ORDER_LASTFIRST);
	popup->setSelectedItem(m_Order);
	popup->setSelectedCallback([](void *data) {
		contacts_setting_set_name_display_order((contacts_name_display_order_e) (long) data);
	});

	elm_popup_orient_set(popup->getEvasObject(), ELM_POPUP_ORIENT_CENTER);
}

void NameFormatItem::onNameOrderChanged(contacts_name_display_order_e order)
{
	m_Order = order;
	elm_genlist_item_fields_update(getObjectItem(), PART_SETTING_VALUE, ELM_GENLIST_ITEM_FIELD_TEXT);
}
