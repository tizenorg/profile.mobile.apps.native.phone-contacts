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
			class MfcProvider : public Contacts::List::Model::PersonProvider
			{
			public:
				MfcProvider();
				virtual ~MfcProvider() override;

			protected:
				/**
				 * @see PersonProvider::getFilter
				 */
				virtual contacts_filter_h getFilter() const override;

				/**
				 * @see PersonProvider::getQuery
				 */
				virtual contacts_query_h getQuery() const override;

			private:
				virtual contacts_list_h getPersonList() const override;

				void onPersonUsageUpdate(const char *viewUri);

				void updateMfcList();
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_MFC_PROVIDER_H */
