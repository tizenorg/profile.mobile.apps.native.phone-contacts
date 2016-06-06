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
#include <contacts.h>

namespace Contacts
{
	namespace Model
	{
		class SearchData;

		class EXPORT_API SearchProvider : public ContactDataProvider
		{
		public:
			/**
			 * @brief Notifies SearchEngine about @a SearchData change
			 * @param[in]   Insert/Update/Delete
			 * @param[in]   SearchData
			 */
			typedef std::function<void(contacts_changed_e, SearchData *)> SearchNotifyCallback;

			/**
			 * @brief Set @ref SearchEngine notify callback
			 * @param[in]   callback    @see SearchNotifyCallback
			 */
			void setSearchNotifyCallback(SearchNotifyCallback callback);

		protected:
			void notifySearch(contacts_changed_e changed, SearchData *searchData);

		private:
			SearchNotifyCallback m_OnSearchNotify;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_PROVIDER_H */
