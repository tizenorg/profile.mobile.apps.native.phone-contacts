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

#include "Contacts/Model/ContactDataProvider.h"
#include "Contacts/Model/DbChangeObserver.h"

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API ContactRecordProvider : public ContactDataProvider
		{
		public:
			ContactRecordProvider();
			virtual ~ContactRecordProvider() override;

			/**
			 * @see ContactDataProvider::getContactDataList()
			 */
			virtual const ContactDataList &getContactDataList() override;

		protected:
			/**
			 * @see ContactData::ContactData()
			 */
			virtual ContactData *createContact(contacts_record_h record);

		private:
			void onContactInserted(int id, contacts_changed_e changeType);
			void onContactChanged(ContactDataList::iterator contactIt, int id, contacts_changed_e changeType);

			DbChangeObserver::CallbackHandle m_Handle;
			ContactDataList m_ContactList;
		};
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_RECORD_PROVIDER_H */
