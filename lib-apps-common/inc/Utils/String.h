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

#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <cstring>

namespace Utils
{
	/**
	 * @brief Compare strings for equality allowing null-pointers.
	 * @param[in]   str1    First string
	 * @param[in]   str2    Second string
	 * @return Whether strings are equal.
	 */
	inline bool safeCmp(const char *str1, const char *str2)
	{
		return (str1 && str2) ? (strcmp(str1, str2) == 0) : (str1 == str2);
	}

	/**
	 * @brief Safely duplicate the string.
	 * @see strdup()
	 */
	inline char *safeDup(const char *str)
	{
		return str ? strdup(str) : nullptr;
	}
}

#endif /* UTILS_STRING_H */
