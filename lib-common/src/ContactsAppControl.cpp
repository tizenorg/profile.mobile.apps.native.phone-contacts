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

#include "ContactsAppControl.h"
#include <string>

int appControlGetInt(app_control_h appControl, const char *key)
{
	int value = 0;
	char *str = nullptr;

	app_control_get_extra_data(appControl, key, &str);
	if (str) {
		value = atoi(str);
		free(str);
	}

	return value;
}

int launchContactCreate(const char *number)
{
	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_ADD);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);

	if (number) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_PHONE, number);
	}

	app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	app_control_destroy(request);

	return result;
}

int launchContactDetails(int personId)
{
	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_VIEW);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);
	app_control_add_extra_data(request, APP_CONTROL_DATA_ID,
			std::to_string(personId).c_str());

	app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	app_control_destroy(request);

	return result;
}

int launchContactEdit(int personId, const char *number)
{
	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_EDIT);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);

	if (personId > 0) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_ID,
				std::to_string(personId).c_str());
	}
	if (number) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_PHONE, number);
	}

	app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	app_control_destroy(request);

	return result;
}

int launchContactPick(const char *selectionMode, const char *resultType,
		app_control_reply_cb replyCallback, void *userData,
		bool groupMode, app_control_h *appControl)
{
	if (!selectionMode || !resultType) {
		return APP_CONTROL_ERROR_INVALID_PARAMETER;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_PICK);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);

	app_control_add_extra_data(request, APP_CONTROL_DATA_SELECTION_MODE, selectionMode);
	app_control_add_extra_data(request, APP_CONTROL_DATA_TYPE, resultType);

	if (groupMode) {
		app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	}

	int result = app_control_send_launch_request(request, replyCallback, userData);
	if (appControl) {
		*appControl = request;
	} else {
		app_control_destroy(request);
	}

	return result;
}

int launchCall(const char *number)
{
	if (!number) {
		return APP_CONTROL_ERROR_INVALID_PARAMETER;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_CALL);
	app_control_set_uri(request, std::string("tel:").append(number).c_str());

	int result = app_control_send_launch_request(request, NULL, NULL);
	app_control_destroy(request);

	return result;
}

int launchMessageComposer(const char *scheme, const char *to,
		const char *subject, const char *text, bool groupMode)
{
	if (!scheme || !to) {
		return APP_CONTROL_ERROR_INVALID_PARAMETER;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_COMPOSE);
	app_control_set_uri(request, std::string(scheme).append(to).c_str());

	if (subject) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_TITLE, subject);
	}

	if (text) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_TEXT, text);
	}

	if (groupMode) {
		app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	}

	int result = app_control_send_launch_request(request, NULL, NULL);
	app_control_destroy(request);

	return result;
}

int launchShareContact(const char *personId, bool isMyProfile)
{
	if (!personId) {
		return APP_CONTROL_ERROR_INVALID_PARAMETER;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_SHARE);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);
	app_control_add_extra_data(request, APP_CONTROL_DATA_ID, personId);
	if(isMyProfile) {
		app_control_add_extra_data(request, APP_CONTROL_DATA_TYPE, "my_profile");
	}
	app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	app_control_destroy(request);

	return result;
}

int launchMultiShareContacts(const char **personIdArray, int count)
{
	if (!(personIdArray && (count > 0))) {
		return APP_CONTROL_ERROR_INVALID_PARAMETER;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, APP_CONTROL_OPERATION_MULTI_SHARE);
	app_control_set_mime(request, APP_CONTROL_MIME_CONTACT);
	app_control_add_extra_data_array(request, APP_CONTROL_DATA_ID, personIdArray, count);

	/* FIXME: Remove this hack once share panel fixes multi share contact */
	const char *dummyPath = "/dummy.vcf";
	app_control_add_extra_data_array(request, APP_CONTROL_DATA_PATH, &dummyPath, 1);

	app_control_set_launch_mode(request, APP_CONTROL_LAUNCH_MODE_GROUP);
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	app_control_destroy(request);

	return result;
}
