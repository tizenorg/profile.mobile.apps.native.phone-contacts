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

#ifndef CONTACTS_LIST_MODEL_VCARD_PROVIDER_H
#define CONTACTS_LIST_MODEL_VCARD_PROVIDER_H

#include "Contacts/ContactDataProvider.h"
#include "Contacts/List/Model/VcardContact.h"

#include <contacts.h>
#include <string>

namespace Contacts
{
	class ContactDataProvider;
}

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class VcardProvider : public ContactDataProvider
			{
			public:
				VcardProvider(std::string path);

				virtual const ContactDataList &getContactDataList() override;

			private:
				contacts_list_h getListFromVcard(std::string path);

			private:
				ContactDataList m_ContactList;
			};
		}
	}
}



#endif /* CONTACTS_LIST_MODEL_VCARD_PROVIDER_H */
