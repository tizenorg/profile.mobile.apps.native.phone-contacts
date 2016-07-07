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

#include "Contacts/List/Model/PersonProvider.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of most frequent contacts(mfc).
			 */
			class MfcProvider : public PersonProvider
			{
			public:
				MfcProvider();
				virtual ~MfcProvider() override;

			private:
				virtual contacts_list_h getPersonList() const override;
				virtual contacts_record_h getPersonRecord(int id, IdType idType) const override;
				virtual void insertPerson(contacts_record_h personRecord) override;
				virtual void deletePerson(DataList::const_iterator personIt) override;

				contacts_list_h getPersonUsageList() const;

				void onPersonUsageUpdate(const char *viewUri);
				bool update();
				static bool equalPredicate(::Model::DataItem *data, contacts_record_h record);
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_MFC_PROVIDER_H */
