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

#include "Contacts/Model/ContactFieldContainer.h"
#include "Contacts/Model/ContactFactory.h"

#include <algorithm>

using namespace Contacts::Model;

bool ContactFieldContainer::isEmpty() const
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

void ContactFieldContainer::reset()
{
	for (auto &&field : *this) {
		field.reset();
	}
}

ContactFieldIterator ContactFieldContainer::begin() const
{
	return ContactFieldIterator(*this, 0);
}

ContactFieldIterator ContactFieldContainer::end() const
{
	return ContactFieldIterator(*this, m_Fields.size());
}

ContactField *ContactFieldContainer::getField(unsigned index) const
{
	if (index < m_Fields.size()) {
		return m_Fields[index].get();
	}

	return nullptr;
}

ContactField &ContactFieldContainer::addField(contacts_record_h record,
		const ContactFieldMetadata &metadata)
{
	ContactFieldPtr field = ContactFactory::createField(record, metadata);
	m_Fields.push_back(std::move(field));
	return *m_Fields.back();
}

void ContactFieldContainer::removeField(ContactField &field)
{
	auto comp = [&field](ContactFieldPtr &ptr) {
		return ptr.get() == &field;
	};

	m_Fields.erase(std::remove_if(m_Fields.begin(), m_Fields.end(), comp), m_Fields.end());
}
