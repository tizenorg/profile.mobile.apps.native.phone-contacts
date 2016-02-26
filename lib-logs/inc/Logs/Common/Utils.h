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

#ifndef LOGS_COMMON_UTILS_H
#define LOGS_COMMON_UTILS_H

#include <time.h>
#include <string>

namespace Logs
{
	namespace Common
	{
		/**
		 * @brief Convert tm to string
		 * @param[in]   time   Time to convert
		 * @return string.
		 */
		std::string formatTime(const tm &time);
	}
}

#endif /* LOGS_COMMON_UTILS_H */
