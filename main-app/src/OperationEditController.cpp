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

#include "OperationEditController.h"

#include "WApp.h"
#include "WNaviframe.h"

#include "ContactsAppControl.h"
#include "CtDetailView.h"
#include "CtInputView.h"

OperationEditController::OperationEditController()
	: OperationController(OPERATION_ADD | OPERATION_EDIT | OPERATION_VIEW),
	  m_Request(nullptr), m_Naviframe(nullptr)
{
}

OperationEditController::~OperationEditController()
{
	m_Naviframe->setOnLastItemPop(nullptr);
	app_control_destroy(m_Request);
}

void OperationEditController::onCreate()
{
	m_Naviframe = static_cast<WNaviframe*>(getApplication()->getWindow()->getBaseUiObject());
	m_Naviframe->setOnLastItemPop([](bool *popOut) {
		ui_app_exit();
	});
}

void OperationEditController::onAppControl(Operation operation, app_control_h request)
{
	app_control_destroy(m_Request);
	app_control_clone(&m_Request, request);

	WView *view = nullptr;
	int personId = appControlGetInt(request, APP_CONTROL_DATA_ID);

	if (personId == 0 && operation == OPERATION_EDIT) {
		using namespace std::placeholders;
		CtListView *listView = new CtListView(LIST_SELECT_SINGLE, LIST_RESULT_ID, 0);
		listView->setOnSingleResult(std::bind(&OperationEditController::onContactPick, this, _1, _2));
		view = listView;
	} else {
		view = createInputView(personId, m_Request);
	}

	if (view) {
		m_Naviframe->push(view);
	} else {
		app_control_h reply;
		app_control_create(&reply);
		app_control_reply_to_launch_request(reply, m_Request, APP_CONTROL_RESULT_FAILED);
		app_control_destroy(reply);
		ui_app_exit();
	}
}

void OperationEditController::onContactPick(const char *result, CtListResultType resultType)
{
	int personId = result ? atoi(result) : 0;
	if (personId < 1) {
		ui_app_exit();
	}

	CtInputView *view = createInputView(personId, m_Request);
	m_Naviframe->push(view);
}

CtInputView * OperationEditController::createInputView(int personId, app_control_h request)
{
	char *name = nullptr;
	char *number = nullptr;
	char *email = nullptr;
	char *url = nullptr;

	app_control_get_extra_data(request, APP_CONTROL_DATA_NAME, &name);
	app_control_get_extra_data(request, APP_CONTROL_DATA_PHONE, &number);
	app_control_get_extra_data(request, APP_CONTROL_DATA_EMAIL, &email);
	app_control_get_extra_data(request, APP_CONTROL_DATA_URL, &url);

	CtInputView *inputView = nullptr;
	if (name) {
		inputView = new CtInputView(name, number, email, url);
	} else {
		inputView = new CtInputView(personId, number, email, url);
	}

	inputView->setOnResult([request](int personId, CtResultType result) {
		replyPersonId(personId, request, app_control_result_e(result));
	});

	free(name);
	free(number);
	free(email);
	free(url);

	return inputView;
}

void OperationEditController::replyPersonId(int personId, app_control_h request, app_control_result_e result)
{
	app_control_h reply;
	app_control_create(&reply);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		char itemId[CT_TEXT_SHORT_LEN];
		snprintf(itemId, sizeof(itemId), "%d", personId);
		app_control_add_extra_data(reply, APP_CONTROL_DATA_ID, itemId);
	}

	app_control_reply_to_launch_request(reply, request, result);
	app_control_destroy(reply);
}
