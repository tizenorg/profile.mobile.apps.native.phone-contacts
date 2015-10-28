/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _PH_DIALER_PREDICTIVE_NUMBER_
#define _PH_DIALER_PREDICTIVE_NUMBER_

#include <string>

#include "PhDialerSearchInfo.h"
#include "PhDialerPredictiveNumberTypes.h"

namespace Ph {

	namespace Dialer {

		/**
		 * @brief This class provides incremental search logic for predictive number functionality
		 */
		class PredictiveNumber
		{
		public:
			PredictiveNumber();

			/**
			 * @brief Perform incremental search
			 * @param[in]	number		Value to find
			 */
			void search(const std::string &number);

			/**
			 * @brief Retrieves result list
			 * @return Result list or nullptr on empty list
			 */
			const SearchList *getSearchResult() const;

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
			void distinctLogs(SearchList &searchList);
			void firstSearch(const std::string &number);
			SearchList searchInDB(const std::string &number);
			void chooseSearch(const std::string &number);

			bool searchInCache(Cache::iterator from, const std::string &number);
			Cache::reverse_iterator firstMismatch(const std::string &number);
			Cache::reverse_iterator skipEmptyResults(size_t offset);

			void clear();

			bool needSearch(const std::string &number);

			std::string m_Number;
			Cache m_PredictiveNumberCache;
			int m_LastFoundIndex;
		};
	}
}

#endif //_PH_DIALER_PREDICTIVE_NUMBER_
