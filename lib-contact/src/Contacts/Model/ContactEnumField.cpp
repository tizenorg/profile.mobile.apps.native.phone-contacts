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

#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

namespace
{
	/* FIXME: move type-label interaction to the object level and into separate class. */
	unsigned labelPropId[] = {
		_contacts_image.label,
		0,
		_contacts_number.label,
		_contacts_email.label,
		0,
		_contacts_address.label,
		_contacts_event.label,
		_contacts_relationship.label,
		_contacts_url.label,
		_contacts_messenger.label
	};
}

ContactEnumField::ContactEnumField(contacts_record_h record,
		const ContactFieldMetadata *metadata)
	: ContactField(record, metadata),
	  m_CustomValuePropId(labelPropId[getId()])
{
}

void ContactEnumField::initialize()
{
	setValue(getEnumMetadata()->defaultValue);
}

int ContactEnumField::getValue() const
{
	int value = 0;
	contacts_record_get_int(getRecord(), getPropertyId(), &value);
	return value;
}

void ContactEnumField::setValue(int value)
{
	contacts_record_set_int(getRecord(), getPropertyId(), value);
}

const char *ContactEnumField::getCustomValue() const
{
	char *value = nullptr;
	contacts_record_get_str_p(getRecord(), m_CustomValuePropId, &value);
	return value;
}

void ContactEnumField::setCustomValue(const char *value)
{
	setValue(getEnumMetadata()->customValue);
	contacts_record_set_str(getRecord(), m_CustomValuePropId, value);
}

bool ContactEnumField::hasCustomValue() const
{
	return getValue() == getEnumMetadata()->customValue;
}

const ContactEnumMetadata *ContactEnumField::getEnumMetadata() const
{
	return (const ContactEnumMetadata *) ContactField::getMetadata()->typeMetadata;
}
