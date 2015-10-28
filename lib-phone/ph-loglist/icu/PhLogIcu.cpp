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

#include <utils_i18n.h>
#include <system_settings.h>

#include "ContactsDebug.h"
#include "PhLogIcu.h"
#include <string>

#define LOGS_COMMON_MAX_UCHAR_PATTERN_LEN	64
#define LOGS_COMMON_MAX_CHAR_PATTERN_LEN	128
#define PH_TEXT_SHORT_LEN 256

//// for ICU ///
static char *systemLocale = NULL;
static char *systemTimezone = NULL;
static std::string groupFormatType("dd/MM/y");
static PhLogIcu::PhTimeFormat timeFormatType = PhLogIcu::PH_TIME_FORMAT_12H;

PhLogIcu::PhLogIcu()
{
	// TODO Auto-generated constructor stub

}

PhLogIcu::~PhLogIcu()
{
	// TODO Auto-generated destructor stub
}

char *PhLogIcu::__getString(const char *pattern, time_t timeData)
{
    WENTER();

	i18n_udate_format_h formatter = NULL;
	i18n_uchar formatted[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};
	char result[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};
	int formattedLength;
	i18n_udate date;

	i18n_uchar bestPattern[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};
	i18n_ustring_copy_ua_n(bestPattern, pattern, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN);

	i18n_uchar utf16_timezone[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};

	if(NULL == systemTimezone)
	{
	    WWARNING("systemTimezone = NULL");
	}
	i18n_ustring_copy_ua_n(utf16_timezone, systemTimezone, SAFE_STRLEN(systemTimezone));
	i18n_udate_create(I18N_UDATE_PATTERN , I18N_UDATE_PATTERN , systemLocale, utf16_timezone, -1, bestPattern, -1, &formatter);
	date = (i18n_udate)timeData * 1000;

	i18n_udate_format_date(formatter, date, formatted, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN, NULL, &formattedLength);
	i18n_ustring_copy_au_n(result, formatted, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN);

	i18n_udate_destroy(formatter);

	return strdup(result);

}

char *PhLogIcu::__getDateByBestPattern(const char* skeleton)
{
	WPRET_VM(systemLocale == NULL, NULL, "systemLocale is NULL");
	WPRET_VM(skeleton == NULL, NULL, "skeleton is NULL");

	static char lastTimezoneId[PH_TEXT_SHORT_LEN+1] = {0};
	static char lastLocale[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0};
	static char lastSkeleton[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0};
	static char lastBestPatternString[LOGS_COMMON_MAX_CHAR_PATTERN_LEN + 1] = {0};

	// Use previous cache (for performance)
	if(strcmp(systemLocale, lastLocale) == 0 && strcmp(skeleton, lastSkeleton) == 0 && (strlen(lastBestPatternString) > 0)
			&& (systemTimezone && strncmp(systemTimezone, lastTimezoneId, PH_TEXT_SHORT_LEN-1) == 0))
	{
		return strdup(lastBestPatternString);
	}

	WWARNING("ICU cache updated!");
	if(systemTimezone)
	{
		strncpy(lastTimezoneId, systemTimezone, PH_TEXT_SHORT_LEN-1);
	}

	i18n_udatepg_h pattern_generator = NULL;

	if(!pattern_generator) {
		i18n_udatepg_create(systemLocale, &pattern_generator);
	}
	if(!pattern_generator) {
		WDEBUG("udatpg_open fail");
		return NULL;
	}

	i18n_uchar bestPattern[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};
	char bestPatternString[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0,};
	int bestPatternLength, len;
	i18n_uchar uch_custom_format[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {0};

	int skeletonLength = strlen(skeleton);

	if (skeletonLength <= LOGS_COMMON_MAX_UCHAR_PATTERN_LEN)
		i18n_ustring_copy_ua_n(uch_custom_format, skeleton, skeletonLength);

	else
		i18n_ustring_copy_ua_n(uch_custom_format, skeleton, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN);

	len = i18n_ustring_get_length(uch_custom_format);

	i18n_udatepg_get_best_pattern(pattern_generator, uch_custom_format, len, bestPattern, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN, &bestPatternLength);
	i18n_ustring_copy_au_n(bestPatternString, bestPattern, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN);
	i18n_udatepg_destroy(pattern_generator);

	strncpy(lastLocale, systemLocale, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN-1);
	strncpy(lastSkeleton, skeleton, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN-1);
	strncpy(lastBestPatternString, bestPatternString, LOGS_COMMON_MAX_CHAR_PATTERN_LEN);

	return strdup(bestPatternString);
}

void PhLogIcu::setTimeFormat()
{
	bool timeformat = false;

	system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, &timeformat);

	if (timeformat == true)
		timeFormatType = PH_TIME_FORMAT_24H;
	else
		timeFormatType = PH_TIME_FORMAT_12H;
}

int PhLogIcu::generatePatternForLocal(const char *locale, const char *patternFormat, char *genPattern, int genPatternSize)
{
	WENTER();
	int status = 0;
	i18n_udatepg_h udatepg = NULL;
	i18n_uchar format[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {'\0'};
	i18n_uchar bestPattern[LOGS_COMMON_MAX_UCHAR_PATTERN_LEN] = {'\0'};
	int patternLen = 0;

	status = i18n_udatepg_create(locale, &udatepg);
	if (!udatepg || status != 0) {
		WDEBUG("i18n_udatepg_create() failed: %d", status);
		return -1;
	}

	i18n_ustring_copy_ua_n(format, patternFormat, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN);

	status = i18n_udatepg_get_best_pattern(udatepg, format, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN,
			bestPattern, LOGS_COMMON_MAX_UCHAR_PATTERN_LEN, &patternLen);
	if (status != I18N_ERROR_NONE) {
		WDEBUG("i18n_udatepg_get_best_pattern() failed: %d", status);
		i18n_udatepg_destroy(udatepg);
		return -1;
	}

	i18n_ustring_copy_au_n(genPattern, bestPattern, genPatternSize);
	i18n_udatepg_destroy(udatepg);

	return 0;
}

void PhLogIcu::setDateFormat()
{
	WENTER();
	char *country = NULL;
	char result[LOGS_COMMON_MAX_CHAR_PATTERN_LEN] = { '\0' };
	int res = 0;

	res = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, &country);
	if (res != SYSTEM_SETTINGS_ERROR_NONE )
	{
		return;
	}

	res = generatePatternForLocal(country, "yy-MM-dd", result, LOGS_COMMON_MAX_CHAR_PATTERN_LEN);
	free(country);
	if (res != 0)
	{
		WDEBUG("failed to generate pattern for local");
		return ;
	}
	groupFormatType = result;
}

void PhLogIcu::setLocale()
{
	WENTER();
	int status = I18N_ERROR_NONE ;

	status = i18n_ulocale_set_default(getenv("LC_TIME"));
	WDEBUG("status : [%d]", status);
	if (systemLocale)
	{
		free(systemLocale);
		systemLocale = NULL;
	}
	const char* locale;
	i18n_ulocale_get_default(&locale);
	systemLocale = SAFE_STRDUP(locale);
	WDEBUG("%s",systemLocale);
	strtok((char *)systemLocale, ".UTF8");
}

char *PhLogIcu::getTimeString(int time)
{
	time_t timeData = time;
	char *timeFormat = NULL;
	char *resultString = NULL;

	if (timeFormatType == PH_TIME_FORMAT_12H)
		timeFormat = __getDateByBestPattern("hma");
	else
		timeFormat = __getDateByBestPattern("Hm");

	resultString = __getString(timeFormat, timeData);

	if (timeFormat) free(timeFormat);

	return resultString;
}

char *PhLogIcu::getGroupIndexDateString(int time)
{
	time_t timeData = time;
	return __getString(groupFormatType.c_str(), timeData);
}

char *PhLogIcu::getCallDurationString(int time)
{
	int sec, min, hour, day;
	struct tm tmp={0};
	char buf_t[1024]={0};

	sec = time;
	day = sec/(60*60*24);
	sec %= (60*60*24);
	hour = sec/(60*60);
	sec %= (60*60);
	min = sec/(60);
	sec %= (60);
	tmp.tm_mday = day;
	tmp.tm_hour = hour;
	tmp.tm_min = min;
	tmp.tm_sec = sec;
	strftime(buf_t, sizeof(buf_t), "%H:%M:%S", &tmp);

	return strdup(buf_t);
}

void PhLogIcu::setDefaultTimezoneId()
{
	WENTER();
	int error = I18N_ERROR_NONE;
	i18n_uchar utimezone_id[PH_TEXT_SHORT_LEN] = {0};
	char  timezone_buffer[PH_TEXT_SHORT_LEN] = {0};
	char timezone_id[PH_TEXT_SHORT_LEN] = {0};
	char* buffer;
	int res = 0;
	int timezone_str_size;

	res =  system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, &buffer);
	WPRET_M(SYSTEM_SETTINGS_ERROR_NONE != res, "SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE is NULL");
	strncpy(timezone_id, buffer, sizeof(timezone_id)-1);
	timezone_str_size = readlink("/opt/etc/localtime", timezone_buffer, sizeof(timezone_buffer)-1);
	free(buffer);

	if (timezone_str_size > 0) {
		char *ptr, *sp, *zone = NULL, *city = NULL;
		ptr = strtok_r(timezone_buffer, "/", &sp);

		while((ptr = strtok_r(NULL, "/", &sp))) {
			zone = city;
			city = ptr;
		}

		if(zone != NULL && city != NULL) {
			if(strcmp("zoneinfo", zone) == 0) {
				snprintf(timezone_id, sizeof(timezone_id), "%s", city);
			}
			else {
				snprintf(timezone_id, sizeof(timezone_id), "%s/%s", zone, city);
			}
		}
		WDEBUG("timezone_id:%s", timezone_id);
		if (systemTimezone)
			free(systemTimezone);
		systemTimezone = strdup(timezone_id);
	}
	else {
		WDEBUG("%s", "readlink fail");
	}

	if (*timezone_id) {
		i18n_ustring_copy_ua_n(utimezone_id, timezone_id, PH_TEXT_SHORT_LEN);
		error = i18n_ucalendar_set_default_timezone(utimezone_id);
		WDEBUG("error:%d, utimezone_id:%s, timezone_id:%s", error, utimezone_id, timezone_id);
	}
	else {
		WPRET_M(NULL == timezone_id, "get timezone_id is failed(%d)", error);
	}
}
