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

#include "OperationPickController.h"

#include "WApp.h"
#include "WNaviframe.h"

#include "ContactsAppControl.h"

namespace
{
	const char *resultTypes[] = {
		APP_CONTROL_DATA_TYPE_ID,
		APP_CONTROL_DATA_TYPE_PHONE,
		APP_CONTROL_DATA_TYPE_PHONE_CALL,
		APP_CONTROL_DATA_TYPE_PHONE_MESSAGE,
		APP_CONTROL_DATA_TYPE_EMAIL,
		APP_CONTROL_DATA_TYPE_VCARD
	};
}

OperationPickController::OperationPickController()
	: OperationController(OPERATION_PICK),
	  m_Naviframe(nullptr), m_Request(nullptr),
	  m_SelectMode(LIST_SELECT_SINGLE), m_ResultType(LIST_RESULT_ID)
{
}

OperationPickController::~OperationPickController()
{
	evas_object_smart_callback_del(getApplication()->getWindow()->getEvasObj(),
			"iconified", onLowered);
	m_Naviframe->setOnLastItemPop(nullptr);
	app_control_destroy(m_Request);
}

void OperationPickController::onCreate()
{
	m_Naviframe = static_cast<WNaviframe*>(getApplication()->getWindow()->getBaseUiObject());
	m_Naviframe->setOnLastItemPop([](bool *popOut) {
		ui_app_exit();
	});
}

void OperationPickController::onAppControl(Operation operation, app_control_h request)
{
	CtListView *view = NULL;
	app_control_clone(&m_Request, request);

	app_control_launch_mode_e launchMode = APP_CONTROL_LAUNCH_MODE_SINGLE;
	app_control_get_launch_mode(m_Request, &launchMode);

	if (launchMode == APP_CONTROL_LAUNCH_MODE_SINGLE) {
		evas_object_smart_callback_add(getApplication()->getWindow()->getEvasObj(),
				"iconified", onLowered, this);
	}

	m_SelectMode = getSelectMode(m_Request);
	m_ResultType = getResultType(m_Request);

	int limit = appControlGetInt(m_Request, APP_CONTROL_DATA_TOTAL_COUNT);
	view = new CtListView(m_SelectMode, m_ResultType, limit);

	using namespace std::placeholders;
	if (m_SelectMode == LIST_SELECT_SINGLE) {
		view->setOnSingleResult(std::bind(&OperationPickController::replySingleResult,
				this, _1, _2));
	} else {
		view->setOnMultiResult(std::bind(&OperationPickController::replyMultiResult,
				this, _1, _2));
	}

	m_Naviframe->push(view);
}

void OperationPickController::onLowered(void *data, Evas_Object *obj, void *event_info)
{
	OperationPickController *controller = (OperationPickController *) data;
	controller->replyFailure();
	ui_app_exit();
}

CtListSelectMode OperationPickController::getSelectMode(app_control_h request)
{
	char *selectMode = NULL;
	app_control_get_extra_data(request, APP_CONTROL_DATA_SELECTION_MODE, &selectMode);

	if (selectMode) {
		if (strcmp(selectMode, APP_CONTROL_DATA_SELECTION_MODE_SINGLE) == 0) {
			return LIST_SELECT_SINGLE;
		} else if (strcmp(selectMode, APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE) == 0) {
			return LIST_SELECT_MULTIPLE;
		}

		free(selectMode);
	}

	return LIST_SELECT_SINGLE;
}

CtListResultType OperationPickController::getResultType(app_control_h request)
{
	char *resultType = NULL;
	app_control_get_extra_data(request, APP_CONTROL_DATA_TYPE, &resultType);

	if (resultType) {
		if (strcmp(resultType, APP_CONTROL_DATA_TYPE_ID) == 0) {
			return LIST_RESULT_ID;
		} else if (strcmp(resultType, APP_CONTROL_DATA_TYPE_PHONE) == 0) {
			return LIST_RESULT_NUMBER;
		} else if (strcmp(resultType, APP_CONTROL_DATA_TYPE_EMAIL) == 0) {
			return LIST_RESULT_EMAIL;
		} else if (strcmp(resultType, APP_CONTROL_DATA_TYPE_VCARD) == 0) {
			return LIST_RESULT_VCARD;
		} else if (strcmp(resultType, APP_CONTROL_DATA_TYPE_PHONE_OR_EMAIL) == 0) {
			return LIST_RESULT_NUMBER_OR_EMAIL;
		}

		free(resultType);
	}

	return LIST_RESULT_ID;
}

void OperationPickController::replyFailure()
{
	app_control_h reply;
	app_control_create(&reply);
	app_control_reply_to_launch_request(reply, m_Request, APP_CONTROL_RESULT_FAILED);
	app_control_destroy(reply);
}

void OperationPickController::replySingleResult(const char *data, CtListResultType type)
{
	app_control_h reply = NULL;
	app_control_create(&reply);

	if (data) {
		app_control_add_extra_data(reply, APP_CONTROL_DATA_TYPE, resultTypes[type]);
		app_control_add_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &data, 1);
	}

	app_control_reply_to_launch_request(reply, m_Request, APP_CONTROL_RESULT_SUCCEEDED);
	app_control_destroy(reply);
}

void OperationPickController::replyMultiResult(const char *data[], size_t count)
{
	app_control_h reply = NULL;
	app_control_create(&reply);

	if (data) {
		app_control_add_extra_data(reply, APP_CONTROL_DATA_TYPE, resultTypes[m_ResultType]);
		app_control_add_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, data, count);
	}

	app_control_reply_to_launch_request(reply, m_Request, APP_CONTROL_RESULT_SUCCEEDED);
	app_control_destroy(reply);
}
