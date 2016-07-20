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

#ifndef WIDGET_ITEMS_H
#define WIDGET_ITEMS_H

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
	 * @return Begin iterator of underlying container.
	 */
	Container::iterator begin();

	/**
	 * @return End iterator of underlying container.
	 */
	Container::iterator end();

	/**
	 * @return Widget items count.
	 */
	size_t count() const;

	/**
	 * @brief Add widget item.
	 * @param[in]   item    Item to add
	 * @return true on success, otherwise false.
	 */
	bool addItem(WidgetItem item);

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
	void onDbChanged(const char *uri);
	static void onNameOrderChanged(contacts_name_display_order_e order, void *data);

	WidgetDatabase m_Database;
	Container m_Items;
	int m_DbVersion;
};

#endif /* WIDGET_ITEMS_H */
