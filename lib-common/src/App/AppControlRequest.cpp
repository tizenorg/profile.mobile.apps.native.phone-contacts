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

#include "App/AppControlRequest.h"
#include <string>

using namespace App;

AppControl App::requestContactCreate(const char *number)
{
	AppControl request(APP_CONTROL_OPERATION_ADD, APP_CONTROL_MIME_CONTACT);
	request.addExtra(APP_CONTROL_DATA_PHONE, number);
	return request;
}

AppControl App::requestContactDetails(int personId)
{
	AppControl request(APP_CONTROL_OPERATION_VIEW, APP_CONTROL_MIME_CONTACT);
	request.addExtra(APP_CONTROL_DATA_ID, std::to_string(personId).c_str());
	return request;
}

AppControl App::requestContactEdit(int personId, const char *number)
{
	AppControl request(APP_CONTROL_OPERATION_EDIT, APP_CONTROL_MIME_CONTACT);
	if (personId > 0) {
		request.addExtra(APP_CONTROL_DATA_ID, std::to_string(personId).c_str());
	}

	request.addExtra(APP_CONTROL_DATA_PHONE, number);
	return request;
}

AppControl App::requestContactPick(const char *selectionMode, const char *resultType)
{
	AppControl request(APP_CONTROL_OPERATION_PICK, APP_CONTROL_MIME_CONTACT);
	request.addExtra(APP_CONTROL_DATA_SELECTION_MODE, selectionMode);
	request.addExtra(APP_CONTROL_DATA_TYPE, resultType);
	return request;
}

AppControl App::requestTelephonyCall(const char *number)
{
	return AppControl(APP_CONTROL_OPERATION_CALL, nullptr,
			std::string("tel:").append(number).c_str());
}

AppControl App::requestCallSettings()
{
	return AppControl(APP_CONTROL_OPERATION_SETTING_CALL);
}

AppControl App::requestMessageComposer(const char *scheme, const char *to,
		const char *subject, const char *text)
{
	AppControl request(APP_CONTROL_OPERATION_COMPOSE, nullptr,
			std::string(scheme).append(to).c_str());
	request.addExtra(APP_CONTROL_DATA_TITLE, subject);
	request.addExtra(APP_CONTROL_DATA_TEXT, text);
	return request;
}
