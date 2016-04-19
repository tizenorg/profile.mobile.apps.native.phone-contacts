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

#include "Contacts/Model/DbChangeObserver.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Callback.h"
#include "Utils/Range.h"

using namespace Contacts::Model;

namespace
{
	const char *uris[] = {
		/* Contact   = */ _contacts_contact._uri
	};
}

DbChangeObserver::DbChangeObserver(Table table)
	: m_Table(table), m_DbVersion(0), m_IsSubscribed(false)
{
}

DbChangeObserver::~DbChangeObserver()
{
	unsubscribe();
}

DbChangeObserver *DbChangeObserver::getInstance(Table table)
{
	if (table < 0 || table >= Utils::count(m_Observers)) {
		return nullptr;
	}

	return &m_Observers[table];
}

DbChangeObserver::CallbackHandle DbChangeObserver::addCallback(Callback callback)
{
	return addCallback(m_Callbacks, std::move(callback));
}

void DbChangeObserver::removeCallback(CallbackHandle handle)
{
	m_Callbacks.erase(handle);

	if (m_Callbacks.empty() && m_ChangeCallbacks.empty()) {
		unsubscribe();
	}
}

DbChangeObserver::CallbackHandle DbChangeObserver::addCallback(int id, Callback callback)
{
	return addCallback(m_ChangeCallbacks[id], std::move(callback));
}

void DbChangeObserver::removeCallback(int id, CallbackHandle handle)
{
	auto it = m_ChangeCallbacks.find(id);
	if (it != m_ChangeCallbacks.end()) {
		it->second.erase(handle);

		if (it->second.empty()) {
			m_ChangeCallbacks.erase(it);
		}
	}

	if (m_Callbacks.empty() && m_ChangeCallbacks.empty()) {
		unsubscribe();
	}
}

void DbChangeObserver::subscribe()
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(uris[m_Table],
			makeCallbackWithLastParam(&DbChangeObserver::onChanged), this);

	m_IsSubscribed = true;
}

void DbChangeObserver::unsubscribe()
{
	m_DbVersion = 0;
	contacts_db_remove_changed_cb(uris[m_Table],
			makeCallbackWithLastParam(&DbChangeObserver::onChanged), this);

	m_IsSubscribed = false;
}

DbChangeObserver::CallbackHandle DbChangeObserver::addCallback(RecordCbs &recordCbs,
		Callback callback)
{
	if (!m_IsSubscribed) {
		subscribe();
	}

	recordCbs.push_back(std::move(callback));
	return --recordCbs.cend();
}

void DbChangeObserver::onChanged(const char *viewUri)
{
	switch (m_Table) {
		case TableContact: notifyContactChanges(); break;
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
		for (auto it = callbacks.begin(); it != callbacks.end(); ) {
			auto &callback = *it++;
			if (callback) {
				callback(id, changeType);
			}
		}
	};

	auto it = m_ChangeCallbacks.find(id);
	if (it != m_ChangeCallbacks.end()) {
		notifyAll(it->second);
	}
	notifyAll(m_Callbacks);
}

DbChangeObserver DbChangeObserver::m_Observers[DbChangeObserver::TableMax] = {
	/* TableContact = */ { TableContact }
};
