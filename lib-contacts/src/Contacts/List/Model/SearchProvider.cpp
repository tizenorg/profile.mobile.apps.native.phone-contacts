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

#include "Contacts/List/Model/SearchProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/Model/PersonProvider.h"
#include "Contacts/List/Model/PersonSearchData.h"
#include "Utils/Range.h"

using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace Utils;
using namespace std::placeholders;

SearchProvider::SearchProvider(PersonProvider &provider)
	: m_Provider(provider)
{
	for (auto &&data : m_Provider.getDataList()) {
		auto &person = static_cast<Person &>(*data);
		insertPerson(person);
	}

	m_Provider.setInsertCallback([this](ContactData &contactData) {
		onInserted(static_cast<Person &>(contactData));
	});
}

SearchProvider::~SearchProvider()
{
	for (auto &&person : m_ContactList) {
		delete person;
	}
}

const SearchProvider::DataList &SearchProvider::getDataList()
{
	return m_ContactList;//Todo: Return filtered list by search engine instead of all contacts
}

ContactData &SearchProvider::insertPerson(Person &person)
{
	auto searchData = new PersonSearchData(person);
	m_ContactList.push_back(searchData);

	DataList::iterator personIt = --m_ContactList.end();

	person.setUpdateCallback(std::bind(&SearchProvider::onUpdated, this, std::ref(*searchData), _1));
	person.setDeleteCallback(std::bind(&SearchProvider::onDeleted, this, personIt));

	return *searchData;
}

void SearchProvider::onInserted(Person &person)
{
	ContactData &searchData = insertPerson(person);
	onInserted(searchData);
}

void SearchProvider::onUpdated(PersonSearchData &searchData, int changes)
{
	searchData.onUpdated(changes);
}

void SearchProvider::onDeleted(DataList::iterator personIt)
{
	PersonSearchData *searchData = static_cast<PersonSearchData *>(*personIt);
	searchData->onDeleted();

	delete searchData;
	m_ContactList.erase(personIt);
}
