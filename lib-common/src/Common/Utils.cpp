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

#define TAG_PREFIX "<match>"
#define TAG_SUFFIX "</match>"

std::string Common::highlightStr(const char *str, const Substring &substr)
{
	std::string retStr;
	retStr.reserve(sizeof(TAG_PREFIX) + sizeof(TAG_SUFFIX) + strlen(str));

	retStr.append(str, substr.begin());
	retStr.append(TAG_PREFIX);
	retStr.append(substr.begin(), substr.end());
	retStr.append(TAG_SUFFIX);
	retStr.append(substr.end());

	return retStr;
}
