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

#ifndef CONTACTS_LIST_MODEL_DB_CHANGE_OBSERVER_H
#define CONTACTS_LIST_MODEL_DB_CHANGE_OBSERVER_H

#include <contacts.h>
#include <functional>
#include <unordered_map>
#include <list>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Observe changes in contacts DB table
			 */
			class DbChangeObserver
			{
			public:
				/**
				 * @brief Represents what DB table observes this object
				 */
				enum Table
				{
					Contact
				};
				/**
				 * @brief Callback function prototype
				 * @param[in]   id          Contact DB record ID
				 * @param[in]   changeType  Inserted/Updated/Deleted change type
				 */
				typedef std::function<void(int id, contacts_changed_e changeType)> Callback;

			private:
				typedef std::list<Callback> RecordCbs;

			public:
				/**
				 * @brief Callback handle
				 * @remark Used to remove callback
				 */
				typedef RecordCbs::const_iterator CallbackHandle;

				/**
				 * @brief Create new observer object
				 * @param[in]   viewUri     Uri to DB table view
				 */
				DbChangeObserver(Table table = Contact);
				~DbChangeObserver();

				DbChangeObserver(const DbChangeObserver &that) = delete;
				DbChangeObserver &operator=(const DbChangeObserver &that) = delete;

				/**
				 * @brief Add callback
				 * @param[in]   callback    DB change callback
				 * @return Callback handle
				 */
				CallbackHandle addCallback(Callback callback);

				/**
				 * @brief Remove callback
				 * @param[in]   handle      Handle to delete callback
				 */
				void removeCallback(CallbackHandle handle);

				/**
				 * @brief Add callback
				 * @param[in]   id          Record ID
				 * @param[in]   callback    DB change callback
				 * @return Callback handle
				 */
				CallbackHandle addCallback(int id, Callback callback);

				/**
				 * @brief Remove callback
				 * @param[in]   id          Record ID
				 * @param[in]   handle      Callback handle
				 */
				void removeCallback(int id, CallbackHandle handle);

			private:
				typedef RecordCbs                          InsertCallbacks;
				typedef std::unordered_map<int, RecordCbs> ChangeCallbacks;

				void onChanged(const char *viewUri);
				void notifyContactChanges();
				void notify(int id, contacts_changed_e changeType);

				InsertCallbacks m_InsertCallbacks;
				ChangeCallbacks m_ChangeCallbacks;

				const char *m_ViewUri;
				Table m_Table;
				int m_DbVersion;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_DB_CHANGE_OBSERVER_H */
