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

#include "Contacts/Model/ContactNumberData.h"
#include "Common/Database/RecordUtils.h"

using namespace Common::Database;
using namespace Contacts::Model;
using namespace Utils;

ContactNumberData::ContactNumberData(ContactData &contact, contacts_record_h numberRecord)
	: ContactData(TypeContactNumber),
	  m_ContactData(contact), m_NumberRecord(numberRecord)
{
}

ContactNumberData::~ContactNumberData()
{
	contacts_record_destroy(m_NumberRecord, true);
}

int ContactNumberData::getId() const
{
	return m_ContactData.getId();
}

const char *ContactNumberData::getName() const
{
	return m_ContactData.getName();
}

const char *ContactNumberData::getNumber() const
{
	return getRecordStr(m_NumberRecord, _contacts_number.number);
}

const char *ContactNumberData::getImagePath() const
{
	return m_ContactData.getImagePath();
}
