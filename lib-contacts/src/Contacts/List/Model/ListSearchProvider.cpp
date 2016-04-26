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

#include "Contacts/List/Model/ListSearchProvider.h"
#include "Contacts/List/Model/PersonSearchData.h"
#include "Contacts/Model/SearchEngine.h"
#include "Utils/Range.h"

using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace Utils;
using namespace std::placeholders;

ListSearchProvider::ListSearchProvider(SearchEngine &searchEngine)
	: m_SearchEngine(searchEngine)
{
}

ListSearchProvider::~ListSearchProvider()
{
	for (auto &&contact : m_ContactList) {
		delete contact;
	}
}

const ListSearchProvider::DataList &ListSearchProvider::getDataList()
{
	return m_SearchEngine.empty() ? m_ContactList : *m_SearchEngine.getSearchResult();
}

void ListSearchProvider::addProvider(ContactDataProvider *provider)
{
	const DataList &list = provider->getDataList();
	for (auto it = list.begin(); it != list.end(); ++it) {
		insertContact(m_ContactList.end(), **it, provider);
	}

	m_SubProviders.insert({ provider, --m_ContactList.end() });
	provider->setInsertCallback(
		[this, provider](ContactData &contactData) {
			onInserted(contactData, provider);
		});
}

ContactData &ListSearchProvider::insertContact(DataList::const_iterator position, ContactData &contact,
		ContactDataProvider *provider)
{
	auto searchData = new PersonSearchData(contact);
	auto newDataIt = m_ContactList.insert(position, searchData);

	DataList::iterator contactIt = --m_ContactList.end();

	contact.setUpdateCallback(std::bind(&ListSearchProvider::onUpdated, this, std::ref(*searchData), _1));
	contact.setDeleteCallback(std::bind(&ListSearchProvider::onDeleted, this, contactIt, provider));

	if (m_SubProviders.find(provider) != m_SubProviders.end()) {
		m_SubProviders[provider] = newDataIt;
	}

	return *searchData;
}

void ListSearchProvider::onInserted(ContactData &contactData, ContactDataProvider *provider)
{
	auto position = Utils::advance(m_SubProviders[provider], 1);
	ContactData &searchData = insertContact(position, contactData, provider);
	onInserted(searchData);
}

void ListSearchProvider::onUpdated(PersonSearchData &searchData, int changes)
{
	searchData.onUpdated(changes);
}

void ListSearchProvider::onDeleted(DataList::iterator contactIt, ContactDataProvider *provider)
{
	PersonSearchData *searchData = static_cast<PersonSearchData *>(*contactIt);
	searchData->onDeleted();

	delete searchData;
	DataList::const_iterator newBound = m_ContactList.erase(contactIt);

	DataList::const_iterator &oldBound = m_SubProviders[provider];
	if (oldBound == contactIt) {
		oldBound = Utils::advance(newBound, -1);
	}
}
