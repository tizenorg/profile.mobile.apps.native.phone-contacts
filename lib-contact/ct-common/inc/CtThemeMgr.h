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

#ifndef _CT_THEME_MGR_H_
#define _CT_THEME_MGR_H_

#include <string>
#include <vector>
#include <memory>

#include "WDefine.h"

/**
 * @brief Provides methods to handle look and feel of Elementary Widgets.
 */
class WAPP_ASSIST_EXPORT CtThemeMgr
{
public:

	/**
	 * @brief Gets instance of CtThemeMgr class
	 * @return instance of CtThemeMgr class
	 */
	static CtThemeMgr* getInstance();

	/**
	 * @brief Destroy instance of CtThemeMgr class
	 */
	void destroy();

	/**
	 * @brief Sets a specific theme to be used for this object and its children.
	 * @param[in]   obj The object to get the specific theme from
	 */
	void setObjectTheme(Evas_Object *obj);

	/**
	 * @brief Gets the specific theme to be used.
	 * @return The specific theme
	 */
	Elm_Theme* getTheme();

	/**
	 * @brief Appends a theme extension to the list of extensions.
	 * @param[in]   themePath The Edje file path to be used
	 * @return true on success, otherwise false
	 */
	bool addExtensionTheme(const char *themePath);

	/**
	 * @brief Deletes a theme extension from the list of extensions.
	 * @param[in]   themePath The Edje file path to be used
	 * @return true on success, otherwise false
	 */
	bool removeExtensionTheme(const char *themePath);

private:
	CtThemeMgr();
	virtual ~CtThemeMgr();

private:
	static CtThemeMgr* __instance;
	Elm_Theme *__theme;
	std::vector<std::string> __themeList;

};

#endif /* _CT_THEME_MGR_H_ */
