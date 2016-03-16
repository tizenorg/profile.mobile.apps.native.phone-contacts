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

#ifndef CONTACTS_LIST_MODEL_CONTACT_RECORD_PROVIDER_H
#define CONTACTS_LIST_MODEL_CONTACT_RECORD_PROVIDER_H

#include "Contacts/ContactDataProvider.h"
#include "Contacts/DbChangeObserver.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class ContactRecordProvider : public ContactDataProvider
			{
			public:
				ContactRecordProvider();
				virtual ~ContactRecordProvider() override;

				/**
				 * @see ContactDataProvider::getContactDataList()
				 */
				virtual const ContactDataList &getContactDataList() override;

			private:
				void onContactInserted(int id, contacts_changed_e changeType);
				void onContactChanged(ContactDataList::iterator contactIt, int id, contacts_changed_e changeType);

				DbChangeObserver::CallbackHandle m_Handle;
				ContactDataList m_ContactList;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_RECORD_PROVIDER_H */
