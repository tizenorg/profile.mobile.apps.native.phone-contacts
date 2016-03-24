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

#ifndef CONTACTS_COMMON_STRINGS_H
#define CONTACTS_COMMON_STRINGS_H

#include "Contacts/Model/ContactFields.h"
#include "Contacts/Model/ContactTypes.h"
#include "Utils/Range.h"

namespace Contacts
{
	namespace Common
	{
		/**
		 * @brief Get contact field name.
		 * @param[in]   fieldId     Contact field ID
		 * @return Contact field translatable name.
		 */
		const char *getContactFieldName(Model::ContactFieldId fieldId);

		/**
		 * @brief Get name of a field which is not an object or array.
		 * @param[in]   fieldId     Contact child field ID
		 * @return Contact child field translatable name.
		 *
		 * @see ContactFieldId
		 * @see ContactCompanyFieldId
		 * @see ContactNameFieldId
		 * @see ContactPhoneticNameFieldId
		 */
		const char *getContactChildFieldName(unsigned fieldId);

		/**
		 * @brief Contact enum value-name pair.
		 * @see ContactEnumType
		 */
		struct ContactEnumValueName
		{
			int value;          /**< Enum value */
			const char *name;   /**< Translatable value name */
		};

		/**
		 * @brief Get contact enum value names.
		 * @param[in]   type    Enum type
		 * @return Enum value translatable names range.
		 */
		Utils::Range<const ContactEnumValueName *> getContactEnumValueNames(Model::ContactEnumType type);

		/**
		 * @brief Get translatable name for enum value.
		 * @param[in]   type    Enum type
		 * @param[in]   value   Enum value
		 * @return Enum value translatable name.
		 */
		const char *getContactEnumValueName(Model::ContactEnumType type, int value);
	}
}

#endif /* CONTACTS_COMMON_STRINGS_H */
