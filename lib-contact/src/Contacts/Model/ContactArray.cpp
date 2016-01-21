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

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

void ContactArray::initialize()
{
	int count = 0;
	contacts_record_get_child_record_count(getRecord(), getPropertyId(), &count);

	for (int i = 0; i < count; ++i) {
		contacts_record_h record = nullptr;
		contacts_record_get_child_record_at_p(getRecord(), getPropertyId(), i, &record);
		ContactFieldContainer::addField(record, getArrayMetadata().element);
	}

	m_InitialCount = getFieldCount();
}

bool ContactArray::isChanged() const
{
	if (getFieldCount() < m_InitialCount) {
		return true;
	}

	return ContactFieldContainer::isChanged();
}

ContactField &ContactArray::addField()
{
	const ContactFieldMetadata *elementMetadata = &getArrayMetadata().element;
	const char *uri = ((const ContactObjectMetadata *) elementMetadata->typeMetadata)->uri;

	contacts_record_h record = nullptr;
	contacts_record_create(uri, &record);
	contacts_record_add_child_record(getRecord(), getPropertyId(), record);

	ContactField &field = ContactFieldContainer::addField(record, getArrayMetadata().element);
	field.reset();
	return field;
}

void ContactArray::removeField(ContactField &field)
{
	contacts_record_h record = field.getRecord();
	ContactFieldContainer::removeField(field);

	contacts_record_remove_child_record(getRecord(), getPropertyId(), record);
	contacts_record_destroy(record, true);
}

const ContactArrayMetadata &ContactArray::getArrayMetadata() const
{
	return *(const ContactArrayMetadata *) ContactField::getMetadata().typeMetadata;
}
