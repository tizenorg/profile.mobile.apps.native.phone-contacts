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

#include "Contacts/Groups/GroupsView.h"

#include "Contacts/Groups/CreateGroupItem.h"
#include "Contacts/Groups/GroupItem.h"
#include "Contacts/Groups/InputView.h"
#include "Contacts/Groups/Model/Group.h"

#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Common/Strings.h"

#include "Ui/Genlist.h"
#include "Ui/GenGroupItem.h"
#include "Ui/ListPopup.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"
#include "Ui/ProcessPopup.h"
#include "Utils/Logger.h"
#include "Utils/Thread.h"

using namespace Common::Database;
using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace std::placeholders;
using namespace Ui;
using namespace Utils;
using namespace Ux;

GroupsView::GroupsView()
	: m_Genlist(nullptr), m_CreateItem(nullptr), m_GroupsItem(nullptr),
	  m_IsAssignMode(false), m_NewGroupId(0)
{
	auto strings = Common::getSelectViewStrings();
	strings.titleDefault = "IDS_PB_HEADER_GROUPS_ABB";
	strings.titleMulti = "IDS_PB_HEADER_SELECT_GROUPS_ABB";
	setStrings(strings);

	m_Provider.setInsertCallback(std::bind(&GroupsView::onInserted, this, _1));
}

void GroupsView::setAssignMode(bool isEnabled)
{
	m_IsAssignMode = isEnabled;
	updateCreateItem();
}

Evas_Object *GroupsView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Genlist();
	m_Genlist->create(parent);

	m_GroupsItem = new GenGroupItem("IDS_PB_OPT_GROUPS");
	m_Genlist->insert(m_GroupsItem);
	for (auto &&data : m_Provider.getDataList()) {
		GroupItem *groupItem = createItem(*static_cast<Group *>(data));
		const char *name = groupItem->getGroup().getName();
		if (safeCmp(name, "Co-workers") || safeCmp(name, "Family")
				|| safeCmp(name, "Friends")) {
			m_MainItems.push_back(groupItem);
		}
		insertItem(groupItem);
	}

	updateCreateItem();
	return m_Genlist->getEvasObject();
}

void GroupsView::onMenuPressed()
{
	if (getSelectMode() != SelectNone) {
		return;
	}

	Menu *menu = new Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_CREATE", [this] {
		getNavigator()->navigateTo(new InputView());
	});

	menu->addItem("IDS_LOGS_OPT_DELETE", [this] {
		setSelectMode(SelectMulti);

		setCancelCallback(std::bind(&GroupsView::onDeleteFinished, this));
		setSelectCallback([this](SelectResults results) {
			ListPopup *popup = new ListPopup();
			popup->create(getEvasObject());
			popup->setTitle("IDS_PB_HEADER_DELETE_GROUPS_ABB");

			popup->addItem("IDS_PB_OPT_GROUPS_ONLY_ABB", (void *)false);
			popup->addItem("IDS_PB_OPT_GROUPS_AND_MEMBERS_ABB", (void *)true);
			popup->setSelectedCallback(std::bind(&GroupsView::onDeleteSelected,
					this, std::move(results), _1));

			return false;
		});
	});

	menu->show();
}

void GroupsView::onSelectAllInsert(GenItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Genlist::After);
}

void GroupsView::onSelectModeChanged(SelectMode selectMode)
{
	if (getSelectMode() == SelectMulti) {
		for (auto &&groupItem : m_MainItems) {
			groupItem->pop();
			groupItem->setExcluded(true);
		}
	} else {
		for (auto &&groupItem : m_MainItems) {
			groupItem->setExcluded(false);
			insertItem(groupItem);
		}
	}
}

void GroupsView::updateCreateItem()
{
	if (!m_Genlist) {
		return;
	}

	if (m_IsAssignMode) {
		if (!m_CreateItem) {
			m_CreateItem = new CreateGroupItem();
			m_CreateItem->setResultCallback([this](int groupId) {
				m_NewGroupId = groupId;
			});
			m_Genlist->insert(m_CreateItem);
			elm_genlist_item_select_mode_set(m_CreateItem->getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
		}
	} else {
		if (m_CreateItem) {
			delete m_CreateItem;
			m_CreateItem = nullptr;
		}
	}
}

void GroupsView::onInserted(::Model::DataItem &data)
{
	Group &group = static_cast<Group &>(data);
	GroupItem *item = createItem(group);
	insertItem(item);

	if (m_NewGroupId == group.getId()) {
		m_NewGroupId = 0;
		item->setChecked(true);
	}
}

GroupItem *GroupsView::createItem(Group &group)
{
	GroupItem *item = new GroupItem(group);
	group.setUpdateCallback(std::bind(&GroupsView::updateItem, this, item, _1));
	group.setDeleteCallback(std::bind(&GroupsView::deleteItem, this, item));
	return item;
}

void GroupsView::insertItem(GroupItem *item)
{
	m_GroupsItem->insertSubItem(item, getNextItem(item->getGroup()));
	addSelectItem(item);
}

void GroupsView::updateItem(GroupItem *item, int changes)
{
	if (changes & Group::ChangedName) {
		item->pop();
		insertItem(item);
	} else {
		item->update(changes);
	}
}

void GroupsView::deleteItem(GroupItem *item)
{
	removeSelectItem(item);
	delete item;
}

GenItem *GroupsView::getNextItem(Group &group)
{
	for (auto &&item : *m_GroupsItem) {
		GroupItem *groupItem = static_cast<GroupItem *>(item);
		if (group < groupItem->getGroup()) {
			return groupItem;
		}
	}
	return m_CreateItem;
}

bool GroupsView::onDeleteFinished()
{
	setSelectMode(SelectNone);
	getPage()->setTitleVisibility(true);
	setCancelCallback(nullptr);
	setSelectCallback(nullptr);
	return false;
}

void GroupsView::onDeleteSelected(SelectResults results, void *data)
{
	bool shouldDeleteMembers = (bool)data;
	auto popup = ProcessPopup::create(getEvasObject(), "IDS_PB_TPOP_PROCESSING_ING");
	new Thread(std::bind(&GroupsView::deleteGroups, this, std::move(results), shouldDeleteMembers),
			[this, popup] {
				popup->close();
				onDeleteFinished();
			});
}

void GroupsView::deleteGroups(SelectResults results, bool shouldDeleteMembers)
{
	contacts_connect_on_thread();

	std::vector<int> groupIds;
	groupIds.reserve(results.size());
	for (auto &&result : results) {
		groupIds.push_back(static_cast<Group *>(result.value.data)->getId());
	}

	if (shouldDeleteMembers) {
		deleteMembers(groupIds);
	}
	int err = contacts_db_delete_records(_contacts_group._uri, groupIds.data(), groupIds.size());
	WARN_IF_ERR(err, "contacts_db_delete_records() failed.");

	contacts_disconnect_on_thread();
}

void GroupsView::deleteMembers(const std::vector<int> &groupIds)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_group_relation._uri, &filter);

	for (auto &&id : groupIds) {
		contacts_filter_add_int(filter, _contacts_group_relation.group_id,
				CONTACTS_MATCH_EQUAL, id);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_OR);
	}

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_group_relation._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 0, &list);
	WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");

	std::vector<int> contactIds;
	int count = 0;
	contacts_list_get_count(list, &count);
	contactIds.reserve(count);
	for (auto &&record : makeRange(list)) {
		contactIds.push_back(getRecordInt(record, _contacts_group_relation.contact_id));
	}
	err = contacts_db_delete_records(_contacts_contact._uri, contactIds.data(), contactIds.size());
	WARN_IF_ERR(err, "contacts_db_delete_records() failed.");

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);
	contacts_list_destroy(list, true);
}
