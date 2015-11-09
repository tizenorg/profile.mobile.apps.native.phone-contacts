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

#include "App/AppControl.h"
#include "Utils/Logger.h"

#include <utility>

using namespace App;

AppControl::AppControl()
	: m_AppControl(nullptr), m_Detached(false)
{
}

AppControl::AppControl(const char *operation, const char *mime, const char *uri)
	: AppControl()
{
	app_control_create(&m_AppControl);
	app_control_set_operation(m_AppControl, operation);
	app_control_set_mime(m_AppControl, mime);
	app_control_set_uri(m_AppControl, uri);
}

AppControl::AppControl(AppControl &&that)
{
	*this = std::move(that);
}

AppControl::~AppControl()
{
	reset();
}

app_control_h AppControl::getHandle() const
{
	return m_AppControl;
}

void AppControl::addExtra(const char *key, const char *value)
{
	app_control_add_extra_data(m_AppControl, key, value);
}

void AppControl::addExtra(const char *key, const char **array, int length)
{
	app_control_add_extra_data_array(m_AppControl, key, array, length);
}

int AppControl::launch(app_control_reply_cb replyCallback, void *userData,
		bool groupMode)
{
	m_Detached = !groupMode;
	if (groupMode) {
		app_control_set_launch_mode(m_AppControl, APP_CONTROL_LAUNCH_MODE_GROUP);
	}

	int result = app_control_send_launch_request(m_AppControl, replyCallback, userData);
	WARN_IF(result != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", result);

	return result;
}

void AppControl::terminate()
{
	app_control_send_terminate_request(m_AppControl);
}

void AppControl::detach()
{
	m_Detached = true;
}

AppControl &AppControl::operator=(AppControl &&that)
{
	reset();
	m_AppControl = that.m_AppControl;
	m_Detached = that.m_Detached;
	that.m_AppControl = nullptr;

	return *this;
}

void AppControl::reset()
{
	if (m_AppControl) {
		if (!m_Detached) {
			terminate();
		}
		app_control_destroy(m_AppControl);
	}
}
