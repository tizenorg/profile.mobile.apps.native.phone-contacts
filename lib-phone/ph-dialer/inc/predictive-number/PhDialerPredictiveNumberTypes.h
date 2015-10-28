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

#ifndef _PH_DIALER_PREDICTIVE_NUMBER_TYPES_H_
#define _PH_DIALER_PREDICTIVE_NUMBER_TYPES_H_

#include "PhDialerSearchInfo.h"
#include <memory>
#include <vector>

namespace Ph {

	namespace Dialer {
		/**
		 * @brief Smart pointer on SearchInfo.
		 */
		typedef std::shared_ptr<SearchInfo> PSearchInfo;

		/**
		 * @brief List of searchable results.
		 */
		typedef std::vector<PSearchInfo> SearchList;

		/**
		 * @brief History of results.
		 */
		typedef std::vector<SearchList> Cache;
	}
}

#endif //_PH_DIALER_PREDICTIVE_NUMBER_TYPES_H_
