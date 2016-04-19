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

#ifndef CONTACTS_MODEL_CONTACT_RECORD_PROVIDER_H
#define CONTACTS_MODEL_CONTACT_RECORD_PROVIDER_H

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
			 * @see ContactDataProvider::getDataList()
			 */
			virtual const DataList &getDataList() override;

			/**
			 * @see ContactDataProvider::clearDataList()
			 */
			virtual void clearDataList() override;

		protected:
			using ContactDataProvider::onInserted;

			/**
			 * @brief Create ContactData object
			 * @param[in]   record  Contact record
			 */
			virtual ContactData *createContactData(contacts_record_h record);

			/**
			 * @brief Creates contact data and insert it in the list
			 * @param[in]   record  Contact record
			 */
			void insertContactData(contacts_record_h record);

			/**
			 * @brief Retrieve _contacts_contact record from db by @a id
			 * @remark Override this method when you need to get _contacts_contact
			 *      record filtered, with projection etc. Or when you need to get
			 *      record from another DB view (not _contacts_contact)
			 * @param[in]   id  Contact ID
			 * @return Record on success, otherwise nullptr
			 */
			virtual contacts_record_h getRecord(int id);

			/**
			 * @return contact list
			 */
			const DataList &getContactList();

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
			void onChanged(DataList::iterator contactIt, int id, contacts_changed_e changeType);

		private:
			/**
			 * @brief Determine should we reassign changed callback on ContactData update or not
			 * @remark Override this method only when you need to reassign changed callback
			 * @return true if should update callback, false if not
			 */
			virtual bool shouldUpdateChangedCallback() { return false; }

			void updateChangedCallback(DataList::iterator it);

			DbChangeObserver::CallbackHandle m_Handle;
			DataList m_ContactList;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_RECORD_PROVIDER_H */
