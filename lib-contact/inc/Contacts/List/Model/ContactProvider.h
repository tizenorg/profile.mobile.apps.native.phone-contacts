/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACT_PROVIDER_H
#define CONTACT_PROVIDER_H

#include <contacts.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of contacts
			 */
			class ContactProvider
			{
			public:

				/**
				 * @brief Determines how to filter contact list
				 */
				enum Filter {
					FilterNone = 0,
					FilterNumber = 1 << 0,
					FilterEmail = 1 << 1
				};

				/**
				 * @brief Person change callback
				 * @param[in]    id            Person ID
				 * @param[in]    changeType    Change type
				 */
				typedef std::function<void(int id, contacts_changed_e changeType)> ChangeCallback;

				/**
				 * @brief Person create callback
				 * @param[in]    record        Person record
				 */
				typedef std::function<void(contacts_record_h record)> CreateCallback;

				/**
				 * @brief Constructor
				 * @param[in]    filter    Contact filter
				 */
				ContactProvider(Filter filter);

				/**
				 * @brief Destructor
				 */
				virtual ~ContactProvider(){};

				/**
				 * @brief Get contact list
				 * @return Person list
				 */
				virtual contacts_list_h getContactsList() const;

				/**
				 * @brief Set person change callback
				 * @remark It can be update or delete of person
				 * @param[in]    id          Person ID
				 * @param[in]    callback    Change callback
				 */
				void setChangeCallback(int id, ChangeCallback callback);

				/**
				 * @brief Unset person change callback
				 * @param[in]    id    Person ID
				 */
				void unsetChangeCallback(int id);

				/**
				 * @brief Set person create callback
				 * @param[in]    callback    Create callback
				 */
				void setCreateCallback(CreateCallback callback);

			private:
				void changedCb(const char *viewUri);
				void notify(contacts_changed_e changeType, contacts_record_h record);

				Filter m_ListFilter;
				int m_DbVersion;

				std::unordered_map<int, ChangeCallback> m_ChangeCallbacks;
				std::vector<CreateCallback> m_CreateCallbacks;
			};
		}
	}
}

#endif /* CONTACT_PROVIDER_H */
