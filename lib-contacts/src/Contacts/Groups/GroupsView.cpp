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
#include "Contacts/Groups/GroupItem.h"
#include "Contacts/Groups/Model/Group.h"
#include "Ui/Genlist.h"

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace std::placeholders;

GroupsView::GroupsView()
	: m_Genlist(nullptr)
{
	m_Provider.setInsertCallback(std::bind(&GroupsView::onInserted, this, _1));
}

Evas_Object *GroupsView::onCreate(Evas_Object *parent)
{
	m_Genlist = new Ui::Genlist();
	m_Genlist->create(parent);

	for (auto &&data : m_Provider.getDataList()) {
		Group *group = static_cast<Group *>(data);
		insertItem(createItem(*group));
	}

	return m_Genlist->getEvasObject();
}

void GroupsView::onPageAttached(Ui::NavigatorPage *page)
{
	page->setTitle("Groups");
}

void GroupsView::onInserted(::Model::DataItem &data)
{
	insertItem(createItem(static_cast<Group &>(data)));
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
	GroupItem *nextItem = getNextItem(item->getGroup());
	m_Genlist->insert(item, nullptr, nextItem);
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
	delete item;
}

GroupItem *GroupsView::getNextItem(Group &group)
{
	for (auto &&item : *m_Genlist) {
		GroupItem *groupItem = static_cast<GroupItem *>(item);
		if (group < groupItem->getGroup()) {
			return groupItem;
		}
	}
	return nullptr;
}
