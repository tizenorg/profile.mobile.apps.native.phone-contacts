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

#include "Contacts/List/Model/ContactNumberProvider.h"
#include "Contacts/List/Model/ContactNumberData.h"
#include "Contacts/Utils.h"

using namespace Contacts;
using namespace Contacts::List::Model;
using namespace std::placeholders;

ContactNumberProvider::ContactNumberProvider()
{
	m_Handle = DbChangeObserver::getInstance()->addCallback(
			std::bind(&ContactNumberProvider::onContactInserted, this, _1, _2));
}

const ContactDataList &ContactNumberProvider::getContactDataList()
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_contact._uri, 0, 0, &list);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		auto contact = new ContactNumberData(record);
		m_ContactList.push_back(contact);

		contact->setChangedCallback(
				std::bind(&ContactNumberProvider::onChanged, this, --m_ContactList.end(), _1, _2));
	}

	contacts_list_destroy(list, false);

	return m_ContactList;
}

void ContactNumberProvider::onContactInserted(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_INSERTED) {
		contacts_record_h record = nullptr;
		contacts_db_get_record(_contacts_contact._uri, id, &record);
		m_ContactList.push_back(new ContactNumberData(record));

		onInserted(*m_ContactList.back());
	}
}

void ContactNumberProvider::onChanged(ContactDataList::iterator contactIt, int id, contacts_changed_e changeType)
{
	auto contact = static_cast<ContactNumberData *>(*contactIt);
	contact->onChanged(id, changeType);

	if (changeType == CONTACTS_CHANGE_UPDATED) {
		contact->setChangedCallback(
				std::bind(&ContactNumberProvider::onChanged, this, contactIt, _1, _2));
	} else if (changeType == CONTACTS_CHANGE_DELETED) {
		delete contact;
		m_ContactList.erase(contactIt);
	}
}
