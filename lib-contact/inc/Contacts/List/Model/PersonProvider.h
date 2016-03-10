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

#ifndef CONTACTS_LIST_MODEL_PERSON_PROVIDER_H
#define CONTACTS_LIST_MODEL_PERSON_PROVIDER_H

#include "Contacts/Common/ContactSelectTypes.h"
#include "Contacts/ContactDataProvider.h"
#include "Contacts/DbChangeObserver.h"

#include <contacts.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class Person;

			/**
			 * @brief Provides list of person
			 */
			class PersonProvider : public ContactDataProvider
			{
			public:
				/**
				 * @brief Constructor
				 * @param[in]    filterType    Contact filter
				 */
				explicit PersonProvider(int filterType = FilterNone);

				virtual ~PersonProvider() override;

				/**
				 * @brief Get person list
				 * @return List of contact objects
				 */
				virtual const ContactDataList &getContactDataList() override;

			private:
				contacts_record_h getFilteredRecord(int contactId);

				void insertPerson(contacts_record_h record);

				void setChangedCallback(ContactDataList::iterator personIt);
				void updateChangedCallback(ContactDataList::iterator personIt);
				void unsetChangedCallback(const Person &person);

				void onPersonInserted(int id, contacts_changed_e changeType);
				void onPersonChanged(ContactDataList::iterator it, int id, contacts_changed_e changeType);

				int m_FilterType;

				DbChangeObserver::CallbackHandle m_Handle;
				ContactDataList m_PersonList;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_PROVIDER_H */
