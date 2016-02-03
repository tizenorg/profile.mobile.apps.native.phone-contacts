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

#include "Contacts/Model/Contact.h"
#include "Contacts/Model/ContactFieldMetadata.h"
#include "Contacts/Utils.h"
#include "Utils/Callback.h"

using namespace Contacts::Model;

Contact::Contact(ContactObjectType type)
	: ContactObject(nullptr, *getContactMetadata(type)),
	  m_IsNew(true), m_DbVersion(0)
{
}

Contact::~Contact()
{
	unsetChangeCallback();
	contacts_record_destroy(getRecord(), true);
}

int Contact::initialize(int recordId)
{
	const char *uri = getObjectMetadata().uri;
	contacts_record_h record = nullptr;
	int err = CONTACTS_ERROR_NONE;

	m_IsNew = (recordId <= 0);
	if (m_IsNew) {
		err = contacts_record_create(uri, &record);
	} else {
		err = contacts_db_get_record(uri, recordId, &record);
		setChangeCallback();
	}

	ContactObject::initialize(record);
	return err;
}

bool Contact::isNew() const
{
	return m_IsNew;
}

int Contact::save()
{
	int err = CONTACTS_ERROR_NONE;
	if (m_IsNew) {
		err = contacts_db_insert_record(getRecord(), nullptr);
		setChangeCallback();
	} else {
		err = contacts_db_update_record(getRecord());
	}

	return err;
}

int Contact::remove()
{
	int err = CONTACTS_ERROR_NONE;
	if (!m_IsNew) {
		unsetChangeCallback();
		contacts_db_delete_record(getObjectMetadata().uri, getRecordId());
	}

	return err;
}

void Contact::setChangeCallback()
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(getObjectMetadata().uri,
			makeCallbackWithLastParam(&Contact::onDatabaseChanged), this);
}

void Contact::unsetChangeCallback()
{
	contacts_db_remove_changed_cb(getObjectMetadata().uri,
			makeCallbackWithLastParam(&Contact::onDatabaseChanged), this);
}

int Contact::getContactChange()
{
	int changeType = -1;

	contacts_list_h list = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0,
			m_DbVersion, &list, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		int id = 0;
		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &id);

		if (id == getRecordId()) {
			contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);
			break;
		}
	}

	contacts_list_destroy(list, true);
	return changeType;
}

int Contact::getMyProfileChange()
{
	contacts_list_h list = nullptr;
	contacts_db_get_changes_by_version(_contacts_my_profile_updated_info._uri, 0,
			m_DbVersion, &list, &m_DbVersion);

	contacts_record_h record = nullptr;
	contacts_list_get_current_record_p(list, &record);

	int changeType = -1;
	contacts_record_get_int(record, _contacts_my_profile_updated_info.last_changed_type, &changeType);
	contacts_list_destroy(list, true);

	return changeType;
}

void Contact::onDatabaseChanged(const char *uri)
{
	int changeType = (getSubType() == ObjectTypeContact)
			? getContactChange() : getMyProfileChange();

	if (changeType == CONTACTS_CHANGE_UPDATED) {
		contacts_record_h oldRecord = getRecord();
		contacts_record_h newRecord = nullptr;

		contacts_db_get_record(getObjectMetadata().uri, getRecordId(), &newRecord);
		update(newRecord);
		contacts_record_destroy(oldRecord, true);
	} else if (changeType == CONTACTS_CHANGE_DELETED) {
		update(nullptr);
	}
}
