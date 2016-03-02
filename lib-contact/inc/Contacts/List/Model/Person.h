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

#include "Contacts/DbChangeObserver.h"
#include "Contacts/List/Model/ContactRecordData.h"
#include "Utils/UniString.h"
#include <contacts.h>
#include <memory>
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
			class Person;

			typedef std::unique_ptr<Person> PersonPtr;
			typedef std::vector<PersonPtr> PersonList;

			class Person : public ContactRecordData
			{
			public:
				/**
				 * @brief Array of contact IDs
				 */
				typedef std::vector<int> ContactIds;

				Person(const Person &person) = delete;
				Person &operator=(const Person &person) = delete;

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
				const Utils::UniString &getIndexLetter() const;

				/**
				 * @return _contacts_person record
				 */
				const contacts_record_h getRecord() const;

				/**
				 * @brief Set new _contacts_person record
				 */
				void setRecord(contacts_record_h record);

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Person &that) const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values on inequality
				 * @return True if sort values are not equivalent, otherwise false
				 */
				bool operator!=(const Person &that) const;//Todo: Delete when refactor will be finished

			private:
				void init();
				void clear();
				int update(contacts_record_h record);

				const Utils::UniString &getSortValue() const;
				const char *getDbSortValue() const;

				void subscribeToDb();
				void unsubscribeFromDb();

				int getChanges(contacts_record_h record);

				void onChanged(int id, contacts_changed_e changeType);

				contacts_record_h m_PersonRecord;

				Utils::UniString m_IndexLetter;
				mutable Utils::UniString m_SortValue;

				ContactIds m_ContactIds;

				std::vector<DbChangeObserver::CallbackHandle> m_Handles;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_H */
