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
#include <unordered_map>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class Person;
			class PersonSearchData;

			class SearchProvider : public Contacts::Model::ContactDataProvider
			{
			public:
				SearchProvider();
				virtual ~SearchProvider() override;

				/**
				 * @see ContactDataProvider::getDataList()
				 */
				virtual const DataList &getDataList() override;

				/**
				 * @brief Add @a provider to search provider
				 * @param[in]   provider    Contact data provider
				 */
				void addProvider(ContactDataProvider *provider);

			private:
				typedef std::unordered_map<ContactDataProvider *, DataList::const_iterator> SubProviders;
				using ContactDataProvider::onInserted;

				Contacts::Model::ContactData &insertContact(DataList::const_iterator position, Person &contact,
						ContactDataProvider *provider);

				void onInserted(Person &person, ContactDataProvider *provider);
				void onUpdated(PersonSearchData &searchData, int changes);
				void onDeleted(DataList::iterator contactIt, ContactDataProvider *provider);

				DataList m_ContactList;
				SubProviders m_SubProviders;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_SEARCH_PROVIDER_H */
