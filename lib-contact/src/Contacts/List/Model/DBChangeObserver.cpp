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

#include "Contacts/List/Model/DBChangeObserver.h"
#include "Contacts/Utils.h"
#include "Utils/Callback.h"

using namespace Contacts::List::Model;

namespace
{
	typedef std::unordered_map<int, contacts_changed_e> Changes;

	Changes getChanges(int &dbVersion)
	{
		Changes changes;

		contacts_list_h contactsChanges = nullptr;
		contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, dbVersion, &contactsChanges, &dbVersion);

		contacts_record_h record = nullptr;
		CONTACTS_LIST_FOREACH(contactsChanges, record) {
			int contactId = 0;
			int changeType = -1;

			contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
			contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);

			changes[contactId] = static_cast<contacts_changed_e>(changeType);
		}

		contacts_list_destroy(contactsChanges, true);

		return changes;
	}
}

DbChangeObserver::DbChangeObserver(const char *viewUri)
	: m_ViewUri(viewUri)
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(m_ViewUri,
			makeCallbackWithLastParam(&DbChangeObserver::onChanged), this);
}

DbChangeObserver::~DbChangeObserver()
{
	contacts_db_remove_changed_cb(m_ViewUri,
			makeCallbackWithLastParam(&DbChangeObserver::onChanged), this);
}

DbChangeObserver::CallbackHandle DbChangeObserver::addInsertCallback(Callback callback)
{
	m_InsertCallbacks.push_back(std::move(callback));
	return --m_InsertCallbacks.cend();
}

void DbChangeObserver::removeInsertCallback(CallbackHandle handle)
{
	m_InsertCallbacks.erase(handle);
}

DbChangeObserver::CallbackHandle DbChangeObserver::addUpdateCallback(int id, Callback callback)
{
	m_UpdateCallbacks[id].push_back(std::move(callback));
	return --m_UpdateCallbacks[id].cend();
}

void DbChangeObserver::removeUpdateCallback(int id, CallbackHandle handle)
{
	m_UpdateCallbacks[id].erase(handle);
}

DbChangeObserver::CallbackHandle DbChangeObserver::addDeleteCallback(int id, Callback callback)
{
	m_DeleteCallbacks[id].push_back(std::move(callback));
	return --m_DeleteCallbacks[id].cend();
}

void DbChangeObserver::removeDeleteCallback(int id, CallbackHandle handle)
{
	m_DeleteCallbacks[id].erase(handle);
}

void DbChangeObserver::onChanged(const char *viewUri)
{
	Changes changes = getChanges(m_DbVersion);

	for (auto &&change : changes) {
		notify(change.first, change.second);
	}
}

void DbChangeObserver::notify(int id, contacts_changed_e changeType)
{
	RecordCbs callbacks;
	switch (changeType) {
		case CONTACTS_CHANGE_INSERTED: callbacks = m_InsertCallbacks;     break;
		case CONTACTS_CHANGE_UPDATED:  callbacks = m_UpdateCallbacks[id]; break;
		case CONTACTS_CHANGE_DELETED:  callbacks = m_DeleteCallbacks[id]; break;
		default: break;
	}

	for (auto &&callback : callbacks) {
		if (callback) {
			callback(id);
		}
	}
}
