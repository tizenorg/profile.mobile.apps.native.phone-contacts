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
#include "Contacts/Groups/GroupsItem.h"
#include "Contacts/Groups/InputView.h"
#include "Contacts/Groups/Model/Group.h"

#include "Common/Strings.h"
#include "Ui/Genlist.h"
#include "Ui/Menu.h"
#include "Ui/Navigator.h"

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace std::placeholders;

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
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	m_GroupsItem = new GroupsItem("IDS_PB_OPT_GROUPS");
	m_Genlist->insert(m_GroupsItem);
	for (auto &&data : m_Provider.getDataList()) {
		insertItem(createItem(*static_cast<Group *>(data)));
	}

	updateCreateItem();
	return m_Genlist->getEvasObject();
}

void GroupsView::onMenuPressed()
{
	if (getSelectMode() != Ux::SelectNone) {
		return;
	}

	Ui::Menu *menu = new Ui::Menu();
	menu->create(getEvasObject());

	menu->addItem("IDS_PB_OPT_CREATE", [this] {
		getNavigator()->navigateTo(new InputView());
	});

	menu->show();
}

void GroupsView::onSelectAllInsert(Ui::GenlistItem *item)
{
	m_Genlist->insert(item, nullptr, nullptr, Ui::Genlist::After);
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
	}
}

void GroupsView::deleteItem(GroupItem *item)
{
	removeSelectItem(item);
	delete item;
}

Ui::GenlistItem *GroupsView::getNextItem(Group &group)
{
	for (auto &&item : *m_GroupsItem) {
		GroupItem *groupItem = static_cast<GroupItem *>(item);
		if (group < groupItem->getGroup()) {
			return groupItem;
		}
	}
	return m_CreateItem;
}
