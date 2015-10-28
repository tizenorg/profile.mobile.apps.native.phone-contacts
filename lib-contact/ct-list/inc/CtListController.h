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

#ifndef _CT_LIST_CONTROLLER_H_
#define _CT_LIST_CONTROLLER_H_

#include <Elementary.h>

class CtListView;

/**
 * @brief Contacts list controller.
 */
class CtListController
{
public:
	virtual ~CtListController(){};

	/**
	 * @brief Called when list's item is selected.
	 *
	 * @param[in]   selectedItem    The selected item
	 * @param[in]   view            List view
	 * @param[in]   checkState      The item's checked state
	 */
	virtual void onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState) = 0;

	/**
	 * @brief Called when Create button is selected.
	 */
	virtual void onSelectCreate(){};

	/**
	 * @brief Called when Done button is selected.
	 */
	virtual void onSelectDone() = 0;

	/**
	 * @brief Called when Back button is selected.
	 */
	virtual void onSelectBack() = 0;

	/**
	 * @brief Called when Cancel button is selected.
	 */
	virtual void onSelectCancel() = 0;

	/**
	 * @brief Called when "Check all" item is selected.
	 *
	 * @param[in]   selectedItem    The selected item
	 * @param[in]   view            List view
	 * @param[in]   checkState      The item's checked state
	 */
	virtual void onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState) = 0;

	/**
	 * @brief Called when Menu(More) button is selected.
	 */
	virtual void onSelectMenu() = 0;
};

#endif // _CT_LIST_CONTROLLER_H_

