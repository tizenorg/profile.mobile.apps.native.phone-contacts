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

#include "Contacts/Model/ContactTypedObject.h"
#include "Contacts/Model/ContactFieldFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

void ContactTypedObject::reset()
{
	ContactObject::reset();
	m_TypeField->reset();
	m_LabelField->reset();
}

bool ContactTypedObject::isChanged() const
{
	if (ContactObject::isChanged()) {
		return true;
	}

	if (!isEmpty()) {
		if (m_TypeField->isChanged()) {
			return true;
		}

		if (getTypeField().hasCustomValue()) {
			return m_LabelField->isChanged();
		}
	}

	return false;
}

ContactEnumField &ContactTypedObject::getTypeField() const
{
	return m_TypeField->cast<ContactEnumField>();
}

ContactTextField &ContactTypedObject::getLabelField() const
{
	return m_LabelField->cast<ContactTextField>();
}

const ContactTypedObjectMetadata &ContactTypedObject::getTypedObjectMetadata() const
{
	return (const ContactTypedObjectMetadata &) getObjectMetadata();
}

void ContactTypedObject::onInitialize(contacts_record_h record)
{
	ContactObject::onInitialize(record);
	if (!m_TypeField) {
		m_TypeField = ContactFieldFactory::createField(this, getTypedObjectMetadata().typeField);
	}

	if (!m_LabelField) {
		m_LabelField = ContactFieldFactory::createField(this, getTypedObjectMetadata().labelField);
	}

	m_TypeField->initialize(record);
	m_LabelField->initialize(record);
}
