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

#ifndef CONTACTS_SELECT_TYPES_H
#define CONTACTS_SELECT_TYPES_H

#include "Utils/Range.h"
#include <functional>

namespace Contacts
{
	/**
	 * @brief Determines how items can be selected.
	 */
	enum SelectMode
	{
		SelectNone,     /**< Selection is disabled */
		SelectSingle,   /**< Only one item can be selected */
		SelectMulti     /**< Multiple items can be selected */
	};

	/**
	 * @brief Selection result.
	 */
	struct SelectResult
	{
		unsigned type;  /**< Result type (depends on result source) */
		union Value
		{
			Value(void *data) : data(data) { }
			Value(int id) : id(id) { }
			void *data; /**< Result data */
			int id;     /**< Result data ID */
		} value;        /**< Result value (depends on type) */
	};

	/**
	 * @brief Range of consecutive selection results.
	 */
	typedef Utils::Range<SelectResult *> SelectResults;

	/**
	 * @brief Callback to be called when selection is done.
	 * @param[in]   Selection results
	 * @return Whether the component that provided the selection should be destroyed.
	 */
	typedef std::function<bool(SelectResults)> SelectCallback;
}

#endif /* CONTACTS_SELECT_TYPES_H */
