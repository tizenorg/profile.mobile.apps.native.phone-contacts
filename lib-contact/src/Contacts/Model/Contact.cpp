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

#include "Contacts/Model/Contact.h"
#include "Contacts/Model/ContactFieldMetadata.h"

using namespace Contacts::Model;

Contact::Contact(ContactObjectType type)
	: ContactObject(*getContactMetadata(type)), m_IsNew(true)
{
}

Contact::~Contact()
{
	contacts_record_destroy(getRecord(), true);
}

int Contact::initialize(int recordId)
{
	const char *uri = getObjectMetadata().uri;
	contacts_record_h record = nullptr;
	int err = CONTACTS_ERROR_NONE;

	m_IsNew = (recordId <= 0);
	if (m_IsNew) {
		err = contacts_record_create(uri, &record);
	} else {
		err = contacts_db_get_record(uri, recordId, &record);
	}

	setRecord(record);
	ContactObject::initialize();

	return err;
}

bool Contact::isNew() const
{
	return m_IsNew;
}

int Contact::save()
{
	int err = CONTACTS_ERROR_NONE;
	if (m_IsNew) {
		err = contacts_db_insert_record(getRecord(), nullptr);
	} else {
		err = contacts_db_update_record(getRecord());
	}

	return err;
}
