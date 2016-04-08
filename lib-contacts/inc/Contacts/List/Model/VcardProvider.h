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

#ifndef CONTACTS_LIST_MODEL_VCARD_PROVIDER_H
#define CONTACTS_LIST_MODEL_VCARD_PROVIDER_H

#include "Contacts/Model/ContactDataProvider.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of contacts from vcard
			 */
			class VcardProvider : public Contacts::Model::ContactDataProvider
			{
			public:
				/**
				 * @brief Create list of vcard contacts
				 * @param[in]   path    Vcard path
				 */
				VcardProvider(const char *path);
				VcardProvider(const VcardProvider &contact) = delete;
				virtual ~VcardProvider() override;

				VcardProvider &operator=(const VcardProvider &contact) = delete;

				/**
				 * @see ContactDataProvider::getContactDataList()
				 * @return Contact data list
				 */
				virtual const Contacts::Model::ContactDataList &getContactDataList() override;

			private:
				Contacts::Model::ContactDataList m_ContactsList;
			};
		}
	}
}



#endif /* CONTACTS_LIST_MODEL_VCARD_PROVIDER_H */
