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

#include "Contacts/List/MfcGroup.h"
#include "Contacts/List/PersonItem.h"
#include "Contacts/List/Model/Person.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

using namespace Contacts::List;
using namespace Contacts::List::Model;

MfcGroup::MfcGroup()
{
	m_Provider.setInsertCallback(std::bind(&MfcGroup::onInserted, this, std::placeholders::_1));

	for (auto &&contactData : m_Provider.getDataList()) {
		insertSubItem(createItem(static_cast<Person &>(*contactData)));
	}
}

void MfcGroup::setShowCallback(ShowCallback callback)
{
	m_OnShow = std::move(callback);
}

char *MfcGroup::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_HEADER_MOST_FREQUENT_CONTACTS_ABB2"));
	}

	return nullptr;
}

void MfcGroup::onInserted(Contacts::Model::ContactData &person)
{
	if (empty() && m_OnShow) {
		m_OnShow();
	}

	PersonItem *item = createItem(static_cast<Person &>(person));
	insertSubItem(item);
}

void MfcGroup::onUpdated(PersonItem *item, int changes)
{
	item->update(changes);
}

void MfcGroup::onDeleted(PersonItem *item)
{
	item->getPerson().unsetUpdateCallback();
	item->getPerson().unsetDeleteCallback();
	delete item;

	if (empty()) {
		pop();
	}
}

PersonItem *MfcGroup::createItem(Person &person)
{
	PersonItem *item = new PersonItem(person);
	person.setUpdateCallback(std::bind(&MfcGroup::onUpdated, this, item, std::placeholders::_1));
	person.setDeleteCallback(std::bind(&MfcGroup::onDeleted, this, item));
	return item;
}
