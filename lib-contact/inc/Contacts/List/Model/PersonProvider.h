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

#include "Contacts/List/Model/DbChangeObserver.h"
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
				enum Mode
				{
					ModeAll,
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
					FilterEmail     = 1 << 1
				};

				/**
				 * @brief Person insert callback
				 * @param[in]    person        Person object
				 */
				typedef std::function<void(PersonPtr person)> InsertCallback;

				/**
				 * @brief Person change callback
				 * @param[in]    person        Person object
				 * @param[in]    changeType    Change type
				 */
				typedef std::function<void(PersonPtr person, contacts_changed_e changeType)> ChangeCallback;

				/**
				 * @brief Constructor
				 * @param[in]    filterType    Contact filter
				 */
				explicit PersonProvider(Mode modeType = ModeAll,
						FilterType filterType = FilterNone);

				~PersonProvider();

				/**
				 * @brief Get person list
				 * @return List of contact objects
				 */
				PersonList getPersonList() const;

				/**
				 * @brief Set person insert callback
				 * @param[in]    callback    Create callback
				 */
				void setInsertCallback(InsertCallback callback);

				/**
				 * @brief Unset person insert callback
				 */
				void unsetInsertCallback();

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

			private:
				void onPersonInserted(int id, contacts_changed_e changeType);
				void onPersonChanged(int id, contacts_changed_e changeType);

				Mode m_Mode;
				FilterType m_FilterType;

				std::pair<InsertCallback, Model::DbChangeObserver::CallbackHandle> m_InsertCallback;
				std::unordered_map<int, std::pair<ChangeCallback, Model::DbChangeObserver::CallbackHandle>> m_ChangeCallbacks;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_PERSON_PROVIDER_H */
