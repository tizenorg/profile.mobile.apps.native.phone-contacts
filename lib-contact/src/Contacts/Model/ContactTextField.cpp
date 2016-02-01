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

#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

void ContactTextField::initialize()
{
	const char *value = getValue();
	if (value) {
		m_InitialValue = value;
	}
}

void ContactTextField::reset()
{
	setValue("");
}

bool ContactTextField::isEmpty() const
{
	const char *value = getValue();
	return !(value && *value);
}

bool ContactTextField::isChanged() const
{
	const char *value = getValue();
	if (!value) {
		value = "";
	}

	return m_InitialValue.compare(value) != 0;
}

const char *ContactTextField::getValue() const
{
	char *value = nullptr;
	contacts_record_get_str_p(getRecord(), getPropertyId(), &value);
	return value;
}

void ContactTextField::setValue(const char *value)
{
	bool wasFilled = isFilled();
	contacts_record_set_str(getRecord(), getPropertyId(), value);

	bool isFilled = value && *value;
	if (wasFilled != isFilled) {
		onFilled(isFilled);
	}
}
