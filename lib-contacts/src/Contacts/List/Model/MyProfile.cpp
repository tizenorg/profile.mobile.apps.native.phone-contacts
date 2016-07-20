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

#include "Contacts/List/Model/MyProfile.h"
#include "Common/Database/ChildRecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Logger.h"

#include <string>
#include <telephony.h>

using namespace Common::Database;
using namespace Contacts::List::Model;

MyProfile::MyProfile()
	: ContactData(TypeContact), m_Record(fetchRecord())
{
	contacts_db_add_changed_cb(_contacts_my_profile._uri, &MyProfile::onChanged, this);
	contacts_setting_add_name_display_order_changed_cb(&MyProfile::onNameFormatChanged, this);
}

MyProfile::~MyProfile()
{
	contacts_setting_remove_name_display_order_changed_cb(&MyProfile::onNameFormatChanged, this);
	contacts_db_remove_changed_cb(_contacts_my_profile._uri, &MyProfile::onChanged, this);
	contacts_record_destroy(m_Record, true);
}

int MyProfile::getId() const
{
	return getRecordInt(m_Record, _contacts_my_profile.id);
}

const char *MyProfile::getName() const
{
	return getRecordStr(m_Record, _contacts_my_profile.display_name);
}

const char *MyProfile::getNumber() const
{
	for (auto &&record : makeRange(m_Record, _contacts_my_profile.number)) {
		if (getRecordBool(record, _contacts_number.is_default)) {
			return getRecordStr(record, _contacts_number.number);
		}
	}

	return fetchNumber();
}

const char *MyProfile::getImagePath() const
{
	return getRecordStr(m_Record, _contacts_my_profile.image_thumbnail_path);
}

void MyProfile::update()
{
	contacts_record_h record = fetchRecord();

	int changes = ChangedNone;
	if (!compareRecordsStr(record, m_Record, _contacts_my_profile.display_name)) {
		changes |= ChangedName;
	}
	if (!compareRecordsStr(record, m_Record, _contacts_my_profile.image_thumbnail_path)) {
		changes |= ChangedImage;
	}

	contacts_record_destroy(m_Record, true);
	m_Record = record;
	onUpdated(changes);
}

const char *MyProfile::fetchNumber()
{
	static bool isInitialized = false;
	static std::string number;

	if (!isInitialized) {
		isInitialized = true;

		telephony_handle_list_s handles;
		int err = telephony_init(&handles);
		RETVM_IF_ERR(err, nullptr, "telephony_init() failed.");

		for (size_t i = 0; i < handles.count; ++i) {
			char *simNumber = nullptr;
			err = telephony_sim_get_subscriber_number(handles.handle[i], &simNumber);
			WARN_IF_ERR(err, "telephony_sim_get_subscriber_number() failed.");

			if (simNumber && *simNumber) {
				number = simNumber;
				free(simNumber);
				break;
			}
			free(simNumber);
		}
		telephony_deinit(&handles);
	}

	return !number.empty() ? number.c_str() : nullptr;
}

contacts_record_h MyProfile::fetchRecord()
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_my_profile._uri, 0, 1, &list);

	contacts_record_h record = nullptr;
	contacts_list_get_current_record_p(list, &record);
	contacts_list_destroy(list, false);

	return record;
}

void MyProfile::onChanged(const char *uri, void *data)
{
	MyProfile *myProfile = (MyProfile *) data;
	myProfile->update();
}

void MyProfile::onNameFormatChanged(contacts_name_display_order_e order, void *data)
{
	MyProfile *myProfile = (MyProfile *) data;
	myProfile->update();
}
