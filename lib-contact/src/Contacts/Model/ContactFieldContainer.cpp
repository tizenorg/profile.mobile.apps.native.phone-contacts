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

void ContactFieldContainer::reset()
{
	for (auto &&field : m_Fields) {
		field->reset();
	}
}

bool ContactFieldContainer::isEmpty() const
{
	return !std::any_of(m_Fields.begin(), m_Fields.end(), [](const ContactFieldPtr &field) {
		return !field->isEmpty();
	});
}

bool ContactFieldContainer::isFilled() const
{
	return std::any_of(m_Fields.begin(), m_Fields.end(), [](const ContactFieldPtr &field) {
		return field->isRequired() && field->isFilled();
	});
}

bool ContactFieldContainer::isChanged() const
{
	return std::any_of(m_Fields.begin(), m_Fields.end(), std::mem_fn(&ContactField::isChanged));
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

size_t ContactFieldContainer::getFieldCount() const
{
	return m_Fields.size();
}

ContactField &ContactFieldContainer::addField(contacts_record_h record,
		const ContactFieldMetadata &metadata)
{
	ContactFieldPtr field = ContactFactory::createField(record, metadata);
	if (field->isRequired()) {
		if (field->isFilled()) {
			onChildFillChanged(true);
		}

		field->setFillChangedCallback(std::bind(&ContactFieldContainer::onChildFillChanged,
				this, std::placeholders::_1));
	}

	m_Fields.push_back(std::move(field));
	return *m_Fields.back();
}

void ContactFieldContainer::removeField(ContactField &field)
{
	auto comp = [&field](ContactFieldPtr &ptr) {
		return ptr.get() == &field;
	};

	if (field.isRequired() && field.isFilled()) {
		onChildFillChanged(false);
	}

	m_Fields.erase(std::remove_if(m_Fields.begin(), m_Fields.end(), comp), m_Fields.end());
}

void ContactFieldContainer::onChildFillChanged(bool isChildFilled)
{
	/* Equals zero if no fields were PREVIOUSLY filled or no fields are NOW filled */
	size_t checkCount = isChildFilled ? m_FilledCount++ : --m_FilledCount;

	if (checkCount == 0) {
		onFillChanged(isChildFilled);
	}
}
