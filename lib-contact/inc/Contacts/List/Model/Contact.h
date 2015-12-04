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

#ifndef CONTACTS_LIST_MODEL_CONTACT_H
#define CONTACTS_LIST_MODEL_CONTACT_H

#include <contacts.h>
#include <memory>
#include <vector>
#include <utils_i18n.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Contact object
			 */
			class Contact;

			typedef std::unique_ptr<Contact> ContactPtr;
			typedef std::vector<ContactPtr> ContactList;

			class Contact
			{
			public:
				Contact(Contact &contact) = delete;

				/**
				 * @brief Create contact object
				 * @param[in]   record      Person record
				 */
				Contact(contacts_record_h record);
				~Contact();

				/**
				 * @brief Compares contact's "Sort by" (first name/last name) values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Contact &that) const;

				/**
				 * @return Person ID
				 */
				int getPersonId() const;

				/**
				 * @return First letter from formatted person name
				 */
				const char *getIndexLetter() const;

				/**
				 * @return Person name
				 */
				const char *getName() const;

				/**
				 * @return Person picture path
				 */
				const char *getPicturePath() const;

				/**
				 * @return Person DB handle
				 */
				const contacts_record_h getRecord() const;

			private:
				const i18n_uchar *getSortValue() const;
				void initSortValue(const char *sortValue) const;
				const char *getDbSortValue() const;

				contacts_record_h m_PersonRecord;
				contacts_record_h m_ContactRecord;

				mutable i18n_uchar *m_SortValue;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_CONTACT_H */
