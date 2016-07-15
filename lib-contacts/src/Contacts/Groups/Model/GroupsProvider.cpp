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

#include "Contacts/Groups/Model/GroupsProvider.h"
#include "Contacts/Groups/Model/Group.h"
#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Callback.h"

#include <algorithm>

using namespace Contacts::Groups::Model;
using namespace Common::Database;
using namespace Model;

GroupsProvider::GroupsProvider()
	: m_DbVersion(0), m_MembersDbVersion(0), m_IsFilled(false)
{
}

GroupsProvider::~GroupsProvider()
{
	clearDataList();
}

const DataProvider::DataList &GroupsProvider::getDataList()
{
	if (!m_IsFilled) {
		contacts_db_get_current_version(&m_DbVersion);
		m_MembersDbVersion = m_DbVersion;
		contacts_db_add_changed_cb(_contacts_group._uri,
				makeCallbackWithLastParam(&GroupsProvider::onChanged), this);
		contacts_db_add_changed_cb(_contacts_group_relation._uri,
				makeCallbackWithLastParam(&GroupsProvider::onMembersChanged), this);

		contacts_list_h list = nullptr;
		contacts_db_get_all_records(_contacts_group._uri, 0, 0, &list);
		for (auto &&record : makeRange(list)) {
			m_List.push_back(createGroup(record));
		}
		contacts_list_destroy(list, false);
		m_IsFilled = true;
	}
	return m_List;
}

void GroupsProvider::clearDataList()
{
	contacts_db_remove_changed_cb(_contacts_group._uri,
			makeCallbackWithLastParam(&GroupsProvider::onChanged), this);
	contacts_db_remove_changed_cb(_contacts_group_relation._uri,
			makeCallbackWithLastParam(&GroupsProvider::onMembersChanged), this);

	for (auto &&groupData : m_List) {
		delete groupData;
	}
	m_List.clear();
	m_IsFilled = false;
}

void GroupsProvider::onChanged(const char *uri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_group_updated_info._uri, 0,
			m_DbVersion, &changes, &m_DbVersion);

	for (auto &&record : makeRange(changes)) {
		int changeType = getRecordInt(record, _contacts_group_updated_info.type);
		int groupId = getRecordInt(record, _contacts_group_updated_info.group_id);

		switch (changeType) {
			case CONTACTS_CHANGE_INSERTED:
			{
				insertGroup(groupId);
				break;
			}
			case CONTACTS_CHANGE_UPDATED:
			{
				auto groupIt = findGroup(groupId);
				if (groupIt != m_List.end()) {
					updateGroup(groupIt);
				}
				break;
			}
			case CONTACTS_CHANGE_DELETED:
			{
				auto groupIt = findGroup(groupId);
				if (groupIt != m_List.end()) {
					deleteGroup(groupIt);
				}
				break;
			}
		}
	}
}

void GroupsProvider::onMembersChanged(const char *uri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_grouprel_updated_info._uri, 0,
			m_MembersDbVersion, &changes, &m_MembersDbVersion);

	int count = 0;
	contacts_list_get_count(changes, &count);

	for (auto &&record : makeRange(changes)) {
		auto groupIt = findGroup(getRecordInt(record, _contacts_grouprel_updated_info.group_id));
		if (groupIt != m_List.end()) {
			Group *group = static_cast<Group *>(*groupIt);
			group->updateMembersCount();
		}
	}
}

void GroupsProvider::insertGroup(int id)
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_group._uri, id, &record);
	if (record) {
		m_List.push_back(createGroup(record));
		onInserted(*m_List.back());
	}
}

void GroupsProvider::updateGroup(DataList::const_iterator groupIt)
{
	Group *group = static_cast<Group *>(*groupIt);
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_group._uri, group->getId(), &record);
	group->update(record);
}

void GroupsProvider::deleteGroup(DataList::const_iterator groupIt)
{
	Group *group = static_cast<Group *>(*groupIt);
	m_List.erase(groupIt);
	group->onDeleted();
	delete group;
}

Group *GroupsProvider::createGroup(contacts_record_h record)
{
	return new Group(record);
}

DataProvider::DataList::const_iterator GroupsProvider::findGroup(int id)
{
	return std::find_if(m_List.begin(), m_List.end(),
		[id](DataItem *dataItem) {
			Group *group = static_cast<Group *>(dataItem);
			return group->getId() == id;
		}
	);
}
