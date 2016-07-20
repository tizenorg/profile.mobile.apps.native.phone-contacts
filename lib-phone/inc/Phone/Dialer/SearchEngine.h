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

#ifndef PHONE_DIALER_SEARCH_ENGINE_H
#define PHONE_DIALER_SEARCH_ENGINE_H

#include <string>

#include "Phone/Dialer/SearchTypes.h"

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief This class provides incremental search logic for predictive number functionality
		 */
		class SearchEngine
		{
		public:
			SearchEngine();

			/**
			 * @brief Perform incremental search
			 * @param[in]	number		Value to find
			 */
			void search(const std::string &number);

			/**
			 * @brief Retrieves result list
			 * @return Result list or nullptr on empty list
			 */
			const SearchResults *getSearchResult() const;

			/**
			 * @return true if there is no results, otherwise false
			 */
			bool empty() const;

			/**
			 * @brief make search like a first time
			 * @param[in] number value to find
			 */
			void searchFromScratch(const std::string &number);

		private:
			void distinctLogs(SearchResults &searchList);
			void firstSearch(const std::string &number);
			SearchResults searchInDB(const std::string &number);
			void chooseSearch(const std::string &number);

			bool searchInCache(SearchHistory::iterator from, const std::string &number);
			SearchHistory::reverse_iterator firstMismatch(const std::string &number);
			SearchHistory::reverse_iterator skipEmptyResults(size_t offset);

			void clear();

			bool needSearch(const std::string &number);

			std::string m_Number;
			SearchHistory m_Cache;
			int m_LastFoundIndex;
		};
	}
}

#endif /* PHONE_DIALER_SEARCH_ENGINE_H */
