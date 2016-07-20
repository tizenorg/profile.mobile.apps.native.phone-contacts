/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactFieldMetadata.h"
#include "Common/Database/RecordUtils.h"

using namespace Common::Database;
using namespace Contacts::Model;

void ContactEnumField::reset()
{
	setValue(getEnumMetadata().defaultValue);
}

bool ContactEnumField::isChanged() const
{
	return m_InitialValue != getValue();
}

Utils::Range<const int *> ContactEnumField::getValues() const
{
	return getEnumMetadata().values;
}

int ContactEnumField::getCustomValue() const
{
	return getEnumMetadata().customValue;
}

int ContactEnumField::getValue() const
{
	return getRecordInt(getRecord(), getPropertyId());
}

void ContactEnumField::setValue(int value)
{
	contacts_record_set_int(getRecord(), getPropertyId(), value);
}

bool ContactEnumField::hasCustomValue() const
{
	return getValue() == getEnumMetadata().customValue;
}

const ContactEnumMetadata &ContactEnumField::getEnumMetadata() const
{
	return *(const ContactEnumMetadata *) ContactField::getMetadata().typeMetadata;
}

void ContactEnumField::onInitialize(contacts_record_h record)
{
	m_InitialValue = getRecordInt(record, getPropertyId());
}
