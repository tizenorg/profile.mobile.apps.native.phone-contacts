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
				virtual const Utils::UniString *getIndexLetter() const override;

				/**
				 * @return _contacts_person record
				 */
				const contacts_record_h getRecord() const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Person &that) const;

			private:
				friend class PersonProvider;

				void initialize(contacts_record_h personRecord,
						contacts_record_h contactRecord);
				int updatePerson(contacts_record_h record);

				const Utils::UniString &getSortValue() const;
				const char *getDbSortValue() const;

				contacts_record_h m_PersonRecord;
				ContactIds m_ContactIds;

				Utils::UniString m_IndexLetter;
				mutable Utils::UniString m_SortValue;

				std::vector<Contacts::Model::DbChangeObserver::CallbackHandle> m_Handles;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_H */
