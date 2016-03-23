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
			using ContactDataProvider::onInserted;

			/**
			 * @brief Create contact object
			 * @param[in]   record  Contact record
			 */
			virtual ContactData *createContact(contacts_record_h record);

			/**
			 * @brief Insert contact
			 * @param[in]   record  Contact record
			 */
			void insertContact(contacts_record_h record);

			/**
			 * @param[in]   id  Contact ID
			 * @return Contact record
			 */
			virtual contacts_record_h getRecord(int id);

			/**
			 * @return contact list
			 */
			const ContactDataList &getContactList();

			/**
			 * @brief Contact inserted callback
			 * @param[in]   id          Contact ID
			 * @param[in]   changeType  Contact change type (Inserted)
			 */
			void onInserted(int id, contacts_changed_e changeType);

			/**
			 * @brief Contact changed callback
			 * @param[in]   contactIt   Iterator to contact object in @m m_ContactList
			 * @param[in]   id          Contact ID
			 * @param[in]   changeType  Contact change type (Updated/Deleted)
			 */
			void onChanged(ContactDataList::iterator contactIt, int id, contacts_changed_e changeType);

		private:
			virtual void updateChangedCallback(ContactDataList::iterator it) { }

			DbChangeObserver::CallbackHandle m_Handle;
			ContactDataList m_ContactList;
		};
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_RECORD_PROVIDER_H */
