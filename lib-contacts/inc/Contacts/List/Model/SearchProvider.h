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

#include "Contacts/Model/ContactDataProvider.h"
#include "Contacts/Model/SearchEngine.h"
#include <unordered_map>

namespace Contacts
{
	namespace Model
	{
		class ContactDataProvider;
	}

	namespace List
	{
		namespace Model
		{
			class Person;
			class PersonSearchData;
			class PersonProvider;

			class SearchProvider : public Contacts::Model::ContactDataProvider
			{
			public:
				/**
				 * @brief Create SearchProvider
				 * @param[in]   provider    PersonProvider
				 */
				explicit SearchProvider(PersonProvider &provider);
				virtual ~SearchProvider() override;

				/**
				 * @return Found data items
				 */
				virtual const DataList &getDataList() override;

			private:
				using ContactDataProvider::onInserted;
				typedef std::unordered_map<ContactDataProvider *, DataList::const_iterator> SubProviders;

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
