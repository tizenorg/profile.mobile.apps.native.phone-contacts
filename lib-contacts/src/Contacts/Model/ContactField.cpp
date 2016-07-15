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
#include "Contacts/Model/ContactFieldContainer.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

ContactField::ContactField(ContactFieldContainer *parent,
		const ContactFieldMetadata &metadata)
	: m_Record(nullptr), m_Metadata(metadata), m_Parent(parent)
{
}

void ContactField::initialize(contacts_record_h record)
{
	if (m_Record) {
		return;
	}

	onInitialize(record);
	m_Record = record;
}

void ContactField::update(contacts_record_h record)
{
	if (!m_Record) {
		return;
	}

	if (!record) {
		onUpdated(*this, CONTACTS_CHANGE_DELETED);
		return;
	}

	onUpdate(record);

	bool isUpdated = isChanged();
	m_Record = record;

	if (isUpdated) {
		onUpdated(*this, CONTACTS_CHANGE_UPDATED);
	}
}

bool ContactField::isRequired() const
{
	return m_Metadata.isRequired;
}

void ContactField::setFillCallback(FillCallback callback)
{
	m_OnFilled = std::move(callback);
}

void ContactField::setUpdateCallback(UpdateCallback callback)
{
	m_OnUpdated = std::move(callback);
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

ContactFieldContainer *ContactField::getParent() const
{
	return m_Parent;
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

	if (m_Parent) {
		m_Parent->onChildFilled(*this, isFilled);
	}
}

void ContactField::onUpdated(ContactField &field, contacts_changed_e change)
{
	if (m_OnUpdated) {
		m_OnUpdated(field, change);
	}

	if (m_Parent) {
		if (&field != this) {
			change = CONTACTS_CHANGE_UPDATED;
		}
		m_Parent->onChildUpdated(*this, change);
	}
}
