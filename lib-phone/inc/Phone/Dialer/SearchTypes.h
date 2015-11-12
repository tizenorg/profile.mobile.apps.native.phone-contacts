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

#ifndef PHONE_DIALER_SEARCH_TYPES_H
#define PHONE_DIALER_SEARCH_TYPES_H

#include "Phone/Dialer/SearchResult.h"
#include <memory>
#include <vector>

namespace Phone
{
	namespace Dialer
	{
		/**
		 * @brief Smart pointer to SearchResult.
		 */
		typedef std::shared_ptr<SearchResult> SearchResultPtr;

		/**
		 * @brief List of searchable results.
		 */
		typedef std::vector<SearchResultPtr> SearchResults;

		/**
		 * @brief History of results.
		 */
		typedef std::vector<SearchResults> SearchHistory;
	}
}

#endif /* PHONE_DIALER_SEARCH_TYPES_H */
