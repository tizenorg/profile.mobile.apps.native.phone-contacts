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

#include "Contacts/Details/GroupsFieldItem.h"
#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactObject.h"
#include "Common/Database/RecordUtils.h"

#include <app_i18n.h>

using namespace Common::Database;
using namespace Contacts::Details;
using namespace Contacts::Model;

char *GroupsFieldItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		std::string groups = formatGroups();
		return strdup(!groups.empty() ? groups.c_str() : _("IDS_PB_BODY_NOT_ASSIGNED"));
	}

	return FieldItem::getText(parent, part);
}

std::string GroupsFieldItem::formatGroups() const
{
	std::string groups;
	for (auto &&field : *getObject().getField<ContactArray>(0)) {
		const char *name = getRecordStr(field.getRecord(), _contacts_group_relation.name);
		if (!groups.empty()) {
			groups.append(", ");
		}
		groups.append(name);
	}

	return groups;
}
