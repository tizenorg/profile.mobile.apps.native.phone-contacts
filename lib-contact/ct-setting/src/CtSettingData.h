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

#ifndef _CT_SETTING_DATA_H_
#define _CT_SETTING_DATA_H_
#include <string>
#include <vector>
#include <list>

/**
 * @brief Manages contacts settings data
 */
class CtSettingData
{
public:
	CtSettingData();
	~CtSettingData();

	/**
	 * @brief Initializes all data
	 */
	void init();

	/**
	 * @brief Indicates how to sort contacts in contacts list view
	 */
	enum SortMode
	{
		SORT_MODE_FIRST_NAME,         /**< Sort by first name */
		SORT_MODE_LAST_NAME,          /**< Sort by last name */
	};

	/**
	 * @brief Indicates what order to follow in contacts list view
	 */
	enum NameOrder
	{
		NAME_ORDER_FIRST_NAME_FIRST,  /**< First name then last name */
		NAME_ORDER_LAST_NAME_FIRST,   /**< Last name then first name */
	};

	/**
	 * @return Current contacts' sort mode
	 *
	 * @see SortMode
	 */
	SortMode getSortMode() const;

	/**
	 * @brief Specify contacts' sort mode
	 *
	 * @see SortMode
	 */
	void setSortMode(SortMode sortMode);

	/**
	 * @return Current name order
	 *
	 * @see NameOrder
	 */
	NameOrder getNameOrder() const;

	/**
	 * @brief Specify name order
	 *
	 * @see NameOrder
	 */
	void setNameOrder(NameOrder order);

private:
	SortMode __sortMode;
	NameOrder __nameOrder;
};

#endif //_CT_SETTING_DATA_H_
