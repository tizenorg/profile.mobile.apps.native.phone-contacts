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

#include "Contacts/Model/SearchProvider.h"
#include "Contacts/Model/SearchData.h"

using namespace Contacts::Model;
using namespace std::placeholders;

const ContactDataList &SearchProvider::getContactDataList()
{
	return m_ContactList;//Todo: Return filtered list by search engine instead of all contacts
}

void SearchProvider::addProvider(ContactDataProvider *provider)
{
	const ContactDataList &list = provider->getContactDataList();
	for (auto it = list.begin(); it != list.end(); ++it) {
		insertContact(**it);
		if (it == list.begin() && !m_Bounds.empty()) {
			m_Bounds.back() = it;
		}
	}

	m_Bounds.push_back(m_ContactList.end());

	m_SubProviders.insert({ provider, --m_Bounds.end() });
	provider->setInsertCallback(std::bind(&SearchProvider::onInserted, this, _1, provider));
}

void SearchProvider::insertContact(ContactData &contact)
{
	auto contactData = new SearchData(contact, contact.getType());
	m_ContactList.push_back(contactData);

	auto contactIt = --m_ContactList.end();

	contactData->setUpdateCallback(std::bind(&SearchProvider::onUpdated, this, contactData, _1));
	contactData->setDeleteCallback(std::bind(&SearchProvider::onDeleted, this, contactIt));
}

void SearchProvider::onInserted(ContactData &contactData, ContactDataProvider *provider)
{
	insertContact(contactData);
	provider->onInserted(contactData);
}

void SearchProvider::onUpdated(ContactData *contactData, int changes)
{
	contactData->onUpdated(changes);
}

void SearchProvider::onDeleted(ContactDataList::iterator contactIt)
{
	auto contactData = *contactIt;
	contactData->onDeleted();

	delete contactData;
	m_ContactList.erase(contactIt);
}
