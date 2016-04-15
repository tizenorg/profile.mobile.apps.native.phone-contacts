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

#ifndef CONTACTS_LIST_MODEL_MFC_PROVIDER_H
#define CONTACTS_LIST_MODEL_MFC_PROVIDER_H

#include "Contacts/Model/ContactDataProvider.h"

#include <contacts.h>

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of most frequent contacts(mfc).
			 */
			class MfcProvider : public Contacts::Model::ContactDataProvider
			{
			public:
				virtual ~MfcProvider() override;

				/**
				 * @brief Get mfc list
				 * @return List of contact objects
				 */
				virtual const Contacts::Model::ContactDataList &getContactDataList() override;

				/**
				 * @brief Update mfc list
				 * @return True if the list was updated, otherwise - false.
				 */
				bool updateContactDataList();

			private:
				void createMfcList(contacts_list_h list);
				void clearMfcList();

				Contacts::Model::ContactDataList m_MfcList;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_MFC_PROVIDER_H */
