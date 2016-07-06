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

#include "OperationController.h"
#include "Ui/Navigator.h"
#include "Ui/Window.h"

#include <app.h>
#include <string.h>

OperationController::OperationController(int supportedOperations, bool isMinimizable)
	: m_SupportedOperations(supportedOperations), m_Request(nullptr), m_IsMinimizable(isMinimizable),
	  m_Window(nullptr), m_Navigator(nullptr)
{
}

OperationController::~OperationController()
{
	app_control_destroy(m_Request);

	if (!m_IsMinimizable) {
		m_Navigator->unsetLastPageCallback();
	}
}

void OperationController::create(Ui::Window *window, Ui::Navigator *navigator)
{
	m_Window = window;
	m_Navigator = navigator;

	if (!m_IsMinimizable) {
		m_Navigator->setLastPageCallback([] {
			ui_app_exit();
			return true;
		});
	}

	onCreate();
}

void OperationController::request(Operation operation, app_control_h request)
{
	app_control_destroy(m_Request);
	app_control_clone(&m_Request, request);
	onRequest(operation, m_Request);
}

bool OperationController::isOperationSupported(Operation operation) const
{
	return (m_SupportedOperations & operation) != 0;
}

Operation OperationController::getOperation(const char *operation)
{
	if (!operation) {
		return OperationDefault;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_DEFAULT) == 0) {
		return OperationDefault;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_DIAL) == 0) {
		return OperationDial;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_ADD) == 0) {
		return OperationAdd;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_EDIT) == 0) {
		return OperationEdit;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_VIEW) == 0) {
		return OperationView;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_PICK) == 0) {
		return OperationPick;
	}

	return OperationDefault;
}

Ui::Window *OperationController::getWindow() const
{
	return m_Window;
}

Ui::Navigator *OperationController::getNavigator() const
{
	return m_Navigator;
}

app_control_h OperationController::getRequest() const
{
	return m_Request;
}

std::string OperationController::getUrn(const char *scheme) const
{
	char *uri = nullptr;
	app_control_get_uri(m_Request, &uri);
	if (!uri) {
		return "";
	}

	const char *urn = nullptr;
	if (scheme) {
		size_t length = strlen(scheme);
		if (strncmp(uri, scheme, length) == 0) {
			urn = uri + length;
		}
	}

	std::string path = urn ? urn : uri;
	free(uri);
	return path;
}

void OperationController::replyFailure()
{
	app_control_h reply;
	app_control_create(&reply);
	app_control_reply_to_launch_request(reply, m_Request, APP_CONTROL_RESULT_FAILED);
	app_control_destroy(reply);
}
