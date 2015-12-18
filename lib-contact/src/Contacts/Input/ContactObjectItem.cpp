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

#include "Contacts/Input/ContactObjectItem.h"
#include "Contacts/Input/ContactFieldItem.h"
#include "Utils/Callback.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

namespace
{
	Elm_Genlist_Item_Class itc = Ui::GenlistItem::createItemClass(INPUT_ITEM_STYLE);
}

ContactObjectItem::ContactObjectItem(ContactFieldPtr object)
	: GenlistGroupItem(&itc, ELM_GENLIST_ITEM_TREE),
	  m_Object(std::move(object)), m_FirstFieldItem(nullptr)
{
	for (auto &&field : getObject()) {
		ContactFieldItem *item = new ContactFieldItem(std::move(field));
		if (!m_FirstFieldItem) {
			m_FirstFieldItem = item;
		} else {
			insertSubItem(item);
		}
	}
}

ContactObjectItem::~ContactObjectItem()
{
	delete m_FirstFieldItem;
}

void ContactObjectItem::setRemoveCallback(RemoveCallback callback)
{
	m_OnRemove = std::move(callback);
}

const ContactObject &ContactObjectItem::getObject() const
{
	return m_Object->cast<ContactObject>();
}

Evas_Object *ContactObjectItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_RIGHT) == 0) {
		Evas_Object *button = elm_button_add(parent);
		elm_object_style_set(button, "icon_expand_delete");
		evas_object_smart_callback_add(button, "clicked",
				makeCallback(&ContactObjectItem::onRemovePressed), this);

		return button;
	} else {
		return m_FirstFieldItem->getContent(parent, part);
	}
}

void ContactObjectItem::onRemovePressed(Evas_Object *button, void *eventInfo)
{
	if (m_OnRemove) {
		m_OnRemove(this, std::move(m_Object));
	}
	delete this;
}
