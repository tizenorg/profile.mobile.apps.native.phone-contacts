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

#include "Contacts/Model/ContactRecordProvider.h"
#include "Contacts/Model/ContactRecordData.h"
#include "Contacts/Utils.h"

using namespace Contacts;
using namespace Contacts::Model;
using namespace std::placeholders;

ContactRecordProvider::ContactRecordProvider()
{
	m_Handle = DbChangeObserver::getInstance()->addCallback(
		[this](int id, contacts_changed_e changeType) {
			onInserted(id, changeType);
		}
	);
}

ContactRecordProvider::~ContactRecordProvider()
{
	DbChangeObserver::getInstance()->removeCallback(m_Handle);

	for (auto &&contact : m_ContactList) {
		delete contact;
	}
}

const ContactDataList &ContactRecordProvider::getContactDataList()
{
	if (!m_ContactList.empty()) {
		return m_ContactList;
	}

	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_contact._uri, 0, 0, &list);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		insertContact(record);
	}

	contacts_list_destroy(list, false);

	return m_ContactList;
}

ContactData *ContactRecordProvider::createContact(contacts_record_h record)
{
	return new ContactRecordData(ContactData::TypeContact, record);
}

void ContactRecordProvider::insertContact(contacts_record_h record)
{
	auto contact = static_cast<ContactRecordData *>(createContact(record));
	m_ContactList.push_back(contact);

	contact->setChangedCallback(
			std::bind(&ContactRecordProvider::onChanged, this, --m_ContactList.end(), _1, _2));
}

contacts_record_h ContactRecordProvider::getRecord(int id)
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_contact._uri, id, &record);

	return record;
}

const ContactDataList &ContactRecordProvider::getContactList()
{
	return m_ContactList;
}

void ContactRecordProvider::onInserted(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_INSERTED) {
		auto record = getRecord(id);
		if (record) {
			insertContact(record);
			onInserted(*m_ContactList.back());
		}
	}
}

void ContactRecordProvider::onChanged(ContactDataList::iterator contactIt, int id, contacts_changed_e changeType)
{
	auto contact = static_cast<ContactRecordData *>(*contactIt);

	if (changeType == CONTACTS_CHANGE_UPDATED) {
		auto record = getRecord(id);
		if (record) {
			contact->onUpdate(record);
			updateChangedCallback(contactIt);

			return;
		}
	}

	contact->onDeleted();
	delete contact;
	m_ContactList.erase(contactIt);
}
