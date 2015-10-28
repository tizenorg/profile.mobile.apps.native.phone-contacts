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

#include "OperationController.h"
#include "ContactsAppControl.h"
#include <string.h>

OperationController::OperationController(int supportedOperations)
	: m_SupportedOperations(supportedOperations), m_Application(nullptr)
{
}

void OperationController::create(WApp *application)
{
	m_Application = application;
	onCreate();
}

WApp *OperationController::getApplication() const
{
	return m_Application;
}

bool OperationController::isOperationSupported(Operation operation) const
{
	return (m_SupportedOperations & operation) != 0;
}

Operation OperationController::getOperation(const char *operation)
{
	if (!operation) {
		return OPERATION_DEFAULT;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_DEFAULT) == 0) {
		return OPERATION_DEFAULT;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_DIAL) == 0) {
		return OPERATION_DIAL;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_ADD) == 0) {
		return OPERATION_ADD;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_EDIT) == 0) {
		return OPERATION_EDIT;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_VIEW) == 0) {
		return OPERATION_VIEW;
	}
	if (strcmp(operation, APP_CONTROL_OPERATION_PICK) == 0) {
		return OPERATION_PICK;
	}

	return OPERATION_DEFAULT;
}
