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

#ifndef CONTACTS_MODEL_SEARCH_DATA_H
#define CONTACTS_MODEL_SEARCH_DATA_H

#include "Contacts/Model/ContactData.h"
#include "Contacts/Model/SearchResult.h"
#include "Common/Utils.h"
#include "Utils/Range.h"

#include <string>

namespace Contacts
{
	namespace Model
	{
		class EXPORT_API SearchData : public ContactData
		{
		public:
			/**
			 * @brief Create SearchData object
			 * @param[in]   contactData     Contact reference
			 */
			SearchData(ContactData &contactData);

			/**
			 * @see ContactData::getId()
			 */
			virtual int getId() const override;

			/**
			 * @see ContactData::getName()
			 */
			virtual const char *getName() const override;

			/**
			 * @see ContactData::getNumber()
			 */
			virtual const char *getNumber() const override;

			/**
			 * @see ContactData::getImagePath
			 */
			virtual const char *getImagePath() const override;

			/**
			 * @brief Compare @a str with SearchData field
			 *
			 * Compare searchable @a str string with one of
			 * name/number SearchData field. Create found substring if found one.
			 *
			 * @param[in]   str     Searchable string
			 * @return Search result if found something, nullptr if not
			 */
			virtual SearchResultPtr compare(const std::string &str) = 0;

			/**
			 * @return ContactData reference
			 */
			ContactData &getContactData();

			/**
			 * @return Search result
			 */
			const SearchResult *getSearchResult() const;

			/**
			 * @brief Set search result
			 * @param[in]   searchResult    Search result
			 */
			void setSearchResult(SearchResult *searchResult);

		private:
			ContactData &m_ContactData;
			SearchResult *m_SearchResult;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_DATA_H */
