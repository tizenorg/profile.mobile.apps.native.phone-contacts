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

#include "Contacts/Groups/Model/Group.h"

#include "Contacts/Groups/Model/Queries.h"
#include "Common/Database/RecordUtils.h"

using namespace Contacts::Groups::Model;
using namespace Common::Database;

Group::Group(contacts_record_h record)
	: m_Record(record)
{
	m_MembersCount = Model::getMembersCount(getId());
}

Group::~Group()
{
	contacts_record_destroy(m_Record, true);
}

int Group::getId() const
{
	return getRecordInt(m_Record, _contacts_group.id);
}

const char *Group::getName() const
{
	return getRecordStr(m_Record, _contacts_group.name);
}

const char *Group::getRingtone() const
{
	return getRecordStr(m_Record, _contacts_group.ringtone_path);
}

const int Group::getMembersCount() const
{
	return m_MembersCount;
}

contacts_record_h Group::getRecord() const
{
	return m_Record;
}

bool Group::operator<(const Group &that) const
{
	return getSortValue() < that.getSortValue();
}

const Utils::UniString &Group::getSortValue() const
{
	if (m_SortValue.getI18nStr().empty()) {
		m_SortValue = getName();
	}
	return m_SortValue;
}

void Group::update(contacts_record_h record)
{
	int changes = ChangedNone;
	if (!compareRecordsStr(m_Record, record, _contacts_group.name)) {
		m_SortValue.clear();
		changes |= ChangedName;
	}

	contacts_record_destroy(m_Record, true);
	m_Record = record;

	onUpdated(changes);
}

void Group::updateMembersCount()
{
	m_MembersCount = Model::getMembersCount(getId());
	onUpdated(ChangedMembersCount);
}
