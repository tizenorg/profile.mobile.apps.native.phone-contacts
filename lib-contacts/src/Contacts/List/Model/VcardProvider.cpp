/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/Model/ContactRecordData.h"
#include "Contacts/List/Model/VcardProvider.h"
#include "Utils/Logger.h"

using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List::Model;

VcardProvider::VcardProvider(std::string vcardPath)
{
	int err = contacts_vcard_parse_to_contact_foreach(vcardPath.c_str(), [](contacts_record_h record, void *data)->bool {
		RETVM_IF(!record || !data, true, "invalid data");
		DataList *list = (DataList *)data;

		contacts_record_h recordClone;
		contacts_record_clone(record, &recordClone);
		ContactRecordData *contact = new ContactRecordData(recordClone);
		list->push_back(contact);

		// Return true to continue to scan next contact, according to contacts_vcard_parse_cb specification.
		return true;
	}, &m_ContactsList);
	WARN_IF_ERR(err, "contacts_vcard_parse_to_contact_foreach() failed.");
}

VcardProvider::~VcardProvider()
{
	for (auto &&contact : m_ContactsList) {
		delete contact;
	}
}

const VcardProvider::DataList &VcardProvider::getDataList()
{
	return m_ContactsList;
}
