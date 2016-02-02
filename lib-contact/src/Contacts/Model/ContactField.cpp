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

#include "Contacts/Model/ContactField.h"
#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactBoolField.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTypedObject.h"
#include "Contacts/Model/ContactCompoundObject.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

ContactField::ContactField(contacts_record_h record,
		const ContactFieldMetadata &metadata)
	: m_Record(record), m_Metadata(metadata)
{
}

ContactField::ContactField(const ContactFieldMetadata &metadata)
	: m_Record(nullptr), m_Metadata(metadata)
{
}

template <typename FieldType>
FieldType &ContactField::cast()
{
	return static_cast<FieldType &>(*this);
}

template <typename FieldType>
const FieldType &ContactField::cast() const
{
	return static_cast<const FieldType &>(*this);
}

template ContactArray &ContactField::cast();
template ContactBoolField &ContactField::cast();
template ContactDateField &ContactField::cast();
template ContactEnumField &ContactField::cast();
template ContactTextField &ContactField::cast();
template ContactObject &ContactField::cast();
template ContactTypedObject &ContactField::cast();
template ContactCompoundObject &ContactField::cast();

template const ContactArray &ContactField::cast() const;
template const ContactBoolField &ContactField::cast() const;
template const ContactDateField &ContactField::cast() const;
template const ContactEnumField &ContactField::cast() const;
template const ContactTextField &ContactField::cast() const;
template const ContactObject &ContactField::cast() const;
template const ContactTypedObject &ContactField::cast() const;
template const ContactCompoundObject &ContactField::cast() const;

bool ContactField::isRequired() const
{
	return m_Metadata.isRequired;
}

void ContactField::setFillCallback(FillCallback callback)
{
	m_OnFilled = std::move(callback);
}

unsigned ContactField::getId() const
{
	return m_Metadata.id;
}

ContactFieldType ContactField::getType() const
{
	return m_Metadata.typeMetadata->type;
}

unsigned ContactField::getSubType() const
{
	return m_Metadata.typeMetadata->subType;
}

contacts_record_h ContactField::getRecord() const
{
	return m_Record;
}

unsigned ContactField::getPropertyId() const
{
	return m_Metadata.propId;
}

void ContactField::setRecord(contacts_record_h record)
{
	m_Record = record;
}

const ContactFieldMetadata &ContactField::getMetadata() const
{
	return m_Metadata;
}

void ContactField::onFilled(bool isFilled)
{
	if (m_OnFilled) {
		m_OnFilled(isFilled);
	}
}
