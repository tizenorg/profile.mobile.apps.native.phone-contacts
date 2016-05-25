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

#ifndef CONTACTS_MODEL_SEARCH_ENGINE_H
#define CONTACTS_MODEL_SEARCH_ENGINE_H

#include "Contacts/Model/ContactDataProvider.h"
#include "Contacts/Model/SearchResult.h"

#include <string>
#include <vector>

namespace Contacts
{
	namespace Model
	{
		class SearchData;

		/**
		 * @brief This class provides incremental search logic
		 */
		class EXPORT_API SearchEngine
		{
		public:
			/**
			 * @see ContactDataProvider::DataList
			 */
			typedef ContactDataProvider::DataList DataList;

			/**
			 * @brief Create search engine
			 * @param[in]   dataList  ContactData list
			 */
			explicit SearchEngine(DataList &dataList);

			/**
			 * @brief Perform incremental search
			 * @param[in]   query   Value to find
			 */
			void search(const std::string &query);

			/**
			 * @return true if there is no results, otherwise false
			 */
			bool empty() const;

		private:
			typedef std::pair<SearchData *, SearchResultPtr> SearchListItem;
			typedef std::unique_ptr<SearchListItem> SearchListItemPtr;
			typedef std::list<SearchListItemPtr> SearchList;
			typedef std::vector<SearchList> SearchHistory;

			bool needSearch(const std::string &query);
			template <typename List>
			void incrementalSearch(const List &list, const std::string &query);

			SearchData *getSearchData(ContactData *contactData);
			SearchData *getSearchData(const SearchListItemPtr &searchItem);

			void updateSearchResult(SearchList &list);
			void resetSearchResult();

			SearchHistory::iterator firstMismatch(const std::string &query);
			SearchHistory::iterator skipEmptyResults(size_t offset);

			void clear();

			std::string m_PrevQuery;
			SearchHistory m_History;
			int m_LastFoundIndex;

			DataList &m_DataList;
		};
	}
}

#endif /* CONTACTS_MODEL_SEARCH_ENGINE_H */
