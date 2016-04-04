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

#ifndef CONTACTS_MODEL_SEARCH_PROVIDER_H
#define CONTACTS_MODEL_SEARCH_PROVIDER_H

#include "Contacts/Model/ContactDataProvider.h"
#include "Contacts/Model/ContactData.h"

#include <vector>
#include <unordered_map>

namespace Contacts
{
	namespace Model
	{
		class SearchProvider : public ContactDataProvider
		{
		public:
			SearchProvider();
			virtual ~SearchProvider() override;

			/**
			 * @see ContactDataProvider::getContactDataList()
			 */
			virtual const ContactDataList &getContactDataList() override;

			/**
			 * @brief Add @a provider to search provider
			 * @param[in]   provider    Contact data provider
			 */
			void addProvider(ContactDataProvider *provider);

		private:
			typedef std::unordered_map<ContactDataProvider *, ContactDataList::const_iterator> SubProviders;
			using ContactDataProvider::onInserted;

			ContactData &insertContact(ContactData &contact, ContactDataList::const_iterator position);

			void onInserted(ContactData &contactData, ContactDataProvider *provider);
			void onUpdated(ContactData &searchData, int changes);
			void onDeleted(ContactDataList::iterator contactIt);

			ContactDataList m_ContactList;

			ContactDataProvider *m_LastProvider;
			SubProviders m_SubProviders;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_PROVIDER_H */
