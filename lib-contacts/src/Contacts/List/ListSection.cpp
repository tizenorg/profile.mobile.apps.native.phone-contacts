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

#include "Contacts/List/ListSection.h"
#include "Contacts/List/PersonItem.h"
#include "Contacts/List/ReorderItem.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/List/Model/PersonProvider.h"

using namespace Contacts::List;
using namespace Contacts::List::Model;

ListSection::ListSection(const char *title, PersonProvider *provider, SectionMode mode)
	: GenGroupItem(title), m_Provider(provider), m_Mode(mode)
{
	m_Provider->setInsertCallback(std::bind(&ListSection::onPersonInserted, this, std::placeholders::_1));

	for (auto &&contactData : m_Provider->getDataList()) {
		insertSubItem(createItem(static_cast<Person &>(*contactData)));
	}
}

ListSection::~ListSection()
{
	delete m_Provider;
}

void ListSection::setUpdateCallback(UpdateCallback callback)
{
	m_OnUpdated = std::move(callback);
}

void ListSection::update()
{
	m_Provider->reload();
}

void ListSection::reorderItem(int reorderedId, int previousId)
{
	ContactItem *reorderedItem = nullptr;
	ContactItem *previousItem = nullptr;

	for (auto &&item : *this) {
		ContactItem *contactItem = static_cast<ContactItem *>(item);
		int id = contactItem->getContactData().getId();
		if (id == reorderedId) {
			reorderedItem = contactItem;
			if (previousItem) {
				break;
			}
		} else if (id == previousId) {
			previousItem = contactItem;
			if (reorderedItem) {
				break;
			}
		}
	}
	if (reorderedItem) {
		reorderedItem->pop();
		insertSubItem(reorderedItem, previousItem, Ui::GenContainer::After);
	}
}

void ListSection::onPersonInserted(::Model::DataItem &person)
{
	ContactItem *item = createItem(static_cast<Person &>(person));
	insertSubItem(item);

	if (m_OnUpdated) {
		m_OnUpdated(item, Common::ChangeInsert);
	}
}

void ListSection::onPersonDeleted(ContactItem *item)
{
	item->pop();
	if (m_OnUpdated) {
		m_OnUpdated(item, Common::ChangeDelete);
	}

	delete item;
}

ContactItem *ListSection::createItem(Person &person)
{
	ContactItem *item = nullptr;
	if (m_Mode == ReorderMode) {
		item = new ReorderItem(person);
	} else {
		item = new PersonItem(person);
	}
	person.setUpdateCallback(std::bind(&ContactItem::update, item, std::placeholders::_1));
	person.setDeleteCallback(std::bind(&ListSection::onPersonDeleted, this, item));
	return item;
}
