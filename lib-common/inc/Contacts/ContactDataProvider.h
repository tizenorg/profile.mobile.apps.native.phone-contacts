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

#ifndef CONTACTS_CONTACT_DATA_PROVIDER_H
#define CONTACTS_CONTACT_DATA_PROVIDER_H

#include "Contacts/ContactData.h"

#include <functional>
#include <tizen.h>

namespace Contacts
{
	class EXPORT_API ContactDataProvider
	{
	public:
		/**
		 * @brief Insert callback
		 * @param[in] Contact data
		 */
		typedef std::function<void(ContactDataPtr)> InsertCallback;

		virtual ~ContactDataProvider() { }

		/**
		 * @return Contact data list
		 */
		virtual const ContactDataList &getContactDataList() = 0;

		/**
		 * @brief Set insert callback
		 * @param[in]   callback    Create callback
		 */
		void setInsertCallback(InsertCallback callback);

		/**
		 * @brief Unset insert callback
		 */
		void unsetInsertCallback();

	protected:
		/**
		 * @param[in]   id      ContactData ID
		 * @return New ContactData
		 */
		virtual ContactDataPtr createContactData(int id) = 0;

		/**
		 * @brief ContactData inserted callback
		 */
		void onInserted(int id);

	private:
		InsertCallback m_InsertCallback;
	};
}

#endif /* CONTACTS_CONTACT_DATA_PROVIDER_H */
