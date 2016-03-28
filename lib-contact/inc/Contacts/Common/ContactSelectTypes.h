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

#ifndef CONTACTS_COMMON_CONTACT_SELECT_TYPES_H
#define CONTACTS_COMMON_CONTACT_SELECT_TYPES_H

#include "Contacts/SelectTypes.h"
#include "Contacts/Model/ContactTypes.h"

namespace Contacts
{
	/**
	 * @brief Determines which items are displayed.
	 */
	enum FilterType
	{
		FilterNone   = -1,                           /**< All items are displayed */
		FilterNumber = 1 << Model::ObjectTypeNumber, /**< Only items with number */
		FilterEmail  = 1 << Model::ObjectTypeEmail   /**< Only items with email */
	};

	/**
	 * @brief Determines what should be the result of selection.
	 */
	enum ResultType
	{
		ResultMyProfile = Model::ObjectTypeMyProfile, /**< My Profile ID is the result */
		ResultPerson    = Model::ObjectTypeContact,   /**< Person ID is the result */
		ResultNumber    = Model::ObjectTypeNumber,    /**< Number ID is the result */
		ResultEmail     = Model::ObjectTypeEmail,     /**< Email ID is the result */
		ResultVcard,    /**< vCard file path is the result */
		ResultAction    /**< @ref ActionType is the result*/
	};
}

#endif /* CONTACTS_COMMON_CONTACT_SELECT_TYPES_H */
