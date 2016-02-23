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

#ifndef CONTACTS_PROVIDER_H
#define CONTACTS_PROVIDER_H

#include "Contacts/DbChangeObserver.h"
#include "Contacts/ContactData.h"

#include <functional>
#include <tizen.h>
#include <utility>

namespace Contacts
{
	class EXPORT_API Provider
	{
	public:
		/**
		 * @brief Insert callback
		 * @param[in] Result object
		 */
		typedef std::function<void(ContactDataPtr)> InsertCallback;

		virtual ~Provider() { }

		/**
		 * @return Result list
		 */
		virtual const ContactDataList &getResultList() = 0;

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
		 * @brief Create new ContactData instance
		 * @param[in]   id   ID of new ContactData instance
		 */
		virtual ContactDataPtr createContactData(int id) = 0;

	private:
		void onInserted(int id, contacts_changed_e changeType);

		std::pair<InsertCallback, DbChangeObserver::CallbackHandle> m_InsertCallback;
	};
}

#endif /* CONTACTS_PROVIDER_H */
