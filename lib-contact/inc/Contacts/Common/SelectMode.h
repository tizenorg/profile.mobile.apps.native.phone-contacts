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

#ifndef CONTACTS_COMMON_SELECT_MODE_H
#define CONTACTS_COMMON_SELECT_MODE_H

#include "Contacts/Model/ContactField.h"
#include "Contacts/Model/ContactTypes.h"
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
	 * @brief Determines which items are displayed.
	 */
	enum FilterType
	{
		FilterNone = -1,                        /**< All items are displayed */
		FilterNumber = 1 << Model::FieldNumber, /**< Only items with number */
		FilterEmail = 1 << Model::FieldEmail    /**< Only items with email */
	};

	/**
	 * @brief Determines what should be the result of selection.
	 */
	enum ResultType
	{
		ResultNone,     /**< Selection is disabled */
		ResultItem,     /**< Item itself is the result */
		ResultAction    /**< Action associated with the item is the result */
	};

	/**
	 * @brief Type of the selected item for #ResultItem result type.
	 */
	enum ItemType
	{
		ItemPerson = Model::ObjectTypeContact,  /**< Person ID is the result */
		ItemNumber = Model::ObjectTypeNumber,   /**< Number ID is the result */
		ItemEmail = Model::ObjectTypeEmail      /**< Email ID is the result */
	};

	/**
	 * @brief Action associated with selected item for #ResultAction result type.
	 */
	enum ActionType
	{
		ActionCall,     /**< Number ID for telephony call is the result */
		ActionMessage,  /**< Number ID for message is the result */
		ActionEmail     /**< Email ID for recipient is the result */
	};

	/**
	 * @brief Selection result.
	 */
	struct SelectResult
	{
		unsigned type;  /**< @ref ItemType or @ref ActionType depending on @ref ResultType. */
		int itemId;     /**< Item database ID (for _contacts_person, _contacts_number or _contacts_email). */
	};

	/**
	 * @brief Range of consecutive selection results.
	 */
	typedef Utils::Range<const SelectResult *> SelectResults;

	/**
	 * @brief Callback to be called when selection is done.
	 * @param[in]   Selection results
	 * @return Whether the component that provided the selection should be destroyed.
	 */
	typedef std::function<bool(SelectResults)> SelectCallback;
}

#endif /* CONTACTS_COMMON_SELECT_MODE_H */
