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

#include "Contacts/Model/ContactBoolField.h"
#include "Contacts/Utils.h"

using namespace Contacts::Model;

bool ContactBoolField::isChanged() const
{
	return m_InitialValue != getValue();
}

bool ContactBoolField::getValue() const
{
	return getRecordBool(getRecord(), getPropertyId());
}

void ContactBoolField::setValue(bool value)
{
	contacts_record_set_bool(getRecord(), getPropertyId(), value);
}

void ContactBoolField::onInitialize(contacts_record_h record)
{
	m_InitialValue = getRecordBool(record, getPropertyId());
}
