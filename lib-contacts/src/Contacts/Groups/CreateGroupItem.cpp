/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Groups/CreateGroupItem.h"

#include "Ui/Genlist.h"
#include "Ui/Scale.h"
#include "Ux/EditfieldPopup.h"
#include "Utils/Callback.h"

#include <contacts.h>

#include "AppsCommonButtons.h"

#define ITEM_H 144
#define BUTTON_W 496
#define BUTTON_H 70
#define BUTTON_COLOR_NORMAL 78, 97, 173, 255
#define BUTTON_COLOR_PRESSED 61, 76, 135, 255

using namespace Contacts::Groups;

void CreateGroupItem::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

Elm_Genlist_Item_Class *CreateGroupItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("full");
	return &itc;
}

Evas_Object *CreateGroupItem::getContent(Evas_Object *parent, const char *part)
{
	Evas_Object *box = elm_box_add(parent);
	evas_object_size_hint_min_set(box, 0, Ui::getScaledValue(ITEM_H));

	Evas_Object *button = elm_button_add(box);
	elm_object_style_set(button, BUTTON_STYLE_CUSTOM_DEFAULT);
	elm_object_translatable_text_set(button, "IDS_PB_BUTTON_CREATE_GROUP_ABB");
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&CreateGroupItem::onButtonPressed), this);

	Evas_Object *edje = elm_layout_edje_get(button);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_NORMAL, BUTTON_COLOR_NORMAL,
			0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_PRESSED, BUTTON_COLOR_PRESSED,
			0, 0, 0, 0, 0, 0, 0, 0);

	evas_object_size_hint_min_set(button, Ui::getScaledValue(BUTTON_W), Ui::getScaledValue(BUTTON_H));
	elm_box_pack_end(box, button);
	evas_object_show(button);

	return box;
}

void CreateGroupItem::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	auto popup = new Ux::EditfieldPopup();
	popup->setStrings({
		"IDS_PB_HEADER_CREATE_GROUP",
		"IDS_PB_MBODY_GROUP_NAME_ABB",
		"IDS_PB_BUTTON_CREATE_ABB2",
		"IDS_PB_BUTTON_CANCEL"
	});
	popup->setResultCallback([this](const char *name) {
		int id = 0;
		contacts_record_h record = nullptr;
		contacts_record_create(_contacts_group._uri, &record);
		contacts_record_set_str(record, _contacts_group.name, name);
		contacts_db_insert_record(record, &id);
		contacts_record_destroy(record, true);

		if (m_OnResult) {
			m_OnResult(id);
		}
	});
	popup->create(getParent()->getEvasObject());
}
