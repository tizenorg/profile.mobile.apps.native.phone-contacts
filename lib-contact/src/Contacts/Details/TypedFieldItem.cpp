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
#include "Contacts/Common/Strings.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

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
		if (m_TypeField.hasCustomValue()) {
			return strdup(m_LabelField.getValue());
		}

		int currentValue = m_TypeField.getValue();

		auto names = Common::getContactEnumValueNames(ContactEnumType(m_TypeField.getSubType()));
		auto values = m_TypeField.getValues();
		RETVM_IF(names.count() != values.count(), nullptr, "names.count() != values.count()");

		for (size_t i = 0; i < values.count(); ++i) {
			if (values[i] == currentValue) {
				return strdup(_(names[i]));
			}
		}
	}

	return FieldItem::getText(parent, part);
}
