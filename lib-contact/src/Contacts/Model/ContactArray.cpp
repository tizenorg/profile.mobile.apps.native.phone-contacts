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
#include "Contacts/Model/ContactFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

#include <algorithm>

using namespace Contacts::Model;

void ContactArray::initialize()
{
	int count = 0;
	contacts_record_get_child_record_count(getRecord(), getPropertyId(), &count);

	for (int i = 0; i < count; ++i) {
		contacts_record_h record = nullptr;
		contacts_record_get_child_record_at_p(getRecord(), getPropertyId(), i, &record);
		m_Fields.push_back(ContactFactory::createField(record, getArrayMetadata().element));
	}
}

bool ContactArray::isEmpty() const
{
	bool isEmpty = true;
	for (auto &&field : *this) {
		if (!field.isEmpty()) {
			isEmpty = false;
			break;
		}
	}

	return isEmpty;
}

ContactArrayIterator ContactArray::begin() const
{
	return ContactArrayIterator(*this, 0);
}

ContactArrayIterator ContactArray::end() const
{
	return ContactArrayIterator(*this, m_Fields.size());
}

ContactField *ContactArray::getField(unsigned index) const
{
	if (index < m_Fields.size()) {
		return m_Fields[index].get();
	}

	return nullptr;
}

ContactField &ContactArray::addField()
{
	const ContactFieldMetadata *elementMetadata = &getArrayMetadata().element;
	const char *uri = ((const ContactObjectMetadata *) elementMetadata->typeMetadata)->uri;

	contacts_record_h record = nullptr;
	contacts_record_create(uri, &record);
	contacts_record_add_child_record(getRecord(), getPropertyId(), record);

	ContactFieldPtr field = ContactFactory::createField(record, getArrayMetadata().element);
	field->reset();

	m_Fields.push_back(std::move(field));
	return *m_Fields.back().get();
}

void ContactArray::removeField(ContactField &field)
{
	auto comp = [&field](ContactFieldPtr &ptr) {
		return ptr.get() == &field;
	};

	m_Fields.erase(std::remove_if(m_Fields.begin(), m_Fields.end(), comp), m_Fields.end());

	contacts_record_remove_child_record(getRecord(), getPropertyId(), field.getRecord());
	contacts_record_destroy(field.getRecord(), true);
}

const ContactArrayMetadata &ContactArray::getArrayMetadata() const
{
	return *(const ContactArrayMetadata *) ContactField::getMetadata().typeMetadata;
}
