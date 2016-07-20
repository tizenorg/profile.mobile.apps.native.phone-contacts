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

#ifndef CONTACTS_COMMON_STRINGS_H
#define CONTACTS_COMMON_STRINGS_H

#include "Contacts/Model/ContactFields.h"

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
	}
}

#endif /* CONTACTS_COMMON_STRINGS_H */
