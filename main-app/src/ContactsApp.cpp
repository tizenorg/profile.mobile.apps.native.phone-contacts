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

#include <contacts.h>

#include "WAppWindow.h"
#include "WNaviframe.h"

#include "ContactsApp.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"

#include "OperationDefaultController.h"
#include "OperationEditController.h"
#include "OperationViewController.h"
#include "OperationPickController.h"

ContactsApp::ContactsApp()
	: m_Controller(nullptr)
{
	contacts_connect();
}

ContactsApp::~ContactsApp()
{
	contacts_disconnect();
}

bool ContactsApp::onCreate()
{
	WENTER();

	ContactsCommon::loadLocalization();
	app_event_handler_h handle = nullptr;
	ui_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED, onLanguageChanged, this);

	elm_app_base_scale_set(2.6);
	attachWindow(new WAppWindow("Phone-Contacts", ELM_WIN_BASIC));
	getWindow()->attachBaseUiObject(new WNaviframe());

	WLEAVE();

	return true;
}

void ContactsApp::onAppControl(app_control_h request, bool firstLaunch)
{
	char *operationStr = nullptr;
	app_control_get_operation(request, &operationStr);

	Operation operation = OperationController::getOperation(operationStr);
	if (!m_Controller || !m_Controller->isOperationSupported(operation)) {
		delete m_Controller;
		m_Controller = nullptr;

		switch (operation) {
			case OPERATION_DEFAULT:
			case OPERATION_DIAL:
				m_Controller = new OperationDefaultController();
				break;
			case OPERATION_ADD:
			case OPERATION_EDIT:
				m_Controller = new OperationEditController();
				break;
			case OPERATION_VIEW:
				m_Controller = new OperationViewController();
				break;
			case OPERATION_PICK:
				m_Controller = new OperationPickController();
				break;
		}

		if (m_Controller) {
			m_Controller->create(this);
		}
	}

	if (m_Controller) {
		m_Controller->onAppControl(operation, request);
	}

	free(operationStr);
}

void ContactsApp::onPause()
{
	if (m_Controller) {
		m_Controller->onPause();
	}
}

void ContactsApp::onResume()
{
	if (m_Controller) {
		m_Controller->onResume();
	}
}

void ContactsApp::onTerminate()
{
	delete m_Controller;
	m_Controller = nullptr;
}

void ContactsApp::onLanguageChanged(app_event_info_h event, void *data)
{
	char *lang = nullptr;
	app_event_get_language(event, &lang);
	if (lang) {
		elm_language_set(lang);
		free(lang);
	}
}
