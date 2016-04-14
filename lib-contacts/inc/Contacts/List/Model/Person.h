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

#include "Contacts/Model/DbChangeObserver.h"
#include "Contacts/Model/ContactRecordData.h"
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
			class Person : public Contacts::Model::ContactRecordData
			{
			public:
				/**
				 * @brief Complete ContactData::Field with person object fields
				 * @see ContactData::Field
				 */
				enum PersonField
				{
					PersonFieldSortValue = FieldMax, /**< Sort value*/
					PersonFieldMax                   /**< Sentinel value */
				};

				/**
				 * @brief Complete ContactData changes on update event with Person changes
				 * @see ContactData::ChangedInfo
				 */
				enum PersonChangedInfo
				{
					ChangedSortValue = 1 << PersonFieldSortValue /**< Sort value has changed */
				};

				/**
				 * @brief Array of contact IDs
				 */
				typedef std::vector<int> ContactIds;

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
				 * @brief Update record from the database
				 */
				void updateDbRecord();

				/**
				 * @see ContactRecordData::getNumbers()
				 */
				virtual const Numbers &getNumbers() override;

				/**
				 * @return Displayed by default contact ID
				 */
				int getDisplayContactId() const;

				/**
				 * @return Array of contact IDs
				 */
				const ContactIds &getContactIds() const;

				/**
				 * @return First letter from formatted person name
				 */
				const Utils::UniString &getIndexLetter() const;

				/**
				 * @return _contacts_person record
				 */
				const contacts_record_h getRecord() const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Person &that) const;

			protected:
				/**
				 * @see ContactRecordData::setChangedCallback()
				 */
				virtual void setChangedCallback(Contacts::Model::DbChangeObserver::Callback callback) override;

				/**
				 * @see ContactRecordData::unsetChangedCallback
				 */
				virtual void unsetChangedCallback() override;

			private:
				friend class PersonProvider;

				virtual void onUpdate(contacts_record_h personRecord) override;
				void initialize(contacts_record_h personRecord);
				const Utils::UniString &getSortValue() const;
				const char *getDbSortValue(contacts_record_h contactRecord) const;

				contacts_record_h m_PersonRecord;
				ContactIds m_ContactIds;

				Utils::UniString m_IndexLetter;
				mutable Utils::UniString m_SortValue;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_H */
