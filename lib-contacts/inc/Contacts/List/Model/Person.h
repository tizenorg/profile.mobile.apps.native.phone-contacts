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

#ifndef CONTACTS_LIST_MODEL_PERSON_H
#define CONTACTS_LIST_MODEL_PERSON_H

#include "Contacts/Model/ContactData.h"
#include "Common/Database/ChildRecordIterator.h"
#include "Utils/UniString.h"

#include <contacts.h>
#include <vector>

namespace Utils
{
	class UniString;
}

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Person object
			 */
			class Person : public Contacts::Model::ContactData
			{
			public:
				typedef std::vector<Common::Database::ChildRecordRange> ContactChildRecords;

				/**
				 * @brief Complete ContactData::Field with person object fields
				 * @see ContactData::Field
				 */
				enum PersonField
				{
					PersonFieldSortValue = FieldMax, /**< Sort value*/
					PersonFieldContact,              /**< Contact field */
					PersonFieldMax                   /**< Sentinel value */
				};

				/**
				 * @brief Complete ContactData changes on update event with Person changes
				 * @see ContactData::ChangedInfo
				 */
				enum PersonChangedInfo
				{
					ChangedSortValue = 1 << PersonFieldSortValue,   /**< Sort value has changed */
					ChangedContact   = 1 << PersonFieldContact      /**< Whole contact has changed */
				};

				/**
				 * @brief Create person object
				 * @param[in]   record      _contacts_person record
				 */
				explicit Person(contacts_record_h record);
				virtual ~Person() override;

				/**
				 * @return Person ID
				 */
				virtual int getId() const override;

				/**
				 * @see ContactData::getName()
				 */
				virtual const char *getName() const override;

				/**
				 * @see ContactData::getNumber()
				 */
				virtual const char *getNumber() const override;

				/**
				 * @see ContactData::getImagePath()
				 */
				virtual const char *getImagePath() const override;

				/**
				 * @return Person record
				 */
				contacts_record_h getRecord() const;

				/**
				 * @return Default contact ID
				 */
				int getContactId() const;

				/**
				 * @return Nickname
				 */
				const char *getNickname() const;

				/**
				 * @return Note
				 */
				const char *getNotes() const;

				/**
				 * @return Organization record
				 */
				contacts_record_h getOrganization() const;

				/**
				 * @return Person address list
				 */
				const ContactChildRecords getAddresses() const;

				/**
				 * @return Person email list
				 */
				const ContactChildRecords getEmails() const;

				/**
				 * @return Person number list
				 */
				const ContactChildRecords getNumbers() const;

				/**
				 * @return First letter from formatted person name
				 */
				const Utils::UniString &getIndexLetter() const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Person &that) const;

				/**
				 * @brief Add contact to person
				 * @param[in]   record  Contact record
				 */
				void addContact(contacts_record_h record);

				/**
				 * @brief Remove contact from person
				 * @param[in]   id  Contact ID
				 */
				void removeContact(int id);

				/**
				 * @brief Get count of linked contacts in person
				 */
				size_t getContactCount() const;

			private:
				friend class PersonProvider;

				template <typename Pred>
				contacts_record_h getContactChildRecord(unsigned propertyId, Pred predicate) const;
				const ContactChildRecords getContactChildRecords(unsigned propertyId) const;

				const Utils::UniString &getSortValue() const;
				void update(contacts_record_h personRecord);
				int updateName(contacts_record_h record, unsigned sortProperty);

				contacts_record_h m_Record;
				contacts_record_h m_DefaultContactRecord;
				std::vector<contacts_record_h> m_ContactRecords;

				Utils::UniString m_IndexLetter;
				mutable Utils::UniString m_SortValue;
				unsigned m_SortProperty;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_H */
