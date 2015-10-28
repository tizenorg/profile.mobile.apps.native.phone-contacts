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

#include "SystemSettingsMgr.h"
#include "ContactsDebug.h"

void SystemSettingsMgr::__callSystemCb(system_settings_key_e key, void *data)
{
	SystemSettingsKeyEntry *key_entry = getInstance().__keyEntryGet(key);
	for(auto &&callback_entry : key_entry->callbacksList)
	{
		callback_entry.callback(key, callback_entry.userData);
	}
}

int SystemSettingsMgr::registerChangedCallback(system_settings_key_e key, system_settings_changed_cb callback, void *data)
{
	if(NULL == callback)
	{
		return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
	}

	SystemSettingsKeyEntry *keyEntry = __keyEntryGet(key);
	SystemSettingsCallbackEntry callbackEntry = {callback, data};
	if (NULL == keyEntry )
	{
		SystemSettingsKeyEntry newKeyEntry;
		newKeyEntry.key = key;
		newKeyEntry.callbacksList.push_back(callbackEntry);
		_systemSettingsKeyList.push_back(newKeyEntry);
		system_settings_set_changed_cb(key, __callSystemCb, this);
		return SYSTEM_SETTINGS_ERROR_NONE;
	}

	keyEntry->callbacksList.push_back(callbackEntry);
	return SYSTEM_SETTINGS_ERROR_NONE;
}

int SystemSettingsMgr::unregisterChangedCallback(system_settings_key_e key, system_settings_changed_cb callback)
{
	if(NULL == callback)
	{
		return SYSTEM_SETTINGS_ERROR_INVALID_PARAMETER;
	}

	SystemSettingsKeyEntry *keyEntry = __keyEntryGet(key);
	if (NULL == keyEntry || keyEntry->callbacksList.empty())
	{
		  return SYSTEM_SETTINGS_ERROR_NONE;
	}

	int callback_list_length = keyEntry->callbacksList.size();
	for(int i = 0; i < callback_list_length; i++)
	{
		SystemSettingsCallbackEntry &callbackEntry = keyEntry->callbacksList[i];
		if (callbackEntry.callback == callback)
		{
			keyEntry->callbacksList.erase(keyEntry->callbacksList.begin() + i);
			break;
		}
	}

	if (keyEntry->callbacksList.empty())
	{
		int ret = system_settings_unset_changed_cb(key);
		WPWARN(-1 == ret, "system_settings_unset_changed_cb(%i) Failed", key);
		__keyEntryDel(key);
	}

	return SYSTEM_SETTINGS_ERROR_NONE;
}

SystemSettingsMgr& SystemSettingsMgr::getInstance()
{
	static SystemSettingsMgr instance;
	return instance;
}

SystemSettingsMgr::SystemSettingsKeyEntry *SystemSettingsMgr::__keyEntryGet(system_settings_key_e key)
{
	int list_length = _systemSettingsKeyList.size();
	for(int i = 0; i < list_length; i++)
	{
		if(_systemSettingsKeyList[i].key == key)
		{
			return &_systemSettingsKeyList[i];
		}
	}
	return NULL;
}

void SystemSettingsMgr::__keyEntryDel(system_settings_key_e key)
{
	int list_length = _systemSettingsKeyList.size();
	for(int i = 0; i < list_length; i++)
	{
		if(_systemSettingsKeyList[i].key == key)
		{
			_systemSettingsKeyList.erase(_systemSettingsKeyList.begin() + i);
			break;
		}
	}
}
