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
#include "Utils/Logger.h"

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

AppControl App::requestCameraImage()
{
	return AppControl(APP_CONTROL_OPERATION_CREATE_CONTENT, "image/*");
}

AppControl App::requestGalleryImage()
{
	AppControl request(APP_CONTROL_OPERATION_PICK, "image/*");
	app_control_set_app_id(request.getHandle(), "org.tizen.gallery");
	return request;
}

AppControl App::requestShareContact(int personId)
{
	AppControl request(APP_CONTROL_OPERATION_SHARE, APP_CONTROL_MIME_CONTACT);
	request.addExtra(APP_CONTROL_DATA_ID, std::to_string(personId).c_str());
	return request;
}

AppControl App::requestShareMyProfile(int recordId)
{
	AppControl request = requestShareContact(recordId);
	request.addExtra(APP_CONTROL_DATA_TYPE, "my_profile");
	return request;
}

AppControl App::requestMultiShareContacts(const char **personIds, int count)
{
	AppControl request(APP_CONTROL_OPERATION_MULTI_SHARE, APP_CONTROL_MIME_CONTACT);
	request.addExtra(APP_CONTROL_DATA_ID, personIds, count);
	return request;
}

AppControl App::requestPickVcard(const char *path)
{
	//TODO: Replace it with AppControl which will not use application id.
	AppControl request(nullptr);
	request.addExtra("path", path);
	request.addExtra("select_type", "IMPORT_PATH_SELECT");
	request.addExtra("file_type", "vcf");
	app_control_set_app_id(request.getHandle(), "ug-myfile-efl");

	return request;
}

std::string App::getSingleExtraData(app_control_h appControl, const char *key)
{
	std::string result;
	char **array = nullptr;
	int count = 0;

	int err = app_control_get_extra_data_array(appControl, key, &array, &count);
	RETVM_IF_ERR(err, result, "app_control_get_extra_data_array() failed.");

	if (array && array[0]) {
		result = array[0];
	}

	for (int i = 0; i < count; ++i) {
		free(array[i]);
	}
	free(array);

	return result;
}

int App::getIntExtraData(app_control_h appControl, const char *key)
{
	int result = 0;
	char *resultStr = nullptr;

	int err = app_control_get_extra_data(appControl, key, &resultStr);
	RETVM_IF_ERR(err, result, "app_control_get_extra_data_array() failed.");

	if (resultStr) {
		result = atoi(resultStr);
		free(resultStr);
	}

	return result;
}

std::vector<int> App::getIntExtraDataArray(app_control_h appControl, const char *key)
{
	std::vector<int> result;
	char **array = nullptr;
	int count = 0;

	int err = app_control_get_extra_data_array(appControl, key, &array, &count);
	RETVM_IF_ERR(err, result, "app_control_get_extra_data_array() failed.");
	result.reserve(count);

	for (int i = 0; i < count; ++i) {
		int value = atoi(array[i]);
		result.push_back(value);
		free(array[i]);
	}
	free(array);

	return result;
}
