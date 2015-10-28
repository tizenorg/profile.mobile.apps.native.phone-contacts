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

#ifndef _PH_LOG_ICU_H_
#define _PH_LOG_ICU_H_

class PhLogIcu
{
public:
	PhLogIcu();
	virtual ~PhLogIcu();

public:
	typedef enum {
		PH_TIME_FORMAT_12H = 0,
		PH_TIME_FORMAT_24H,
		PH_TIME_FORMAT_UNKNOWN
	} PhTimeFormat;

	typedef enum {
		PH_DATE_FORMAT_DD_MM_YYYY = 0,
		PH_DATE_FORMAT_MM_DD_YYYY,
		PH_DATE_FORMAT_YYYY_DD_MM,
		PH_DATE_FORMAT_YYYY_MM_DD,
		PH_DATE_FORMAT_UNKNOWN
	} PhGroupIndexDateFormat;

	static void setTimeFormat();
	static int generatePatternForLocal(const char *locale, const char *patternFormat, char *genPattern, int genPatternSize);
	static void setDateFormat();
	static void setLocale();
	static void setDefaultTimezoneId();

	static char *getTimeString(int time);
	static char *getGroupIndexDateString(int time);
	static char *getCallDurationString(int time);

private:
	static char *__getString(const char *pattern, time_t timeData);
	static char *__getDateByBestPattern(const char* skeleton);
};

#endif /* _PH_LOG_ICU_H_ */

