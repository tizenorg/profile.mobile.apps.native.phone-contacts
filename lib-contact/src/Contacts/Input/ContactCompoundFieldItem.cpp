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
	: ContactFieldItem(object), m_ExpandButton(nullptr)
{
}

Evas_Object *ContactCompoundFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_RIGHT) == 0) {
		return createExpandButton(parent);
	} else if (strcmp(part, PART_MIDDLE) == 0) {
		auto control = new ContactCompoundFieldControl(this, getObject().cast<ContactCompoundObject>());
		return control->create(parent);
	} else {
		return ContactFieldItem::getContent(parent, part);
	}
}

void ContactCompoundFieldItem::onExpanded()
{
	ContactFieldItem::onExpanded();
	static_cast<ContactCompoundFieldControl *>(getFieldControl())->setCompoundMode(false);
	updateExpandButton();
}

void ContactCompoundFieldItem::onContracted()
{
	ContactFieldItem::onContracted();
	static_cast<ContactCompoundFieldControl *>(getFieldControl())->setCompoundMode(true);
	updateExpandButton();
}

Evas_Object *ContactCompoundFieldItem::createExpandButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	/* FIXME: Request standard style for expand open/close buttons */
	elm_object_style_set(button, "floatingbutton/default");
	elm_object_focus_allow_set(button, EINA_FALSE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&ContactCompoundFieldItem::onExpandPressed), this);

	m_ExpandButton = elm_image_add(button);
	elm_object_part_content_set(button, "elm.swallow.content", m_ExpandButton);
	updateExpandButton();

	return button;
}

void ContactCompoundFieldItem::updateExpandButton()
{
	elm_image_file_set(m_ExpandButton, layoutPath.c_str(), isExpanded() ? GROUP_ICON_CONTRACT : GROUP_ICON_EXPAND);
}

void ContactCompoundFieldItem::onExpandPressed(Evas_Object *button, void *eventInfo)
{
	elm_genlist_item_expanded_set(getObjectItem(), !isExpanded());
}
