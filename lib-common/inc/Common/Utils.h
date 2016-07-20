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

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "Utils/Range.h"
#include <string>
#include <tizen.h>

namespace Common
{
	/**
	 * @brief Determines range of chars, which are as substring
	 */
	typedef Utils::Range<const char *> Substring;

	/**
	 * @brief Highlight @a str, that matches @a substring
	 * @remark @a substring must be part of @a str
	 * @param[in]   str        Text to highlight
	 * @param[in]   substr     Highlighted part of @a str string
	 * @return highlighted string.
	 */
	EXPORT_API std::string highlightStr(const char *str, const Substring &substr);
}

#endif /* COMMON_UTILS_H */
