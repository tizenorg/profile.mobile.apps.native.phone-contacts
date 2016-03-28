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

#include "Contacts/List/Model/MyProfile.h"
#include "Contacts/Utils.h"

using namespace Contacts::List::Model;

MyProfile::MyProfile()
	: m_MyProfileRecord(nullptr)
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_my_profile._uri, 0, 1, &list);
	contacts_list_get_current_record_p(list, &m_MyProfileRecord);
	contacts_list_destroy(list, false);
}

MyProfile::~MyProfile()
{
	contacts_record_destroy(m_MyProfileRecord, true);
}

const int MyProfile::getId() const
{
	return getRecordInt(m_MyProfileRecord, _contacts_my_profile.id);
}

const char *MyProfile::getName() const
{
	return getRecordStr(m_MyProfileRecord, _contacts_my_profile.display_name);
}

const char *MyProfile::getImagePath() const
{
	return getRecordStr(m_MyProfileRecord, _contacts_my_profile.image_thumbnail_path);
}

const contacts_record_h MyProfile::getRecord() const
{
	return m_MyProfileRecord;
}
