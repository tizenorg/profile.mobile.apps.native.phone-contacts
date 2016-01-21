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

			class Person
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
				~Person();

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Person &that) const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values on equality
				 * @return True if sort values are equivalent, otherwise false
				 */
				bool operator==(const Person &that) const;

				/**
				 * @brief Compares person's "Sort by" (first name/last name) values on inequality
				 * @return True if sort values are not equivalent, otherwise false
				 */
				bool operator!=(const Person &that) const;

				/**
				 * @return Person ID
				 */
				int getId() const;

				/**
				 * @return Array of contact IDs
				 */
				const ContactIds &getContactIds() const;

				/**
				 * @return First letter from formatted person name
				 */
				const Utils::UniString &getIndexLetter() const;

				/**
				 * @return Person name
				 */
				const char *getName() const;

				/**
				 * @return Person image path
				 */
				const char *getImagePath() const;

				/**
				 * @return _contacts_person record
				 */
				const contacts_record_h getRecord() const;

			private:
				const Utils::UniString &getSortValue() const;
				void initSortValue(const char *sortValue) const;
				const char *getDbSortValue() const;

				contacts_record_h m_PersonRecord;
				contacts_record_h m_ContactRecord;

				Utils::UniString m_IndexLetter;
				mutable Utils::UniString m_SortValue;

				mutable ContactIds m_ContactIds;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_H */
