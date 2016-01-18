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

#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

void ContactObject::initialize()
{
	for (auto &&field : getObjectMetadata().fields) {
		m_Fields.push_back(ContactFactory::createField(getRecord(), field));
	}
}

bool ContactObject::isEmpty() const
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

void ContactObject::reset()
{
	for (auto &&field : *this) {
		field.reset();
	}
}

ContactObjectIterator ContactObject::begin() const
{
	return ContactObjectIterator(*this, 0);
}

ContactObjectIterator ContactObject::end() const
{
	return ContactObjectIterator(*this, m_Fields.size());
}

ContactField *ContactObject::getField(unsigned index) const
{
	if (index < m_Fields.size()) {
		return m_Fields[index].get();
	}

	return nullptr;
}

ContactField *ContactObject::getFieldById(unsigned id) const
{
	for (auto &&field : m_Fields) {
		if (field->getId() == id) {
			return field.get();
		}
	}

	return nullptr;
}

const ContactObjectMetadata &ContactObject::getObjectMetadata() const
{
	return *(const ContactObjectMetadata *) ContactField::getMetadata().typeMetadata;
}
