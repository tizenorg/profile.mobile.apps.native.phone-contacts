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

#include "OperationViewController.h"

#include "WApp.h"
#include "WNaviframe.h"

#include "ContactsAppControl.h"
#include "CtDetailView.h"
#include "CtListView.h"

#include <string.h>

OperationViewController::OperationViewController()
	: OperationController(OPERATION_VIEW),
	  m_Naviframe(nullptr)
{
}

OperationViewController::~OperationViewController()
{
	m_Naviframe->setOnLastItemPop(nullptr);
}

void OperationViewController::onCreate()
{
	m_Naviframe = static_cast<WNaviframe*>(getApplication()->getWindow()->getBaseUiObject());
	m_Naviframe->setOnLastItemPop([](bool *popOut) {
		ui_app_exit();
	});
}

void OperationViewController::onAppControl(Operation operation, app_control_h request)
{
	WView *view = nullptr;
	char *mime = nullptr;
	app_control_get_mime(request, &mime);

	if (mime && strcmp(mime, APP_CONTROL_MIME_CONTACT) == 0) {
		int personId = appControlGetInt(request, APP_CONTROL_DATA_ID);
		if (personId > 0) {
			view = new CtDetailView(personId);
		}
	} else {
		char *uri = NULL;
		app_control_get_uri(request, &uri);
		if (uri) {
			view = new CtListView(uri);
			free(uri);
		}
	}

	free(mime);

	if (view) {
		m_Naviframe->push(view);
	} else {
		app_control_h reply;
		app_control_create(&reply);
		app_control_reply_to_launch_request(reply, request, APP_CONTROL_RESULT_FAILED);
		app_control_destroy(reply);
		ui_app_exit();
	}
}
