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

#ifndef CONTACTS_LIST_PERSON_SEARCH_ITEM_H
#define CONTACTS_LIST_PERSON_SEARCH_ITEM_H

#include "Contacts/List/PersonItem.h"
#include "Contacts/List/Model/PersonSearchData.h"

#include <string>

#define PART_SUBTEXT     "elm.text.sub"

namespace Contacts
{
	namespace List
	{
		class PersonSearchItem : public PersonItem
		{
		public:
			/**
			 * @brief Create person search item
			 * @param[in]   person  Person object
			 */
			explicit PersonSearchItem(Model::Person &person);

			/**
			 * @set Person search data
			 * @param[in]   searchData  @ref PersonSearchData
			 */
			void setSearchData(Model::PersonSearchData *searchData);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Eina_Bool compare(Evas_Object *parent, void *filter) override;

			std::string getHighlightedStr() const;

		private:
			Model::PersonSearchData *m_SearchData;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_SEARCH_ITEM_H */
