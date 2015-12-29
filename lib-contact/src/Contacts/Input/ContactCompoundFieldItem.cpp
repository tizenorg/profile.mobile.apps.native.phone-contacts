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

#include "Contacts/Input/ContactCompoundFieldItem.h"
#include "Contacts/Input/ContactCompoundObjectControl.h"
#include "Contacts/Model/ContactObject.h"

#include "App/Path.h"
#include "Utils/Callback.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	const std::string layoutPath = App::getResourcePath(INPUT_ITEM_LAYOUT_EDJ);
}

Evas_Object *ContactCompoundFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_RIGHT) == 0) {
		Evas_Object *button = elm_button_add(parent);
		/* FIXME: Request standard style for expand open/close buttons */
		elm_object_style_set(button, "floatingbutton/default");
		evas_object_smart_callback_add(button, "clicked",
				makeCallback(&ContactCompoundFieldItem::onButtonPressed), this);

		Evas_Object *image = elm_image_add(button);
		elm_image_file_set(image, layoutPath.c_str(), isExpanded() ? GROUP_ICON_CONTRACT : GROUP_ICON_EXPAND);
		elm_object_part_content_set(button, "elm.swallow.content", image);

		return button;
	} else if (!isExpanded() && strcmp(part, PART_MIDDLE) == 0) {
		Ui::Control *control = new ContactCompoundObjectControl(&getObject().cast<ContactCompoundObject>());
		return control->create(parent);
	} else {
		return ContactFieldItem::getContent(parent, part);
	}
}

void ContactCompoundFieldItem::onExpanded()
{
	ContactFieldItem::onExpanded();
	elm_genlist_item_update(getObjectItem());
}

void ContactCompoundFieldItem::onContracted()
{
	ContactFieldItem::onContracted();
	elm_genlist_item_update(getObjectItem());
}

void ContactCompoundFieldItem::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	elm_genlist_item_expanded_set(getObjectItem(), !isExpanded());
}
