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
#include "Contacts/Model/ContactDataProvider.h"

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
			class PersonProvider : public Contacts::Model::ContactDataProvider
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
				virtual const DataList &getDataList() override;

				/**
				 * @see ContactDataProvider::clearDataList()
				 */
				virtual void clearDataList() override;

				/**
				 * @brief Enable or disable updates from database.
				 */
				void setUpdateMode(bool isEnabled);

				/**
				 * @brief Enable or disable handling of certain update types.
				 * @param[in]   changeTypes     Event types to handle
				 */
				void setUpdateMask(int changeTypes);

			protected:
				enum IdType
				{
					PersonId,
					ContactId
				};

				/**
				 * @brief Create Person object
				 * @param[in]   record  Person record
				 */
				virtual Person *createPerson(contacts_record_h record);

				/**
				 * @brief Fills the filter for query
				 * @return The filter
				 */
				virtual contacts_filter_h getFilter() const;

				/**
				 * @brief Fills the query to get list of persons
				 * @return The query
				 */
				virtual contacts_query_h getQuery() const;

				/**
				 * @return Person record list fetched using filter and query.
				 */
				contacts_list_h getPersonList() const;

				/**
				 * @brief Fetch person record from database by ID.
				 * @param[in]   id      Person or display contact ID
				 * @param[in]   idType  Type of ID
				 * @return Person iterator if found, end iterator otherwise.
				 */
				contacts_record_h getPersonRecord(int id, IdType idType) const;

				/**
				 * @brief Find person by ID.
				 * @param[in]   id      Person or display contact ID
				 * @param[in]   idType  Type of ID
				 * @return Person iterator if found, end iterator otherwise.
				 */
				DataList::iterator findPerson(int id, IdType idType);

				/**
				 * @brief Fetch person from database by ID and insert it into person list.
				 * @param[in]   id      Person or display contact ID
				 * @param[in]   idType  Type of ID
				 * @return Whether insert was successful.
				 */
				bool insertPerson(int id, IdType idType);

				/**
				 * @brief Update person from database.
				 * @param[in]   personIt    Person iterator
				 * @return Whether update was successful.
				 */
				bool updatePerson(DataList::iterator personIt);

				/**
				 * @brief Delete person from person list.
				 * @param[in]   personIt    Person iterator
				 * @return Whether update was successful.
				 */
				void deletePerson(DataList::iterator personIt);

			private:
				static int getIdProperty(IdType idType);
				void updatePersonList();

				void onChanged(const char *uri);
				void onNameFormatChanged(contacts_name_display_order_e order);

				int m_FilterType;
				int m_DbVersion;
				DataList m_PersonList;

				bool m_IsFilled;
				bool m_IsUpdateEnabled;
				bool m_IsUpdatePending;
				int m_UpdateMask;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_PROVIDER_H */
