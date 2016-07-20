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

#include "Contacts/Input/ContactGroupsFieldItem.h"
#include "Contacts/Groups/GroupsView.h"
#include "Contacts/Groups/GroupItem.h"
#include "Contacts/Groups/Model/Group.h"

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Common/Strings.h"
#include "Common/Database/RecordUtils.h"

#include "Ui/Genlist.h"
#include "Ui/Navigator.h"

#include "InputItemLayout.h"

#include <algorithm>
#include <app_i18n.h>

using namespace Common::Database;
using namespace Contacts::Input;
using namespace Contacts::Model;
using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;
using namespace std::placeholders;

ContactGroupsFieldItem::ContactGroupsFieldItem(ContactObject &object)
	: ContactFieldItem(object), m_GroupRels(*object.getField<ContactArray>(0))
{
}

bool ContactGroupsFieldItem::isFocusable() const
{
	return false;
}

void ContactGroupsFieldItem::update()
{
	elm_genlist_item_fields_update(getObjectItem(), "elm.text", ELM_GENLIST_ITEM_FIELD_TEXT);
}

Elm_Genlist_Item_Class *ContactGroupsFieldItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("type2");
	return &itc;
}

char *ContactGroupsFieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		std::string groups = formatGroups();
		return strdup(!groups.empty() ? groups.c_str() : _("IDS_PB_BODY_NOT_ASSIGNED"));
	} else if (strcmp(part, "elm.text.sub") == 0) {
		const char *name = Common::getContactFieldName(ContactFieldId(getObject().getId()));
		return Utils::safeDup(_(name));
	}

	return nullptr;
}

Evas_Object *ContactGroupsFieldItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.swallow.end") == 0) {
		return ContactFieldItem::getContent(parent, PART_BUTTON);
	}

	return nullptr;
}

void ContactGroupsFieldItem::onInserted()
{
	ContactFieldItem::onInserted();
	elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_DEFAULT);
}

void ContactGroupsFieldItem::onSelected()
{
	Ui::Navigator *navigator = getParent()->findParent<Ui::Navigator>();
	if (!navigator) {
		return;
	}

	auto view = new GroupsView();
	view->setAssignMode(true);
	view->setSelectMode(Ux::SelectMulti);
	view->setSelectCallback(std::bind(&ContactGroupsFieldItem::onGroupsSelected, this, _1));
	navigator->navigateTo(view);

	for (auto &&field : m_GroupRels) {
		int groupId = getRecordInt(field.getRecord(), _contacts_group_relation.group_id);

		for (auto &&item : view->getSelectItems()) {
			GroupItem &groupItem = static_cast<GroupItem &>(*item);
			if (groupItem.getGroup().getId() == groupId) {
				groupItem.setChecked(true);
				break;
			}
		}
	}
}

bool ContactGroupsFieldItem::onGroupsSelected(Ux::SelectResults results)
{
	for (size_t i = 0; i < m_GroupRels.getFieldCount(); ) {
		auto &field = *m_GroupRels.getField(i);
		int groupId = getRecordInt(field.getRecord(), _contacts_group_relation.group_id);

		auto it = std::find_if(results.begin(), results.end(),
			[groupId](Ux::SelectResult &result) {
				Group *group = (Group *) result.value.data;
				return group->getId() == groupId;
			});

		if (it == results.end()) {
			m_GroupRels.removeField(field);
		} else {
			results.erase(it);
			++i;
		}
	}

	for (auto &&result : results) {
		Group *group = (Group *) result.value.data;
		auto &field = m_GroupRels.addField();
		contacts_record_set_int(field.getRecord(), _contacts_group_relation.group_id, group->getId());
		contacts_record_set_str(field.getRecord(), _contacts_group_relation.name, group->getName());
	}

	update();
	return true;
}

std::string ContactGroupsFieldItem::formatGroups() const
{
	std::string groups;
	for (auto &&field : m_GroupRels) {
		const char *name = getRecordStr(field.getRecord(), _contacts_group_relation.name);
		if (!groups.empty()) {
			groups.append(", ");
		}
		groups.append(name);
	}

	return groups;
}
