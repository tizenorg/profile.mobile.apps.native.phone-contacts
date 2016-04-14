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
#include "Contacts/Model/ContactRecordProvider.h"
#include "Contacts/Model/DbChangeObserver.h"

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
			class PersonProvider : public Contacts::Model::ContactRecordProvider
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
				virtual const Contacts::Model::ContactDataList &getContactDataList() override;

			protected:
				/**
				 * @see ContactRecordProvider::createContactData
				 */
				virtual Contacts::Model::ContactData *createContactData(contacts_record_h record) override;

				/**
				 * @see ContactRecordProvider::getRecord
				 */
				virtual contacts_record_h getRecord(int id) override;

			private:
				virtual bool shouldUpdateChangedCallback() override;
				void onNameFormatChanged(contacts_name_display_order_e order);

				int m_FilterType;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_PROVIDER_H */
