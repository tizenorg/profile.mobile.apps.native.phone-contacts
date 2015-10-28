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

#ifndef _SYSTEM_SETTINGS_MGR_H_
#define _SYSTEM_SETTINGS_MGR_H_

#include <system_settings.h>
#include <vector>

#include "WDefine.h"

/**
 * @brief provides methods for sharing configuration over a system.
 */
class WAPP_ASSIST_EXPORT SystemSettingsMgr
{
public:

	/**
	 * @brief register callback to system settings manager
	 * @param[in]   key       event key
	 * @param[in]   callback  callback function
	 * @param[in]   data      data
	 * @return error code
	 */
	int registerChangedCallback(system_settings_key_e key, system_settings_changed_cb callback, void *data);

	/**
	 * @brief unregister callback to system settings manager
	 * @param[in]   key       event key
	 * @param[in]   callback  callback function
	 * @return error code
	 */
	int unregisterChangedCallback(system_settings_key_e key, system_settings_changed_cb callback);

	/**
	 * @brief get instance of system settings manager
	 * @return system settings manager instance
	 */
	static SystemSettingsMgr& getInstance();
private:
	struct SystemSettingsCallbackEntry
	{
		system_settings_changed_cb callback;
		void *userData;
	};

	struct SystemSettingsKeyEntry
	{
		system_settings_key_e key;
		std::vector<SystemSettingsCallbackEntry> callbacksList;
	};
	std::vector<SystemSettingsKeyEntry> _systemSettingsKeyList;
	SystemSettingsMgr() {}
	SystemSettingsMgr(SystemSettingsMgr const&) = delete;
	SystemSettingsMgr& operator=( SystemSettingsMgr const& ) = delete;
	static void __callSystemCb(system_settings_key_e key, void *data);
	SystemSettingsKeyEntry *__keyEntryGet(system_settings_key_e key);
	void __keyEntryDel(system_settings_key_e key);
};

#endif /* _SYSTEM_SETTINGS_MGR_H_ */
