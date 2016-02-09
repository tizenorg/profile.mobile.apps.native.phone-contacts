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

#include "Contacts/List/Model/DbChangeObserver.h"
#include "Contacts/Utils.h"
#include "Utils/Callback.h"

using namespace Contacts::List::Model;

namespace
{
	const char *uris[] = {
		/* Contact   = */ _contacts_contact._uri
	};
}

DbChangeObserver::DbChangeObserver(Table table)
	: m_ViewUri(uris[table])
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

DbChangeObserver::CallbackHandle DbChangeObserver::addCallback(Callback callback)
{
	m_InsertCallbacks.push_back(std::move(callback));
	return --m_InsertCallbacks.cend();
}

void DbChangeObserver::removeCallback(CallbackHandle handle)
{
	m_InsertCallbacks.erase(handle);
}

DbChangeObserver::CallbackHandle DbChangeObserver::addCallback(int id, Callback callback)
{
	m_ChangeCallbacks[id].push_back(std::move(callback));
	return --m_ChangeCallbacks[id].cend();
}

void DbChangeObserver::removeCallback(int id, CallbackHandle handle)
{
	auto it = m_ChangeCallbacks.find(id);
	if (it != m_ChangeCallbacks.end()) {
		it->second.erase(handle);
	}
}

void DbChangeObserver::onChanged(const char *viewUri)
{
	switch (m_Table) {
		case Contact: notifyContactChanges(); break;
		default: break;
	}
}

void DbChangeObserver::notifyContactChanges()
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		int contactId = 0;
		int changeType = -1;

		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
		contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);

		notify(contactId, static_cast<contacts_changed_e>(changeType));
	}

	contacts_list_destroy(changes, true);
}

void DbChangeObserver::notify(int id, contacts_changed_e changeType)
{
	auto notifyAll = [id, changeType](const RecordCbs &callbacks) {
		for (auto &&callback : callbacks) {
			if (callback) {
				callback(id, changeType);
			}
		}
	};

	auto it = m_ChangeCallbacks.find(id);
	if (it != m_ChangeCallbacks.end()) {
		notifyAll(it->second);
	}
	notifyAll(m_InsertCallbacks);
}
