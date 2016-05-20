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

#include "Contacts/Model/ContactData.h"
#include "Contacts/List/ReorderItem.h"

#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"

using namespace Contacts::List;

ReorderItem::ReorderItem(Contacts::Model::ContactData &contactData)
	: ContactItem(contactData)
{}

Evas_Object *ReorderItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_CONTACT_REORDER) == 0) {
		Evas_Object *button = elm_button_add(parent);
		elm_object_style_set(button, "icon_reorder");
		evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_DOWN,
				makeCallback(&ReorderItem::onMouseDown), this);

		return button;
	}

	return ContactItem::getContent(parent, part);
}

void ReorderItem::onMouseDown(Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Object *genlist = elm_object_item_widget_get(getObjectItem());
	elm_genlist_reorder_mode_set(genlist, EINA_TRUE);
}
