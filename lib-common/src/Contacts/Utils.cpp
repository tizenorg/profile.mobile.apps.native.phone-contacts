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

#include "Contacts/Utils.h"
#include "Utils/Logger.h"

contacts_record_h Contacts::getDisplayContact(contacts_record_h personRecord)
{
	int id = 0;
	contacts_record_get_int(personRecord, _contacts_person.display_contact_id, &id);

	contacts_record_h record = nullptr;
	int err = contacts_db_get_record(_contacts_contact._uri, id, &record);
	RETVM_IF_ERR(err, nullptr, "contacts_db_get_record() failed.");

	return record;
}

int Contacts::getDisplayContactId(int personId)
{
	contacts_record_h record = 0;
	int err = contacts_db_get_record(_contacts_person._uri, personId, &record);
	RETVM_IF_ERR(err, 0, "contacts_db_get_record() failed.");

	int id = 0;
	contacts_record_get_int(record, _contacts_person.display_contact_id, &id);
	contacts_record_destroy(record, true);

	return id;
}

int Contacts::getPersonId(int contactId)
{
	contacts_record_h record = nullptr;
	int err = contacts_db_get_record(_contacts_contact._uri, contactId, &record);
	RETVM_IF_ERR(err, 0, "contacts_db_get_record() failed.");

	int id = 0;
	contacts_record_get_int(record, _contacts_contact.person_id, &id);
	contacts_record_destroy(record, true);

	return id;
}
