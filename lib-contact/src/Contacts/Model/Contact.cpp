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

Contact::Contact(ContactObjectType type, int id)
	: ContactObject(*getContactMetadata(type)), m_IsNew(id <= 0)
{
	const char *uri = getObjectMetadata().uri;
	contacts_record_h record = nullptr;

	if (m_IsNew) {
		contacts_record_create(uri, &record);
	} else {
		contacts_db_get_record(uri, id, &record);
	}

	setRecord(record);
}

Contact::~Contact()
{
	contacts_record_destroy(getRecord(), true);
}

bool Contact::isNew() const
{
	return m_IsNew;
}

void Contact::save()
{
	if (m_IsNew) {
		contacts_db_insert_record(getRecord(), nullptr);
	} else {
		contacts_db_update_record(getRecord());
	}
}
