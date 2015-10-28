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

#ifndef _WIDGET_ITEMS_H_
#define _WIDGET_ITEMS_H_

#include <list>
#include "WidgetDatabase.h"

/**
 * @brief Widget items containter.
 */
class WidgetItems
{
public:
	typedef std::list<WidgetItem> Container;

	WidgetItems();
	~WidgetItems();

	/**
	 * @brief Initialize items container and fetch all stored items.
	 * @details If @a widgetId is 0 then a new widget ID is generated.
	 * @param[in]   widgetId    Widget ID to get the items for.
	 */
	bool initialize(unsigned widgetId);

	/**
	 * @return ID of widget items belong to.
	 */
	unsigned getWidgetId() const;

	/**
	 * @brief Set item update callback.
	 * @param[in]   callback    Callback to be called after some item was updated.
	 */
	void setOnItemUpdate(std::function<void(WidgetItem&)> callback);

	/**
	 * @brief Set item delete callback.
	 * @param[in]   callback    Callback to be called before some item is deleted.
	 */
	void setOnItemDelete(std::function<void(WidgetItem&)> callback);

	/**
	 * @return Begin iterator of underlying container.
	 */
	Container::iterator begin();

	/**
	 * @return End iterator of underlying container.
	 */
	Container::iterator end();

	/**
	 * @return Begin iterator of underlying container.
	 */
	Container::const_iterator begin() const;

	/**
	 * @return End iterator of underlying container.
	 */
	Container::const_iterator end() const;

	/**
	 * @return Widget items count.
	 */
	size_t count() const;

	/**
	 * @brief Add widget item.
	 * @param[in]   item    Item to add
	 * @return true on success, otherwise false.
	 */
	bool addItem(WidgetItem &&item);

	/**
	 * @brief Remove widget item.
	 * @param[in]   item    Item to remove
	 * @return true on success, otherwise false.
	 */
	bool removeItem(const WidgetItem &item);

	/**
	 * @brief Destroy all data that belong to this widget.
	 * @remarks This instance should not be used after calling this method.
	 * @return true on success, otherwise false.
	 */
	bool destroy();

private:
	void loadChanges();
	static void onDbChanged(const char *uri, void *data);
	static void onNameOrderChanged(contacts_name_display_order_e name_display_order, void *data);

	WidgetDatabase m_Database;
	Container m_Items;
	int m_DbVersion;

	std::function<void(WidgetItem&)> m_OnUpdate;
	std::function<void(WidgetItem&)> m_OnDelete;
};

#endif /* _WIDGET_ITEMS_H_ */
