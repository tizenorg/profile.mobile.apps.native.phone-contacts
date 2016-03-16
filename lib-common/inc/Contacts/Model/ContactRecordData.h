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

#ifndef CONTACTS_LIST_MODEL_CONTACT_RECORD_DATA_H
#define CONTACTS_LIST_MODEL_CONTACT_RECORD_DATA_H

#include "Contacts/Model/ContactData.h"
#include "Contacts/Model/DbChangeObserver.h"
#include <contacts.h>

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API ContactRecordData : public ContactData
		{
		public:
			/**
			 * @brief Create ContactRecordData object
			 * @param[in]   type    ContactRecordData type
			 */
			ContactRecordData(Type type);
			virtual ~ContactRecordData() override;

			/**
			 * @brief Update contact record handle
			 * @param[in]   record  Contact record
			 */
			void updateContactRecord(contacts_record_h record);

			/**
			 * @see ContactData::getId()
			 * @return Contact ID
			 */
			virtual int getId() const override;

			/**
			 * @see ContactData::getName()
			 * @return Contact name
			 */
			virtual const char *getName() const override;

			/**
			 * @see ContactData::getNumber()
			 * @return Contact number
			 */
			virtual const char *getNumber() const override;

			/**
			 * @see ContactData::getImagePath()
			 * @return Contact image path
			 */
			virtual const char *getImagePath() const override;

			/**
			 * @see ContactData::compare()
			 * @detail Compares by name
			 */
			virtual bool compare(const char *str) override;

		protected:
			/**
			 * @return contact record
			 */
			const contacts_record_h getContactRecord() const;

			/**
			 * @brief Set changed callback
			 * @param[in]   callback    Change callback
			 */
			void setChangedCallback(DbChangeObserver::Callback callback);

			/**
			 * @brief Unset changed callback
			 */
			void unsetChangedCallback();

			/**
			 * @param[in]   record  Contact record
			 * @return Contact ID
			 */
			static int getContactId(contacts_record_h record);

			/**
			 * @param[in]   record  Contact record
			 * @param[in]   field   Contact field
			 * @return Contact value Name/Number/ImagePath
			 */
			static const char *getValue(contacts_record_h record, Field field);

			/**
			 * @param[in]   oldContact  Old contact record
			 * @param[in]   newContact  New contact record
			 * @return Changes between both contacts
			 */
			static int getChanges(contacts_record_h oldContact, contacts_record_h newContact);

		private:
			friend class ContactRecordProvider;

			void onUpdate(contacts_record_h record);
			void onDelete();

			contacts_record_h m_Record;
			DbChangeObserver::CallbackHandle m_Handle;
		};
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_RECORD_DATA_H */
