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

SearchProvider::SearchProvider()
	: m_LastProvider(nullptr)
{
}

SearchProvider::~SearchProvider()
{
	for (auto &&contact : m_ContactList) {
		delete contact;
	}
}

const ContactDataList &SearchProvider::getContactDataList()
{
	return m_ContactList;//Todo: Return filtered list by search engine instead of all contacts
}

void SearchProvider::addProvider(ContactDataProvider *provider)
{
	const ContactDataList &list = provider->getContactDataList();
	for (auto it = list.begin(); it != list.end(); ++it) {
		insertContact(**it, m_ContactList.end());
		if (it == list.begin() && m_LastProvider) {
			m_SubProviders.find(m_LastProvider)->second = it;
		}
	}

	m_SubProviders.insert({ provider, m_ContactList.end() });
	provider->setInsertCallback(
		[this, provider](ContactData &contactData) {
			onInserted(contactData, provider);
		});

	m_LastProvider = provider;
}

ContactData &SearchProvider::insertContact(ContactData &contact, ContactDataList::const_iterator position)
{
	auto searchData = new SearchData(contact, contact.getType());
	m_ContactList.insert(position, searchData);

	ContactDataList::iterator contactIt = --m_ContactList.end();

	contact.setUpdateCallback(std::bind(&SearchProvider::onUpdated, this, std::ref(*searchData), _1));
	contact.setDeleteCallback(std::bind(&SearchProvider::onDeleted, this, contactIt));

	return *searchData;
}

void SearchProvider::onInserted(ContactData &contactData, ContactDataProvider *provider)
{
	ContactData &searchData = insertContact(contactData, m_SubProviders.find(provider)->second);
	onInserted(searchData);
}

void SearchProvider::onUpdated(ContactData &searchData, int changes)
{
	searchData.onUpdated(changes);
}

void SearchProvider::onDeleted(ContactDataList::iterator contactIt)
{
	ContactData *searchData = *contactIt;
	searchData->onDeleted();

	delete searchData;
	m_ContactList.erase(contactIt);
}
