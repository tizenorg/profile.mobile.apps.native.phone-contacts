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
				 * @brief Callback function prototype
				 * @param[in]   id  Contact DB record ID
				 */
				typedef std::function<void(int id)> Callback;

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
				DbChangeObserver(const char *viewUri = _contacts_person._uri);
				~DbChangeObserver();

				/**
				 * @brief Add insert callback
				 * @param[in]   callback    New record callback
				 * @return Callback handle
				 */
				CallbackHandle addInsertCallback(Callback callback);

				/**
				 * @brief Remove insert callback
				 * @param[in]   handle      Handle to delete callback
				 */
				void removeInsertCallback(CallbackHandle handle);

				/**
				 * @brief Add update callback
				 * @param[in]   id          Record ID
				 * @param[in]   callback    Updated record callback
				 * @return Callback handle
				 */
				CallbackHandle addUpdateCallback(int id, Callback callback);

				/**
				 * @brief Remove update callback
				 * @param[in]   id          Record ID
				 * @param[in]   handle      Callback handle
				 */
				void removeUpdateCallback(int id, CallbackHandle handle);

				/**
				 * @brief Add delete callback
				 * @param[in]   id          Record ID
				 * @param[in]   callback    Delete callback handle
				 * @return Callback handle
				 */
				CallbackHandle addDeleteCallback(int id, Callback callback);

				/**
				 * @brief Remove delete callback
				 * @param[in]   id          Record ID
				 * @param[in]   handle      Delete callback handle
				 */
				void removeDeleteCallback(int id, CallbackHandle handle);

			private:
				typedef RecordCbs                          InsertCallbacks;
				typedef std::unordered_map<int, RecordCbs> UpdateCallbacks;
				typedef std::unordered_map<int, RecordCbs> DeleteCallbacks;

				void onChanged(const char *viewUri);
				void notify(int id, contacts_changed_e changeType);

				InsertCallbacks m_InsertCallbacks;
				UpdateCallbacks m_UpdateCallbacks;
				DeleteCallbacks m_DeleteCallbacks;

				const char *m_ViewUri;
				int m_DbVersion;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_DB_CHANGE_OBSERVER_H */
