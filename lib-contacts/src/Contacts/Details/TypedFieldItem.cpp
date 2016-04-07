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

#include "Contacts/Details/TypedFieldItem.h"
#include "Contacts/Model/ContactTypedObject.h"
#include "Common/Strings.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

using namespace Common;
using namespace Contacts::Details;
using namespace Contacts::Model;

TypedFieldItem::TypedFieldItem(ContactObject &object)
	: FieldItem(object),
	  m_TypeField(getTypedObject().getTypeField()),
	  m_LabelField(getTypedObject().getLabelField())
{
}

ContactTypedObject &TypedFieldItem::getTypedObject() const
{
	return getObject().cast<ContactTypedObject>();
}

char *TypedFieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text.sub") == 0) {
		const char *name = nullptr;
		if (m_TypeField.hasCustomValue()) {
			name = m_LabelField.getValue();
		} else {
			EnumType type = EnumType(m_TypeField.getSubType());
			name = _(getEnumValueName(type, m_TypeField.getValue()));
		}

		return Utils::safeDup(name);
	}

	return FieldItem::getText(parent, part);
}

void TypedFieldItem::onFieldUpdated(ContactField &field, contacts_changed_e change)
{
	if (&field == &m_TypeField || &field == &m_LabelField) {
		elm_genlist_item_fields_update(getObjectItem(), "elm.text.sub", ELM_GENLIST_ITEM_FIELD_TEXT);
	} else {
		FieldItem::onFieldUpdated(field, change);
	}
}
