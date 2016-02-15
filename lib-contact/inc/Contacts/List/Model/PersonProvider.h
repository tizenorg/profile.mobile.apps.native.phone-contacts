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

#include "Contacts/List/Model/Person.h"
#include <contacts.h>
#include <functional>
#include <unordered_map>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of person
			 */
			class PersonProvider
			{
			public:

				/**
				 * @brief Determines what persons to provide
				 */
				enum ModeType
				{
					ModeNone,
					ModeFavorites,
					ModeMFC
				};

				/**
				 * @brief Determines how to filter person list
				 */
				enum FilterType
				{
					FilterNone      = 0,
					FilterNumber    = 1 << 0,
					FilterEmail     = 1 << 1,
				};



				/**
				 * @brief Person change callback
				 * @param[in]    person        Person object
				 * @param[in]    changeType    Change type
				 */
				typedef std::function<void(PersonPtr person, contacts_changed_e changeType)> ChangeCallback;

				/**
				 * @brief Person insert callback
				 * @param[in]    person        Person object
				 */
				typedef std::function<void(PersonPtr person)> InsertCallback;

				/**
				 * @brief Constructor
				 * @param[in]    filterType    Contact filter
				 */
				explicit PersonProvider(ModeType modeType = ModeNone,
						FilterType filterType = FilterNone);

				~PersonProvider();

				/**
				 * @brief Get person list
				 * @return List of contact objects
				 */
				PersonList getPersonList() const;

				/**
				 * @brief Set person change callback
				 * @remark It can be update or delete of person
				 * @param[in]    person      Person
				 * @param[in]    callback    Change callback
				 */
				void setChangeCallback(const Person &person, ChangeCallback callback);

				/**
				 * @brief Unset person change callback
				 * @param[in]    person      Person
				 */
				void unsetChangeCallback(const Person &person);

				/**
				 * @brief Set person insert callback
				 * @param[in]    callback    Create callback
				 */
				void setInsertCallback(InsertCallback callback);

				/**
				 * @brief Unset person insert callback
				 */
				void unsetInsertCallback();

			private:
				void onChanged(const char *viewUri);
				void notify(contacts_changed_e changeType, int contactId);

				ModeType m_ListModeType;
				FilterType m_ListFilterType;

				int m_DbVersion;

				std::unordered_map<int, ChangeCallback> m_ChangeCallbacks;
				InsertCallback m_InsertCallback;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_PROVIDER_H */
