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

#include "Contacts/Input/ContactFieldItem.h"
#include "Contacts/Input/ContactFieldSubItem.h"
#include "Contacts/Model/ContactObject.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactFieldItem::ContactFieldItem(ContactObject &object)
	: ContactFieldSubItem(*object.getField(0)),
	  m_Object(object)
{
	unsigned firstFieldId = getField().getId();
	for (auto &&field : getObject()) {
		if (field.getId() != firstFieldId) {
			insertSubItem(new ContactFieldSubItem(field));
		}
	}
}

void ContactFieldItem::setRemoveCallback(RemoveCallback callback)
{
	m_OnRemove = std::move(callback);
}

ContactObject &ContactFieldItem::getObject() const
{
	return m_Object;
}

void ContactFieldItem::update()
{
	ContactFieldSubItem::update();
	for (auto &&item : *this) {
		auto subItem = dynamic_cast<ContactFieldSubItem *>(item);
		if (subItem) {
			subItem->update();
		}
	}
}

Elm_Genlist_Item_Class *ContactFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(INPUT_ITEM_STYLE);
	return &itc;
}

Evas_Object *ContactFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_RIGHT) == 0) {
		return createRemoveButton(parent);
	} else {
		return ContactFieldSubItem::getContent(parent, part);
	}
}

void ContactFieldItem::onInserted()
{
	elm_genlist_item_expanded_set(getObjectItem(), EINA_TRUE);
}

Evas_Object *ContactFieldItem::createRemoveButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "icon_expand_delete");
	elm_object_focus_allow_set(button, EINA_FALSE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&ContactFieldItem::onRemovePressed), this);

	return button;
}

void ContactFieldItem::onRemovePressed(Evas_Object *button, void *eventInfo)
{
	if (m_OnRemove) {
		m_OnRemove(this);
	}
}
