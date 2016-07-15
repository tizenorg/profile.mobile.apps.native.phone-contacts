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

#include "Contacts/Groups/GroupItem.h"

#include "Contacts/Groups/Model/Queries.h"
#include "Contacts/Groups/Model/MembersProvider.h"
#include "Contacts/List/ListView.h"

#include "Common/Strings.h"
#include "Ui/Navigator.h"
#include "Utils/Logger.h"

#define BUFFER_SIZE 1024

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace Contacts::List;

GroupItem::GroupItem(Group &group)
	: m_Group(group)
{
}

Group &GroupItem::getGroup() const
{
	return m_Group;
}

void GroupItem::update(int changes)
{
	if (changes & Group::ChangedName) {
		elm_genlist_item_fields_update(getObjectItem(),
				PART_GROUP_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

char *GroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_NAME) == 0) {
		char title[BUFFER_SIZE] = { 0, };
		snprintf(title, sizeof(title), "%s (%d)", m_Group.getName(), m_Group.getMembersCount());
		return strdup(title);
	}
	return nullptr;
}

Evas_Object *GroupItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_CHECK) == 0) {
		return SelectItem::getContent(parent, part);
	}

	return nullptr;
}

Ux::SelectResult GroupItem::getDefaultResult() const
{
	return { 0, &m_Group };
}

void GroupItem::onSelected()
{
	if (getSelectMode() != Ux::SelectNone) {
		SelectItem::onSelected();
		return;
	}
	Ui::Navigator *navigator = getParent()->findParent<Ui::Navigator>();
	if (!navigator) {
		return;
	}

	MembersProvider *provider = new MembersProvider(m_Group.getId());
	ListView *view = new ListView(provider);
	provider->setUpdateFinishedCallback(std::bind(&GroupItem::changeListTitle, this, view));

	changeListTitle(view);
	view->enableAddButton(false);
	view->setNoContentHelpText("IDS_PB_BODY_AFTER_YOU_ADD_CONTACTS_THEY_WILL_BE_SHOWN_HERE");
	for (size_t i = ListView::SectionFirst; i < ListView::SectionMax; ++i) {
		view->setSectionVisibility(static_cast<ListView::SectionId>(i), false);
	}

	navigator->navigateTo(view);
}

void GroupItem::changeListTitle(ListView *view)
{
	RETM_IF(!view, "view is nullptr");
	auto strings = Common::getSelectViewStrings();

	char title[BUFFER_SIZE] = { 0, };
	int count = getDbMembersCount(m_Group.getId());
	if (count) {
		snprintf(title, sizeof(title), "%s (%d)", m_Group.getName(), count);
	} else {
		snprintf(title, sizeof(title), "%s", m_Group.getName());
	}

	strings.titleDefault = strdup(title);
	view->setStrings(strings);
}
