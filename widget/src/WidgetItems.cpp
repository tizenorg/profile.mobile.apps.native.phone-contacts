/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "WidgetItems.h"
#include "ContactsUtils.h"

WidgetItems::WidgetItems()
	: m_DbVersion(0)
{
}

WidgetItems::~WidgetItems()
{
	contacts_setting_remove_name_display_order_changed_cb(&WidgetItems::onNameOrderChanged, this);
	contacts_db_remove_changed_cb(_contacts_contact._uri, &WidgetItems::onDbChanged, this);
}

bool WidgetItems::initialize(unsigned widgetId)
{
	if (!m_Database.initialize(widgetId)) {
		return false;
	}

	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_contact._uri, &WidgetItems::onDbChanged, this);
	contacts_setting_add_name_display_order_changed_cb(&WidgetItems::onNameOrderChanged, this);

	m_Database.fetch([this](WidgetItem &&item) {
		m_Items.push_back(std::move(item));
	});

	return true;
}

unsigned WidgetItems::getWidgetId() const
{
	return m_Database.getWidgetId();
}

void WidgetItems::setOnItemUpdate(std::function<void(WidgetItem&)> callback)
{
	m_OnUpdate = std::move(callback);
}

void WidgetItems::setOnItemDelete(std::function<void(WidgetItem&)> callback)
{
	m_OnDelete = std::move(callback);
}

WidgetItems::Container::iterator WidgetItems::begin()
{
	return m_Items.begin();
}

WidgetItems::Container::iterator WidgetItems::end()
{
	return m_Items.end();
}

WidgetItems::Container::const_iterator WidgetItems::begin() const
{
	return m_Items.begin();
}

WidgetItems::Container::const_iterator WidgetItems::end() const
{
	return m_Items.end();
}

size_t WidgetItems::count() const
{
	return m_Items.size();
}

bool WidgetItems::addItem(WidgetItem &&item)
{
	int id = m_Database.insert(item);
	if (id != 0) {
		item.setId(id);
		m_Items.push_back(std::move(item));
		return true;
	}

	return false;
}

bool WidgetItems::removeItem(const WidgetItem &item)
{
	if (!m_Database.remove(item)) {
		return false;
	}

	m_Items.remove_if([&item](const WidgetItem &item2) {
		return item2.getId() == item.getId();
	});

	return true;
}

bool WidgetItems::destroy()
{
	return m_Database.destroy();
}

void WidgetItems::loadChanges()
{
	contacts_list_h list = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &list, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		int updateType = 0;
		contacts_record_get_int(record, _contacts_contact_updated_info.type, &updateType);
		if (updateType == CONTACTS_CHANGE_INSERTED) {
			continue;
		}

		int id = 0;
		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &id);

		for (auto it = m_Items.begin(); it != m_Items.end(); ) {
			if (it->getContactId() == id) {
				switch (updateType) {
					case CONTACTS_CHANGE_UPDATED:
						if (it->update()) {
							if (m_OnUpdate) {
								m_OnUpdate(*it);
							}
							break;
						}
					//fall through to delete item if update failed
					case CONTACTS_CHANGE_DELETED:
						if (m_OnDelete) {
							m_OnDelete(*it);
						}

						m_Database.remove(*it);
						it = m_Items.erase(it);
						continue;
					default:
						break;
				}
			}

			++it;
		}
	}

	contacts_list_destroy(list, true);
}

void WidgetItems::onDbChanged(const char *uri, void *data)
{
	WidgetItems *items = (WidgetItems*) data;
	items->loadChanges();
}

void WidgetItems::onNameOrderChanged(contacts_name_display_order_e name_display_order, void *data)
{
	WidgetItems *items = (WidgetItems*) data;
	for (auto &&item : items->m_Items) {
		item.update();
		if (items->m_OnUpdate) {
			items->m_OnUpdate(item);
		}
	}
}
