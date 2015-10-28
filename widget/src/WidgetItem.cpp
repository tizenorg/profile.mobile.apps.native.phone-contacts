/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

namespace
{
	const char *recordUri[] = {
		_contacts_contact_number._uri,
		_contacts_contact_number._uri,
		_contacts_contact_email._uri,
	};

	unsigned dataIdProp[] = {
		_contacts_contact_number.number_id,
		_contacts_contact_number.number_id,
		_contacts_contact_email.email_id,
	};

	unsigned contactIdProp[] = {
		_contacts_contact_number.contact_id,
		_contacts_contact_number.contact_id,
		_contacts_contact_email.contact_id,
	};

	unsigned dataProp[] = {
		_contacts_contact_number.number,
		_contacts_contact_number.number,
		_contacts_contact_email.email,
	};

	unsigned nameProp[] = {
		_contacts_contact_number.display_name,
		_contacts_contact_number.display_name,
		_contacts_contact_email.display_name,
	};

	unsigned thumbnailProp[] = {
		_contacts_contact_number.image_thumbnail_path,
		_contacts_contact_number.image_thumbnail_path,
		_contacts_contact_email.image_thumbnail_path,
	};
}

WidgetItem::WidgetItem(WidgetItemType type, int dataId)
	: m_Id(0), m_Type(type)
{
	m_Record = fetchDataRecord(dataId);
}

WidgetItem::WidgetItem(int id, WidgetItemType type, int dataId)
	: WidgetItem(type, dataId)
{
	m_Id = id;
}

WidgetItem::WidgetItem(WidgetItem &&that)
	: m_UserData(nullptr), m_Id(that.m_Id), m_Type(that.m_Type),
	  m_Record(that.m_Record)
{
	that.m_Record = nullptr;
}

WidgetItem::~WidgetItem()
{
	if (m_Record) {
		contacts_record_destroy(m_Record, true);
	}
}

bool WidgetItem::update()
{
	int id = getDataId();
	if (m_Record) {
		contacts_record_destroy(m_Record, true);
	}

	m_Record = fetchDataRecord(id);
	return m_Record != nullptr;
}

void *WidgetItem::getUserData() const
{
	return m_UserData;
}

void WidgetItem::setUserData(void *data)
{
	m_UserData = data;
}

WidgetItemType WidgetItem::getType() const
{
	return m_Type;
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
	int value = 0;
	if (m_Record) {
		contacts_record_get_int(m_Record, dataIdProp[m_Type], &value);
	}

	return value;
}

int WidgetItem::getContactId() const
{
	int value = 0;
	if (m_Record) {
		contacts_record_get_int(m_Record, contactIdProp[m_Type], &value);
	}

	return value;
}

const char *WidgetItem::getData() const
{
	char *value = nullptr;
	if (m_Record) {
		contacts_record_get_str_p(m_Record, dataProp[m_Type], &value);
	}

	return value;
}

const char *WidgetItem::getDisplayName() const
{
	char *value = nullptr;
	if (m_Record) {
		contacts_record_get_str_p(m_Record, nameProp[m_Type], &value);
	}

	return value;
}

const char *WidgetItem::getThumbnailPath() const
{
	char *value = nullptr;
	if (m_Record) {
		contacts_record_get_str_p(m_Record, thumbnailProp[m_Type], &value);
	}

	return value;
}

contacts_record_h WidgetItem::fetchDataRecord(int dataId)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(recordUri[m_Type], &filter);
	contacts_filter_add_int(filter, dataIdProp[m_Type], CONTACTS_MATCH_EQUAL, dataId);

	contacts_query_h query = nullptr;
	contacts_query_create(recordUri[m_Type], &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	contacts_record_h record = nullptr;
	contacts_db_get_records_with_query(query, 0, 1, &list);
	if (list) {
		contacts_list_get_current_record_p(list, &record);
		contacts_list_destroy(list, false);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return record;
}

WidgetItem & WidgetItem::operator=(WidgetItem &&that)
{
	if (m_Record) {
		contacts_record_destroy(m_Record, true);
	}

	m_Id = that.m_Id;
	m_Type = that.m_Type;
	m_Record = that.m_Record;
	that.m_Record = nullptr;

	return *this;
}
