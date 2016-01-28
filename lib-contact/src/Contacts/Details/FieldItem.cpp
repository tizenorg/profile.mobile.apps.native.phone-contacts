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

#include "Contacts/Details/FieldItem.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Common/Strings.h"

#include <app_i18n.h>

using namespace Contacts::Details;
using namespace Contacts::Model;

#define DATE_BUFFER_SIZE 32

FieldItem::FieldItem(ContactObject &object)
	: m_Object(object), m_Field(*object.getField(0))
{
}

ContactObject &FieldItem::getObject() const
{
	return m_Object;
}

ContactField &FieldItem::getField() const
{
	return m_Field;
}

Elm_Genlist_Item_Class *FieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
	return &itc;
}

char *FieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		switch (m_Field.getType()) {
			case TypeText:
			{
				const char *value = m_Field.cast<ContactTextField>().getValue();
				return value ? strdup(value) : nullptr;
			}
			case TypeDate:
			{
				tm date = m_Field.cast<ContactDateField>().getValue();
				char buffer[DATE_BUFFER_SIZE];
				strftime(buffer, sizeof(buffer), "%x", &date);
				return strdup(buffer);
			}
			default:
				return nullptr;
		}
	} else if (strcmp(part, "elm.text.sub") == 0) {
		const char *name = Common::getContactFieldName(ContactFieldId(m_Object.getId()));
		return name ? strdup(_(name)) : nullptr;
	}

	return nullptr;
}
