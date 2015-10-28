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

#ifndef _WIDGET_DATABASE_H_
#define _WIDGET_DATABASE_H_

#include <functional>
#include "WidgetItem.h"

struct sqlite3;

/**
 * @brief Widget database management class.
 */
class WidgetDatabase
{
public:
	WidgetDatabase();
	WidgetDatabase(const WidgetDatabase&) = delete;
	~WidgetDatabase();

	/**
	 * @brief Initialize widget database connection.
	 * @details If @a widgetId is 0 then a new widget ID is generated.
	 * @param[in]   widgetId    Widget ID to use in database queries.
	 * @return true if database is successfully opened, otherwise false
	 */
	bool initialize(unsigned widgetId);

	/**
	 * @return Widget ID that is used in queries.
	 */
	unsigned getWidgetId() const;

	/**
	 * @brief Fetch all items from database by passing each item to the provided callback.
	 * @param[in]   callback    Callback to receive each fetched item
	 */
	void fetch(const std::function<void(WidgetItem&&)> &callback);

	/**
	 * @brief Insert widget item into database.
	 * @param[in]   item    Widget item
	 * @return Widget item ID on success, otherwise 0.
	 */
	int  insert(const WidgetItem &item);

	/**
	 * @brief Remove widget item from database.
	 * @param[in]   item    Widget item
	 * @return true on success, otherwise false
	 */
	bool remove(const WidgetItem &item);

	/**
	 * @brief Remove all data that belongs to this widget from the database.
	 * @remarks This instance should not be used after calling this method.
	 * @return true on success, otherwise false
	 */
	bool destroy();

	WidgetDatabase & operator=(const WidgetDatabase&) = delete;

private:
	unsigned generateWidgetId();
	bool execute(const char *stmt);

	sqlite3 *m_Handle;
	unsigned m_WidgetId;
};

#endif /* _WIDGET_DATABASE_H_ */
