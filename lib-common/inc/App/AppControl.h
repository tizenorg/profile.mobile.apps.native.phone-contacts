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

#ifndef APP_APP_CONTROL_H
#define APP_APP_CONTROL_H

#include <app_control.h>

namespace App
{
	class EXPORT_API AppControl
	{
	public:
		/**
		 * @brief Create empty app control wrapper. The wrapper doesn't contain any
		 *        value it can only be assigned to.
		 */
		AppControl();

		/**
		 * @brief Create app control request
		 *
		 * @see app_control_set_operation()
		 * @see app_control_set_mime()
		 * @see app_control_set_uri()
		 */
		AppControl(const char *operation,
				const char *mime = nullptr,
				const char *uri = nullptr);

		AppControl(const AppControl&) = delete;
		AppControl(AppControl&&);
		~AppControl();

		/**
		 * @return Stored app control handle
		 */
		app_control_h getHandle() const;

		/**
		 * @see app_control_add_extra_data()
		 */
		void addExtra(const char *key, const char *value);

		/**
		 * @see app_control_add_extra_data_array()
		 */
		void addExtra(const char *key, const char **array, int length);

		/**
		 * @brief Launch app control request
		 * @return App Control launch result
		 *
		 * @see app_control_set_launch_mode()
		 * @see app_control_send_launch_request()
		 */
		int launch(app_control_reply_cb replyCallback = nullptr, void *userData = nullptr,
				bool groupMode = true);

		/**
		 * @see app_control_send_terminate_request()
		 */
		void terminate();

		/**
		 * @brief Prevent app control termination on destruction.
		 */
		void detach();

		AppControl &operator=(const AppControl&) = delete;
		AppControl &operator=(AppControl&&);

	private:
		void reset();

		app_control_h m_AppControl;
		bool m_Detached;
	};
}

#endif /* APP_APP_CONTROL_H */
