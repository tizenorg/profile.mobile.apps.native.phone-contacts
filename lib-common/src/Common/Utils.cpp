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

#include "Common/Utils.h"
#include "Utils/Logger.h"

std::string Common::highlightStr(const char *str, const Substring &substr)
{
	static const char *tagPrefix = "<match>";
	static const char *tagSuffix = "</match>";
	static size_t tagPrefixLength = strlen(tagPrefix);

	std::string highlightedStr = str;
	size_t prefixPos = substr.begin() - str;
	size_t suffixPos = substr.end() - str + tagPrefixLength;

	highlightedStr.insert(prefixPos, tagPrefix);
	highlightedStr.insert(suffixPos, tagSuffix);

	return highlightedStr;
}
