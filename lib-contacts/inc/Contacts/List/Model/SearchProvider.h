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

#ifndef CONTACTS_LIST_MODEL_SEARCH_PROVIDER_H
#define CONTACTS_LIST_MODEL_SEARCH_PROVIDER_H

#include "Contacts/Model/SearchEngine.h"
#include <unordered_map>

namespace Contacts
{
	namespace Model
	{
		class ContactData;
	}

	namespace List
	{
		namespace Model
		{
			class Person;
			class PersonSearchData;
			class PersonProvider;

			class SearchProvider : public ::Model::DataProvider
			{
			public:
				/**
				 * @brief Create SearchProvider
				 * @param[in]   provider    PersonProvider
				 */
				explicit SearchProvider(PersonProvider &provider);
				virtual ~SearchProvider() override;

				/**
				 * @brief Invoke search
				 * @param[in]   query   Passed search string
				 */
				void search(const char *query);

				/**
				 * @return Found data items
				 */
				virtual const DataList &getDataList() override;

				/**
				 * @see DataProvider::clearDataList()
				 */
				virtual void clearDataList() override;

			private:
				using DataProvider::onInserted;

				Contacts::Model::ContactData &insertPerson(Person &person);

				void onInserted(Person &person);
				void onUpdated(PersonSearchData &searchData, int changes);
				void onDeleted(DataList::iterator personIt);

				PersonProvider &m_Provider;
				DataList m_ContactList;
				Contacts::Model::SearchEngine m_SearchEngine;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_SEARCH_PROVIDER_H */
