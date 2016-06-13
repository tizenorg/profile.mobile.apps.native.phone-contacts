/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_LIST_MODEL_FAVORITES_PROVIDER_H
#define CONTACTS_LIST_MODEL_FAVORITES_PROVIDER_H

#include "Contacts/List/Model/PersonProvider.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			/**
			 * @brief Provides list of favorite persons
			 */
			class FavoritesProvider : public PersonProvider
			{
			public:
				/**
			     * @brief Determines which items are displayed.
				 */
				enum Mode
				{
					ModeOnly,                   /**< Only favorites are displayed */
					ModeExclude                 /**< Displayed all contacts without favorites */
				};

				/**
				 * @brief Constructor
				 * @param[in]    mode          The mode
				 * @param[in]    filterType    The filter
				 */
				explicit FavoritesProvider(Mode mode = ModeOnly, int filterType = FilterNone);
				virtual const char *getName() { return "FavoritesProvider"; }

				/**
				 * @brief Destructor
				 */
				virtual ~FavoritesProvider() override;

			protected:
				/**
				 * @see PersonProvider::createPerson
				 */
				virtual Person *createPerson(contacts_record_h record) override;

				/**
				 * @see PersonProvider::getFilter
				 */
				virtual contacts_filter_h getFilter() const override;

				/**
				 * @see PersonProvider::getQuery
				 */
				virtual contacts_query_h getQuery() const override;

			private:
				Mode m_Mode;
			};
		}
	}
}

#endif /* CONTACTS_LIST_MODEL_FAVORITES_PROVIDER_H */
