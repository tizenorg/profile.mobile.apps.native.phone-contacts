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

#ifndef PHONE_DIALER_SEARCH_ENGINE_H
#define PHONE_DIALER_SEARCH_ENGINE_H

#include "Contacts/Model/ContactDataProvider.h"
#include <string>
#include <vector>

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief This class provides incremental search logic
		 */
		class SearchEngine
		{
		public:
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
			 * @brief Retrieves result list
			 * @return Result list or nullptr on empty list
			 */
			const DataList *getSearchResult() const;

			/**
			 * @return true if there is no results, otherwise false
			 */
			bool empty() const;

		private:
			typedef std::vector<DataList> SearchHistory;

			void firstSearch(const std::string &query);
			void chooseSearch(const std::string &query);

			bool searchInCache(SearchHistory::iterator from, const std::string &query);
			SearchHistory::reverse_iterator firstMismatch(const std::string &query);
			SearchHistory::reverse_iterator skipEmptyResults(size_t offset);

			void clear();

			bool needSearch(const std::string &query);

			std::string m_Query;
			SearchHistory m_Cache;
			int m_LastFoundIndex;

			DataList &m_DataList;
		};
	}
}

#endif /* PHONE_DIALER_SEARCH_ENGINE_H */
