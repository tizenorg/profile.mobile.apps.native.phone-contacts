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

#include "Contacts/List/Model/PersonSearchProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/Model/PersonProvider.h"
#include "Contacts/List/Model/PersonSearchData.h"

#include "Utils/Range.h"

using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace Utils;
using namespace std::placeholders;

PersonSearchProvider::PersonSearchProvider(PersonProvider &provider)
	: m_Provider(provider), m_SearchEngine(m_ContactList)
{
	m_Provider.setInsertCallback([this](::Model::DataItem &data) {
		onInserted(static_cast<Person &>(data));
	});
}

PersonSearchProvider::~PersonSearchProvider()
{
	for (auto &&person : m_ContactList) {
		delete person;
	}
}

void PersonSearchProvider::search(const char *query)
{
	m_SearchEngine.search(query ? query : "");
}

const PersonSearchProvider::DataList &PersonSearchProvider::getDataList()
{
	if (m_ContactList.empty()) {
		for (auto &&data : m_Provider.getDataList()) {
			auto &person = static_cast<Person &>(*data);
			insertPerson(person);
		}
	}

	return m_ContactList;
}

void PersonSearchProvider::clearDataList()
{
	for (auto &&contactData : m_ContactList) {
		delete contactData;
	}
	m_ContactList.clear();
	m_Provider.clearDataList();
}

SearchData &PersonSearchProvider::insertPerson(Person &person)
{
	auto searchData = new PersonSearchData(person);
	m_ContactList.push_back(searchData);

	DataList::iterator personIt = --m_ContactList.end();

	person.setUpdateCallback(std::bind(&PersonSearchProvider::onUpdated, this, std::ref(*searchData), _1));
	person.setDeleteCallback(std::bind(&PersonSearchProvider::onDeleted, this, personIt));

	return *searchData;
}

void PersonSearchProvider::onInserted(Person &person)
{
	SearchData &searchData = insertPerson(person);
	onInserted(searchData);
	m_SearchEngine.insertSearchData(&searchData);
}

void PersonSearchProvider::onUpdated(PersonSearchData &searchData, int changes)
{
	searchData.onUpdated(changes);
}

void PersonSearchProvider::onDeleted(DataList::iterator personIt)
{
	PersonSearchData *searchData = static_cast<PersonSearchData *>(*personIt);
	searchData->onDeleted();

	delete searchData;
	m_ContactList.erase(personIt);
}
