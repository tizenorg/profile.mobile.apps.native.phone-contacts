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

#ifndef CONTACTS_MODEL_CONTACT_DATA_PROVIDER_H
#define CONTACTS_MODEL_CONTACT_DATA_PROVIDER_H

#include "Contacts/Model/ContactData.h"

#include <functional>
#include <tizen.h>

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API ContactDataProvider
		{
		public:
			typedef std::list<ContactData *> DataList;

			/**
			 * @brief Change event types.
			 */
			enum ChangeType
			{
				ChangeInsert = 1 << 0,
				ChangeUpdate = 1 << 1,
				ChangeDelete = 1 << 2
			};

			/**
			 * @brief Insert callback
			 * @param[in] Contact data
			 */
			typedef std::function<void(ContactData &)> InsertCallback;

			virtual ~ContactDataProvider() { }

			/**
			 * @return Contact data list
			 */
			virtual const DataList &getDataList() = 0;

			/**
			 * @brief Clear contact data list
			 */
			virtual void clearDataList() { }

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
			 * @brief ContactData inserted callback
			 * @param[in]   contactData     Contact data
			 */
			void onInserted(ContactData &contactData);

		private:
			InsertCallback m_OnInserted;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_DATA_PROVIDER_H */
