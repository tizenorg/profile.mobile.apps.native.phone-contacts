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

bool ContactObject::isEmpty() const
{
	bool isEmpty = true;
	for (auto &&field : *this) {
		if (!field->isEmpty()) {
			isEmpty = false;
			break;
		}
	}

	return isEmpty;
}

ContactObjectIterator ContactObject::begin() const
{
	return ContactObjectIterator(*this, 0);
}

ContactObjectIterator ContactObject::end() const
{
	return ContactObjectIterator(*this, getObjectMetadata()->fields.count());
}

ContactFieldPtr ContactObject::getField(unsigned id) const
{
	for (auto &&field : getObjectMetadata()->fields) {
		if (field.id == id) {
			return ContactFactory::createField(getRecord(), &field);
		}
	}

	return nullptr;
}

ContactFieldPtr ContactObject::getFieldByIndex(unsigned index) const
{
	const ContactObjectMetadata *object = getObjectMetadata();
	const ContactFieldMetadata *field = object->fields.begin() + index;
	if (field < object->fields.end()) {
		return ContactFactory::createField(getRecord(), field);
	}

	return nullptr;
}

const ContactObjectMetadata *ContactObject::getObjectMetadata() const
{
	return (const ContactObjectMetadata *) ContactField::getMetadata()->typeMetadata;
}
