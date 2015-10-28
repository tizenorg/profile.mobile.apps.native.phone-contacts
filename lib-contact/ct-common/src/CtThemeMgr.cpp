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

#include "CtCommon.h"
#include "CtThemeMgr.h"

#include "ContactsDebug.h"

CtThemeMgr* CtThemeMgr::__instance = NULL;

CtThemeMgr* CtThemeMgr::getInstance()
{
	if( __instance == NULL )
		__instance = new CtThemeMgr();
	return __instance;
}

CtThemeMgr::CtThemeMgr()
{
	WENTER();
	__theme = elm_theme_new();
	elm_theme_ref_set(__theme, NULL);
}

CtThemeMgr::~CtThemeMgr()
{
	WHIT();
    for (auto iter = __themeList.begin(); iter != __themeList.end(); iter++) {
    	WDEBUG("theme %s", (*iter).c_str());
    	elm_theme_extension_del(__theme, (*iter).c_str());
    }

	elm_theme_free(__theme);
	__themeList.clear();
}

void CtThemeMgr::destroy()
{
	WHIT();
	delete __instance;
	__instance = NULL;
}


void CtThemeMgr::setObjectTheme(Evas_Object *obj)
{
	elm_object_theme_set(obj, __theme);
}

Elm_Theme* CtThemeMgr::getTheme()
{
	return __theme;
}

bool CtThemeMgr::addExtensionTheme(const char *themePath)
{
	WENTER();
	WPRET_VM(themePath == NULL, false, "themePath is NULL");

    for (auto iter = __themeList.begin(); iter != __themeList.end(); iter++)
    {
    	WDEBUG("themePath:%s", themePath);
    	if(strcmp(themePath, (*iter).c_str()) == 0) {
    		return false;
    	}
    }

    WDEBUG("themePath %s", themePath);
    __themeList.push_back(themePath);
    elm_theme_extension_add(__theme, themePath);

    WLEAVE();
    return true;
}



bool CtThemeMgr::removeExtensionTheme(const char *themePath)
{
	WENTER();
	WPRET_VM(themePath == NULL, false, "themePath is NULL");

    for (auto iter = __themeList.begin(); iter != __themeList.end(); iter++)
    {
    	if(strcmp(themePath, (*iter).c_str()) == 0) {
    		__themeList.erase(iter);
    		 elm_theme_extension_del(__theme, themePath);
    		 WDEBUG("themePath %s", themePath);
    		 return true;
    	}
    }

    return false;
}
