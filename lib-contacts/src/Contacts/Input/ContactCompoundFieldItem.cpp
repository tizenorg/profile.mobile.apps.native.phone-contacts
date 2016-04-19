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
#include "Contacts/Input/ContactCompoundFieldControl.h"
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactCompoundObject.h"

#include "App/Path.h"
#include "Utils/Callback.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	const std::string layoutPath = App::getResourcePath(INPUT_ITEM_LAYOUT_EDJ);
}

ContactCompoundFieldItem::ContactCompoundFieldItem(Model::ContactObject &object)
	: ContactFieldItem(object)
{
}

Evas_Object *ContactCompoundFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_RIGHT) == 0) {
		return createExpandButton(parent);
	} else if (strcmp(part, PART_MIDDLE) == 0) {
		auto control = new ContactCompoundFieldControl(this, getObject().cast<ContactCompoundObject>());
		control->create(parent);
		control->setCompoundMode(!isExpanded());
		return control->getEvasObject();
	} else {
		return ContactFieldItem::getContent(parent, part);
	}
}

void ContactCompoundFieldItem::onExpanded(bool isExpanded)
{
	auto control = static_cast<ContactCompoundFieldControl *>(getFieldControl());
	control->setCompoundMode(!isExpanded);

	elm_image_file_set(getExpandIcon(), layoutPath.c_str(),
			isExpanded ? GROUP_ICON_CONTRACT : GROUP_ICON_EXPAND);

	GenlistGroupItem::onExpanded(isExpanded);

	if (elm_object_focus_get(control->getEntry())) {
		control->updateReturnKey();
	} else {
		focus(ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

Evas_Object *ContactCompoundFieldItem::createExpandButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	/* FIXME: Request standard style for expand open/close buttons */
	elm_object_style_set(button, "floatingbutton/default");
	elm_object_focus_allow_set(button, EINA_FALSE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&ContactCompoundFieldItem::onExpandPressed), this);

	Evas_Object *icon = elm_image_add(button);
	elm_object_part_content_set(button, "elm.swallow.content", icon);
	elm_image_file_set(icon, layoutPath.c_str(),
			isExpanded() ? GROUP_ICON_CONTRACT : GROUP_ICON_EXPAND);

	return button;
}

Evas_Object *ContactCompoundFieldItem::getExpandIcon() const
{
	Evas_Object *button = elm_object_item_part_content_get(getObjectItem(), PART_RIGHT);
	return elm_object_part_content_get(button, "elm.swallow.content");
}

void ContactCompoundFieldItem::onExpandPressed(Evas_Object *button, void *eventInfo)
{
	elm_genlist_item_expanded_set(getObjectItem(), !isExpanded());
}
