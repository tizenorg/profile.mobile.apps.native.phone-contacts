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

#include "OperationPickController.h"

#include "MainApp.h"
#include "Contacts/Common/Chooser.h"

#include "App/AppControlRequest.h"
#include "Ui/Navigator.h"

#include <vector>

#define ID_BUFFER_SIZE 16

using namespace Contacts;
using namespace Contacts::Common;
using namespace std::placeholders;

OperationPickController::OperationPickController()
	: OperationController(OperationPick),
	  m_SelectMode(SelectSingle), m_ResultType(ResultPerson)
{
}

void OperationPickController::onRequest(Operation operation, app_control_h request)
{
	m_SelectMode = getSelectMode(request);
	m_ResultType = getResultType(request);

	int limit = App::getIntExtraData(request, APP_CONTROL_DATA_TOTAL_COUNT);
	Chooser *chooser = new Chooser(m_SelectMode, m_ResultType, limit);
	chooser->setSelectCallback(std::bind(&OperationPickController::onSelected, this, _1));
	getNavigator()->navigateTo(chooser);
}

bool OperationPickController::onSelected(SelectResults results)
{
	if (m_ResultType == ResultVcard) {
		replyPath(results);
	} else {
		replyIds(results);
	}

	return true;
}

void OperationPickController::replyIds(SelectResults results)
{
	size_t count = results.count();
	std::vector<char[ID_BUFFER_SIZE]> buffers(count);
	std::vector<const char *> ids(count);

	for (size_t i = 0; i < count; ++i) {
		snprintf(buffers[i], sizeof(buffers[i]), "%d", results[i].value.id);
		ids[i] = buffers[i];
	}

	const char *type = (m_ResultType == ResultAction)
			? getActionTypeString(ActionType(results.begin()->type))
			: getResultTypeString(m_ResultType);
	replyResults(type, ids.data(), ids.size());
}

void OperationPickController::replyPath(SelectResults results)
{
	const char *type = getResultTypeString(m_ResultType);
	const char *result = (const char *) results.begin()->value.data;
	replyResults(type, &result, 1);
}

void OperationPickController::replyResults(const char *resultType, const char **results, size_t count)
{
	app_control_h reply = nullptr;
	app_control_create(&reply);

	app_control_add_extra_data(reply, APP_CONTROL_DATA_TYPE, resultType);
	app_control_add_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, results, count);

	app_control_reply_to_launch_request(reply, getRequest(), APP_CONTROL_RESULT_SUCCEEDED);
	app_control_destroy(reply);
}

SelectMode OperationPickController::getSelectMode(app_control_h request)
{
	char *selectMode = nullptr;
	app_control_get_extra_data(request, APP_CONTROL_DATA_SELECTION_MODE, &selectMode);

	if (selectMode) {
		if (strcmp(selectMode, APP_CONTROL_SELECT_SINGLE) == 0) {
			return SelectSingle;
		} else if (strcmp(selectMode, APP_CONTROL_SELECT_MULTIPLE) == 0) {
			return SelectMulti;
		}

		free(selectMode);
	}

	return SelectSingle;
}

ResultType OperationPickController::getResultType(app_control_h request)
{
	char *resultType = nullptr;
	app_control_get_extra_data(request, APP_CONTROL_DATA_TYPE, &resultType);

	if (resultType) {
		if (strcmp(resultType, APP_CONTROL_RESULT_ID) == 0) {
			return ResultPerson;
		} else if (strcmp(resultType, APP_CONTROL_RESULT_PHONE) == 0) {
			return ResultNumber;
		} else if (strcmp(resultType, APP_CONTROL_RESULT_EMAIL) == 0) {
			return ResultEmail;
		} else if (strcmp(resultType, APP_CONTROL_RESULT_VCARD) == 0) {
			return ResultVcard;
		} else if (strcmp(resultType, APP_CONTROL_RESULT_ACTION) == 0) {
			return ResultAction;
		}

		free(resultType);
	}

	return ResultPerson;
}

const char *OperationPickController::getResultTypeString(ResultType resultType)
{
	switch (resultType) {
		case ResultMyProfile:
		case ResultPerson:
			return APP_CONTROL_RESULT_ID;
		case ResultNumber:
			return APP_CONTROL_RESULT_PHONE;
		case ResultEmail:
			return APP_CONTROL_RESULT_EMAIL;
		case ResultVcard:
			return APP_CONTROL_RESULT_VCARD;
		case ResultAction:
			return APP_CONTROL_RESULT_ACTION;
	}

	return APP_CONTROL_RESULT_ID;
}

const char *OperationPickController::getActionTypeString(ActionType actionType)
{
	switch (actionType) {
		case ActionCall:
			return APP_CONTROL_RESULT_CALL;
		case ActionMessage:
			return APP_CONTROL_RESULT_MESSAGE;
		case ActionEmail:
			return APP_CONTROL_RESULT_EMAIL;
	}

	return APP_CONTROL_RESULT_CALL;
}
