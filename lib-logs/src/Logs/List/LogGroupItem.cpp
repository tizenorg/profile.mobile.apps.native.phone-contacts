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

#include "Logs/List/LogGroupItem.h"
#include <app_i18n.h>

using namespace Logs::List;

#define PART_LOG_DATE           "elm.text"
#define BUFFER_SIZE             32

LogGroupItem::LogGroupItem(tm date)
	: m_Date(date)
{ }

const tm &LogGroupItem::getDate() const
{
	return m_Date;
}

char *LogGroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_LOG_DATE) == 0) {
		return convertDate(m_Date);
	}

	return nullptr;
}

char *LogGroupItem::convertDate(const tm &date)
{
	time_t nowTime = time(nullptr);
	struct tm nowDate;
	localtime_r(&nowTime, &nowDate);

	if(date.tm_year == nowDate.tm_year &&
			date.tm_mon == nowDate.tm_mon &&
			date.tm_mday == nowDate.tm_mday) {
		return strdup(_("IDS_LOGS_HEADER_TODAY"));
	} else if(date.tm_year == nowDate.tm_year &&
				date.tm_mon == nowDate.tm_mon &&
				(date.tm_mday + 1) == nowDate.tm_mday ) {
		return strdup(_("IDS_LOGS_HEADER_YESTERDAY"));
	} else {
		char buffer[BUFFER_SIZE];
		strftime(buffer, sizeof(buffer), "%x", &date);
		return strdup(buffer);
	}

	return nullptr;
}
