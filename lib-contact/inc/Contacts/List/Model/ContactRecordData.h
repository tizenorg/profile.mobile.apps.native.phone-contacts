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

#include "Contacts/ContactData.h"
#include <contacts.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class ContactRecordData : public ContactData
			{
			public:
				/**
				 * @brief Create ContactRecordData object
				 * @param[in]   type    ContactRecordData type
				 */
				ContactRecordData(Type type);
				ContactRecordData(const ContactRecordData &contact) = delete;
				virtual ~ContactRecordData() override;

				ContactRecordData &operator=(const ContactRecordData &contact) = delete;

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
				 * @param[in]   record  Contact record
				 * @param[in]   field   Contact field
				 * @return contact value Name/Number/ImagePath
				 */
				static const char *getValue(contacts_record_h record, Field field);

			private:
				contacts_record_h m_Record;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_RECORD_DATA_H */
