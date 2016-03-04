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

#ifndef CONTACTS_LIST_MODEL_VCARD_CONTACT_H
#define CONTACTS_LIST_MODEL_VCARD_CONTACT_H

#include "Contacts/List/Model/ContactRecordData.h"

#include <contacts.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Vcard contact object
			 */
			class VcardContact;

			class VcardContact : public ContactRecordData
			{
			public:
				/**
				 * @brief Create vcard contact object
				 * @param[in]   record      _contacts_contact record
				 */
				VcardContact(contacts_record_h record);
				VcardContact(const VcardContact &contact) = delete;

				VcardContact &operator=(const VcardContact &contact) = delete;

				/**
				 * @return Contact ID
				 */
				virtual int getId() const override;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_VCARD_CONTACT_H */
