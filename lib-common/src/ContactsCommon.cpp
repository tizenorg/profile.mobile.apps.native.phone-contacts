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

#include <dlfcn.h>
#include <string.h>
#include <algorithm>
#include <Elementary.h>
#include <app.h>
#include <Eina.h>
#include <telephony.h>
#include <system_settings.h>
#include <system_info.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "PHONE"
#include "WDebugBase.h"

#include "ContactsCommon.h"
#include "ContactsDebug.h"


#define CT_EDITFIELD_MAX_BUFFER 1000*3

ContactsCommon::ContactsCommon()
{
	// TODO Auto-generated constructor stub

}

ContactsCommon::~ContactsCommon()
{
	// TODO Auto-generated destructor stub
}

bool ContactsCommon::__isValidCharForNumber(char character)
{
	if((isdigit(character))
		||character=='*'
		||character=='#'
		||character=='+'
		||character==';'
		||character==','
		||character=='N'
		||character=='.'
		||character=='/'
		||character=='('
		||character==')')
		return true;
	else
		return false;
}

void ContactsCommon::loadLocalization()
{
#if defined RESDIR
	bindtextdomain(PACKAGE, RESDIR LOCALE_DIR);
#else
	char *path = app_get_resource_path();
	size_t len = strlen(path);
	size_t newLen = len + sizeof(LOCALE_DIR);

	path = (char*) realloc(path, newLen);
	strncpy(path + len, LOCALE_DIR, newLen);

	bindtextdomain(PACKAGE, path);
	free(path);
#endif
}

std::string ContactsCommon::getAppControlData(app_control_h appControl, const std::string& key)
{
	char* value = NULL;
	std::string ret = "";

	if (key.compare(CONTACTS_APPCONTROL_KEY_OPERATION) == 0)
	{
		app_control_get_operation(appControl, &value);
	}
	else if (key.compare(CONTACTS_APPCONTROL_KEY_URI) == 0)
	{
		app_control_get_uri(appControl, &value);
	}
	else if (key.compare(CONTACTS_APPCONTROL_KEY_APPID) == 0)
	{
		app_control_get_app_id(appControl, &value);
	}
	else
	{
		if (APP_CONTROL_ERROR_NONE != app_control_get_extra_data(appControl, key.c_str(), &value))
			WINFO("key: [%s] not found", key.c_str());
	}

	if (value)
	{
		ret = value;
		free(value);
		WINFO("key: %s, value: %s", key.c_str(), ret.c_str());
	}

	return ret;
}

std::string ContactsCommon::getResourcePath(const std::string& relativePath)
{
	static std::string resPath;
	if (resPath.empty()) {
		#if defined RESDIR
			resPath = RESDIR;
		#else
			char *path = app_get_resource_path();
			resPath = path;
			free(path);
		#endif
	}

	return std::string(resPath).append(relativePath);
}

int ContactsCommon::getScaledWidth(int width)
{
	int systemWidth = 0;
	int scaledWidth = 0;
	int err = system_info_get_platform_int(SYSTEM_INFO_KEY_SCREEN_WIDTH, &systemWidth);
	if (err != SYSTEM_INFO_ERROR_NONE) {
		WERROR("system_info_get_platform_int error %d", err);
		return width;
	}
	scaledWidth = (systemWidth * width) / SCREEN_WIDTH;
	return scaledWidth;
}

int ContactsCommon::getScaledHeight(int height)
{
	int systemHeight = 0;
	int scaledHeight = 0;
	int err = system_info_get_platform_int(SYSTEM_INFO_KEY_SCREEN_HEIGHT, &systemHeight);
	if (err != SYSTEM_INFO_ERROR_NONE) {
		WERROR("system_info_get_platform_int error %d", err);
		return height;
	}
	scaledHeight = (systemHeight * height) / SCREEN_HEIGHT;
	return scaledHeight;
}

std::string ContactsCommon::highlightTextByPos(std::string &text, size_t position, size_t length)
{
	std::string highlightText = text;
	size_t endPos = position + length + sizeof(TAG_MATCH_PREFIX) - 1;

	highlightText.insert(position, TAG_MATCH_PREFIX);
	highlightText.insert(endPos, TAG_MATCH_SUFFIX);

	return highlightText;
}

std::string ContactsCommon::getHighlightText(const std::string &text, const std::string &searchText)
{
	std::string highlightText = text;
	if (searchText.length() == 0 || text.length() == 0) {
		return highlightText;
	}

	std::string lowerText;
	lowerText.resize(text.size());
	std::transform(text.begin(), text.end(), lowerText.begin(), ::tolower);

	std::string lowerSearchText;
	lowerSearchText.resize(searchText.size());
	std::transform(searchText.begin(), searchText.end(), lowerSearchText.begin(), ::tolower);

	size_t startPos = lowerText.find(lowerSearchText);
	if (startPos == lowerText.npos) {
		return highlightText;
	}

	highlightText = highlightTextByPos(highlightText, startPos, searchText.size());

	return highlightText;
}
