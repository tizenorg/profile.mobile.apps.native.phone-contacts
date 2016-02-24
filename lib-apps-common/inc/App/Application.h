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

#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include <app.h>

namespace App
{
	/**
	 * @brief Application lifecycle controller.
	 */
	class EXPORT_API Application
	{
	public:
		/**
		 * @brief Run application main loop.
		 * @param[in]   argc    argc parameter received in main()
		 * @param[in]   argv    argv parameter received in main()
		 */
		int run(int argc, char **argv);

	protected:
		virtual ~Application() { }

		/**
		 * @brief Called when application is created
		 * @return true on success, otherwise false
		 */
		virtual bool onCreate() { return true; }

		/**
		 * @brief Called when application is terminated
		 */
		virtual void onTerminate() { }

		/**
		 * @brief Called when application is paused
		 */
		virtual void onPause() { }

		/**
		 * @brief Called when application is resumed
		 */
		virtual void onResume() { }

		/**
		 * @brief Called when application receives App Control request
		 * @param   request     App Control request
		 */
		virtual void onAppControl(app_control_h request) { }
	};
}

#endif /* APP_APPLICATION_H */
