/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include <system_settings.h>
#include "Utils/CallbackManager.h"

namespace System
{
	namespace Settings
	{
		typedef Utils::CallbackManager<system_settings_key_e> CallbackManager;

		/**
		 * @see system_settings_set_changed_cb()
		 */
		EXPORT_API void addCallback(system_settings_key_e key, CallbackManager::Callback callback);

		/**
		 * @see system_settings_unset_changed_cb()
		 */
		EXPORT_API void removeCallback(system_settings_key_e key, CallbackManager::Callback callback);
	};
}

#endif /* SYSTEM_SETTINGS_H */
