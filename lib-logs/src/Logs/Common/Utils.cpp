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

#include "Logs/Common/Utils.h"
#include <system_settings.h>

using namespace Logs;

#define BUFFER_SIZE 32

std::string Common::formatTime(const tm &time)
{
	char buffer[BUFFER_SIZE];
	bool is24Hours = false;

	system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, &is24Hours);
	strftime(buffer, sizeof(buffer), is24Hours ? "%R" : "%I:%M %p", &time);

	return buffer;
}

std::string Common::formatDuration(const tm &time)
{
	char buffer[BUFFER_SIZE];
	strftime(buffer, sizeof(buffer), "%T", &time);

	return buffer;
}
