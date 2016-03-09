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

#include "Contacts/List/Model/ContactRecordData.h"
#include "Contacts/List/Model/VcardProvider.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"

using namespace Contacts;
using namespace Contacts::List::Model;

VcardProvider::VcardProvider(const char *path)
{
	contacts_list_h list = getListFromVcard(path);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		ContactRecordData *contact = new ContactRecordData(ContactData::TypeContact);
		contact->updateContactRecord(record);
		m_ContactsList.push_back(contact);
	}

	contacts_list_destroy(list, false);
}

VcardProvider::~VcardProvider()
{
	for (auto &&contact : m_ContactsList) {
		delete contact;
	}
}

const ContactDataList &VcardProvider::getContactDataList()
{
	return m_ContactsList;
}

contacts_list_h VcardProvider::getListFromVcard(const char *path)
{
	contacts_list_h list = nullptr;

	FILE *file = fopen(path, "r");
	RETVM_IF(!file, nullptr, "fopen() failed");
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);

	char *stream = new char[size];
	if (stream) {
		fread(stream, 1, size, file);

		int err = contacts_vcard_parse_to_contacts(stream, &list);
		WARN_IF_ERR(err, "contacts_vcard_parse_to_contacts() failed.");
		delete[] stream;
	}

	fclose(file);
	return list;
}
