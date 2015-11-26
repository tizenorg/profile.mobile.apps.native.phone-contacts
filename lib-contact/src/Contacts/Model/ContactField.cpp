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
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

#define COUNT(array) sizeof(array) / sizeof(*array)

namespace
{
	const char *const uris[] = {
		_contacts_contact._uri,
		_contacts_my_profile._uri,
		_contacts_image._uri,
		_contacts_name._uri,
		_contacts_number._uri,
		_contacts_email._uri,
		_contacts_note._uri,
		_contacts_address._uri,
		_contacts_event._uri,
		_contacts_relationship._uri,
		_contacts_url._uri,
		_contacts_messenger._uri,
		_contacts_nickname._uri,
		_contacts_company._uri,
		_contacts_name._uri,
	};

	const struct
	{
		unsigned recordPropId;
		unsigned valuePropId;
		unsigned typePropId;
		unsigned labelPropId;
	} propIds[] = {
		{ _contacts_contact.image,        _contacts_image.path,        _contacts_image.type,        _contacts_image.label },
		{ _contacts_contact.name, 0, 0, 0 },
		{ _contacts_contact.number,       _contacts_number.number,     _contacts_number.type,       _contacts_number.label },
		{ _contacts_contact.email,        _contacts_email.email,       _contacts_email.type,        _contacts_email.label },
		{ _contacts_contact.note,         _contacts_note.note, 0, 0 },
		{ _contacts_contact.address,      _contacts_address.street,    _contacts_address.type,      _contacts_address.label },
		{ _contacts_contact.event,        _contacts_event.date,        _contacts_event.type,        _contacts_event.label },
		{ _contacts_contact.relationship, _contacts_relationship.name, _contacts_relationship.type, _contacts_relationship.label },
		{ _contacts_contact.url,          _contacts_url.url,           _contacts_url.type,          _contacts_url.label },
		{ _contacts_contact.messenger,    _contacts_messenger.im_id,   _contacts_messenger.type,    _contacts_messenger.label },
		{ _contacts_contact.nickname,     _contacts_nickname.name, 0, 0 },
		{ _contacts_contact.company, 0, 0, 0 },
		{ _contacts_contact.name, 0, 0, 0 }
	};

	const unsigned subPropIds[] = {
		_contacts_company.name,
		_contacts_company.job_title,
		_contacts_name.prefix,
		_contacts_name.first,
		_contacts_name.addition,
		_contacts_name.last,
		_contacts_name.suffix,
		_contacts_name.phonetic_first,
		_contacts_name.phonetic_middle,
		_contacts_name.phonetic_last
	};
}

ContactField::ContactField(contacts_record_h record, unsigned fieldId,
		const ContactFieldMetadata *metadata)
	: m_Record(record), m_Uri(getUri(metadata->subType)), m_PropId(getPropertyId(fieldId, metadata->type)),
	  m_FieldId(fieldId), m_Metadata(metadata)
{
}

unsigned ContactField::getId() const
{
	return m_FieldId;
}

ContactFieldType ContactField::getType() const
{
	return m_Metadata->type;
}

unsigned ContactField::getSubType() const
{
	return m_Metadata->subType;
}

contacts_record_h ContactField::getRecord() const
{
	return m_Record;
}

unsigned ContactField::getPropertyId() const
{
	return m_PropId;
}

unsigned ContactField::getPropertyId(unsigned fieldId, ContactFieldType fieldType)
{
	if (fieldId < COUNT(propIds)) {
		switch (fieldType) {
			case TypeArray:
			case TypeObject:
				return propIds[fieldId].recordPropId;
			case TypeText:
			case TypeDate:
				return propIds[fieldId].valuePropId;
			case TypeEnum:
				return propIds[fieldId].typePropId;
			case TypeLabel:
				return propIds[fieldId].labelPropId;
		}
	} else if (fieldId - FieldEnd < COUNT(subPropIds)) {
		return subPropIds[fieldId - FieldEnd];
	}

	return 0;
}

const char *ContactField::getUri() const
{
	return m_Uri;
}

const char *ContactField::getUri(unsigned fieldSubType)
{
	if (fieldSubType < COUNT(uris)) {
		return uris[fieldSubType];
	}

	return nullptr;
}

const ContactFieldMetadata *ContactField::getMetadata() const
{
	return m_Metadata;
}
