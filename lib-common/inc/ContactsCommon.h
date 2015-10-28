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

#ifndef _CONTACTS_COMMON_H_
#define _CONTACTS_COMMON_H_

#include <app.h>
#include "WDefine.h"
#include <Elementary.h>
#include <functional>
#include <string>

#if !defined(EDJDIR)
#  define EDJDIR "edje/"
#endif

#if !defined(LOCALE_DIR)
#define LOCALE_DIR "locale/"
#endif

#define CONTACTS_APPCONTROL_KEY_OPERATION "appcontrol_key_operation"
#define CONTACTS_APPCONTROL_KEY_URI "appcontrol_key_uri"
#define CONTACTS_APPCONTROL_KEY_APPID "appcontrol_key_appid"

#define SYSTEM_INFO_KEY_SCREEN_WIDTH "http://tizen.org/feature/screen.width"
#define SYSTEM_INFO_KEY_SCREEN_HEIGHT "http://tizen.org/feature/screen.height"

//screen width and height according GUI
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280
//Icon dimensions according GUI
#define ICON_LOG_DETAIL_HEIGHT	45
#define ICON_LOG_DETAIL_WIDTH	45

#define TAG_MATCH_PREFIX	"<match>"
#define TAG_MATCH_SUFFIX	"</match>"

/**
 * @brief Common methods
 */
class WAPP_ASSIST_EXPORT ContactsCommon
{
public:
	ContactsCommon();
	virtual ~ContactsCommon();

public:

	/**
	 * @brief Load localization
	 */
	static void loadLocalization();

	/**
	 * @brief Gets app_control data
	 * @param[in]   appControl   The app_control handle
	 * @param[in]   key          The key of the value contained in the app_control
	 * @return value associated with the given key
	 */
	static std::string getAppControlData(app_control_h appControl, const std::string& key);

	/**
	 * @brief Gets full path for resource file
	 * @param[in]   relativePath   Name of file
	 * @return Full path to file
	 */
	static std::string getResourcePath(const std::string& relativePath);

	/**
	 * @brief Calculate width considering width of device display
	 * @param[in]   width   Width in pixels for display width 720
	 * @return scaled width.
	 */
	static int getScaledWidth(int width);

	/**
	 * @brief Calculate height considering height of device display
	 * @param[in]   height      Height in pixels for display height 1280
	 * @return scaled height.
	 */
	static int getScaledHeight(int height);

	/**
	 * @brief Highlight text from position
	 * @param[in]   text       Text to highlight
	 * @param[in]   position   Position from which the text should be highlighted
	 * @param[in]   length     Length of highlighted part of text
	 * @return highlighted string.
	 */
	static std::string highlightTextByPos(std::string &text, size_t position, size_t length);

	/**
	 * @brief Highlight matched text
	 * @param[in]   text         Text to highlight
	 * @param[in]   searchText   Search string
	 * @return highlighted string.
	 */
	static std::string getHighlightText(const std::string &text, const std::string &searchText);

private:
	static bool __isValidCharForNumber(char character);
};
#endif // _CONTACTS_COMMON_H_

