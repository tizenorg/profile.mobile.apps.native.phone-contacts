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

#include <app_preference.h>

#include "WDebugBase.h"
#include "CtType.h"

class PreferenceInitializer
{
private:
	static void initializeString(const char *key, const char *defaultValue)
	{
		bool keyExists = false;

		preference_is_existing(key, &keyExists);
		if(!keyExists)
		{
			preference_set_string(key, defaultValue);
		}
	}

	static void initializeInt(const char *key, int defaultValue)
	{
		bool keyExists = false;

		preference_is_existing(key, &keyExists);
		if(!keyExists)
		{
			preference_set_int(key, defaultValue);
		}
	}

	static void initializeBool(const char *key, bool defaultValue)
	{
		bool keyExists = false;

		preference_is_existing(key, &keyExists);
		if(!keyExists)
		{
			preference_set_boolean(key, defaultValue);
		}
	}

	static bool preferenceInitialization();

	static bool initializationVariable;
};

bool PreferenceInitializer::initializationVariable = PreferenceInitializer::preferenceInitialization();

bool PreferenceInitializer::preferenceInitialization()
{
	WHIT();

	initializeString(CT_KEY_LIST_DISPLAY_ADDRESSBOOK, "");
	initializeString(CT_KEY_LIST_MFC_PERSON_IDS, "");
	initializeString(CT_KEY_INPUT_BASE_ADDRESSBOOK, "http://tizen.org/addressbook/phone");

	initializeInt(CT_KEY_DISPLAY_WITH_NUMBER, 0);
	initializeInt(CT_KEY_LAST_SELECTED_TAB_INDEX_FOR_PHONE, 0);
	initializeInt(CT_KEY_NUM_OF_LINKABLE_CONTACTS, 0);
	initializeInt(CT_KEY_SETTING_CONTACT_SHARING_SETTINGS, 0);

	initializeBool(CT_KEY_CONTACT_CREATE_FIRST, true);
	initializeBool(CT_KEY_MYPROFILE_CREATE_FIRST, true);
	initializeBool(CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP, false);

	return true;
}

