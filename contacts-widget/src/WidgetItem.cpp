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

#include "WidgetItem.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Logger.h"

using namespace Common;
using namespace Common::Database;

struct Metadata
{
	const char *uri;
	unsigned dataIdProp;
	unsigned contactIdProp;
	unsigned dataProp;
	unsigned nameProp;
	unsigned imageProp;
};

namespace
{
	const Metadata numberMetadata = {
		_contacts_contact_number._uri, _contacts_contact_number.number_id,
		_contacts_contact_number.contact_id, _contacts_contact_number.number,
		_contacts_contact_number.display_name, _contacts_contact_number.image_thumbnail_path
	};

	const Metadata emailMetadata = {
		_contacts_contact_email._uri, _contacts_contact_email.email_id,
		_contacts_contact_email.contact_id, _contacts_contact_email.email,
		_contacts_contact_email.display_name, _contacts_contact_email.image_thumbnail_path
	};

	const Metadata *metadata[] = {
		/* ActionCall    = */ &numberMetadata,
		/* ActionMessage = */ &numberMetadata,
		/* ActionEmail   = */ &emailMetadata
	};
}

WidgetItem::WidgetItem(ActionType actionType, int dataId)
	: m_Id(0), m_ActionType(actionType),
	  m_Metadata(metadata[m_ActionType]), m_Record(fetchRecord(dataId))
{
}

WidgetItem::WidgetItem(int id, ActionType actionType, int dataId)
	: WidgetItem(actionType, dataId)
{
	m_Id = id;
}

WidgetItem::WidgetItem(WidgetItem &&that)
	: m_Id(that.m_Id), m_ActionType(that.m_ActionType),
	  m_Metadata(that.m_Metadata), m_Record(that.m_Record),
	  m_OnChanged(std::move(that.m_OnChanged)), m_OnDelete(std::move(that.m_OnDelete))
{
	that.m_Record = nullptr;
}

WidgetItem::~WidgetItem()
{
	contacts_record_destroy(m_Record, true);
}

void WidgetItem::setChangeCallback(ChangeCallback callback)
{
	m_OnChanged = std::move(callback);
}

void WidgetItem::setDeleteCallback(DeleteCallback callback)
{
	m_OnDelete = std::move(callback);
}

bool WidgetItem::update()
{
	contacts_record_h newRecord = fetchRecord(getDataId());
	if (newRecord) {
		contacts_record_h oldRecord = m_Record;
		m_Record = newRecord;

		if (m_OnChanged) {
			int changes = getChanges(oldRecord, newRecord);
			if (changes != ChangeNone) {
				m_OnChanged(changes);
			}
		}

		contacts_record_destroy(oldRecord, true);
	} else {
		if (m_OnDelete) {
			m_OnDelete();
		}
	}

	return newRecord != nullptr;
}

ActionType WidgetItem::getActionType() const
{
	return m_ActionType;
}

int WidgetItem::getId() const
{
	return m_Id;
}

void WidgetItem::setId(int id)
{
	m_Id = id;
}

int WidgetItem::getDataId() const
{
	return getRecordInt(m_Record, m_Metadata->dataIdProp);
}

int WidgetItem::getContactId() const
{
	return getRecordInt(m_Record, m_Metadata->contactIdProp);
}

const char *WidgetItem::getData() const
{
	return getRecordStr(m_Record, m_Metadata->dataProp);
}

const char *WidgetItem::getName() const
{
	return getRecordStr(m_Record, m_Metadata->nameProp);
}

const char *WidgetItem::getImagePath() const
{
	return getRecordStr(m_Record, m_Metadata->imageProp);
}

contacts_record_h WidgetItem::fetchRecord(int dataId)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(m_Metadata->uri, &filter);
	contacts_filter_add_int(filter, m_Metadata->dataIdProp, CONTACTS_MATCH_EQUAL, dataId);

	contacts_query_h query = nullptr;
	contacts_query_create(m_Metadata->uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	contacts_record_h record = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");
	if (list) {
		contacts_list_get_current_record_p(list, &record);
		contacts_list_destroy(list, false);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return record;
}

int WidgetItem::getChanges(contacts_record_h oldRecord, contacts_record_h newRecord)
{
	int changes = ChangeNone;
	if (!compareRecordsStr(oldRecord, newRecord, m_Metadata->nameProp)) {
		changes |= ChangeName;
	}
	if (!compareRecordsStr(oldRecord, newRecord, m_Metadata->imageProp)) {
		changes |= ChangeImage;
	}

	return changes;
}

WidgetItem & WidgetItem::operator=(WidgetItem &&that)
{
	contacts_record_destroy(m_Record, true);

	m_Id = that.m_Id;
	m_ActionType = that.m_ActionType;
	m_Metadata = that.m_Metadata;
	m_Record = that.m_Record;
	m_OnChanged = std::move(that.m_OnChanged);
	m_OnDelete = std::move(that.m_OnDelete);
	that.m_Record = nullptr;

	return *this;
}
