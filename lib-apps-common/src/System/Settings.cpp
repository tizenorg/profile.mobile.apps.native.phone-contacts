/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "System/Settings.h"
#include "Utils/Callback.h"

#include <list>

using namespace System::Settings;

namespace
{
	class KeyManager : public CallbackManager
	{
	public:
		explicit KeyManager(system_settings_key_e key)
			: m_Key(key)
		{
			system_settings_set_changed_cb(m_Key,
					makeCallbackWithLastParam(&KeyManager::invokeCallbacks), this);
		}

		~KeyManager()
		{
			system_settings_unset_changed_cb(m_Key);
		}

		bool operator==(system_settings_key_e key) const
		{
			return m_Key == key;
		}

	private:
		system_settings_key_e m_Key;
	};

	std::list<KeyManager> keyManagers;
}

void System::Settings::addCallback(system_settings_key_e key, KeyManager::Callback callback)
{
	auto manager = std::find(keyManagers.begin(), keyManagers.end(), key);
	if (manager == keyManagers.end()) {
		keyManagers.emplace_back(key);
		manager = --keyManagers.end();
	}

	manager->addCallback(callback);
}

void System::Settings::removeCallback(system_settings_key_e key, KeyManager::Callback callback)
{
	auto manager = std::find(keyManagers.begin(), keyManagers.end(), key);
	if (manager == keyManagers.end()) {
		return;
	}

	manager->removeCallback(callback);
	if (manager->isEmpty()) {
		keyManagers.erase(manager);
	}
}
