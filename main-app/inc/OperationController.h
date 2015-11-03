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

#ifndef OPERATION_CONTROLLER_H
#define OPERATION_CONTROLLER_H

#include <app_control.h>

enum Operation
{
	OPERATION_DEFAULT = 1 << 0,
	OPERATION_DIAL    = 1 << 1,
	OPERATION_ADD     = 1 << 2,
	OPERATION_EDIT    = 1 << 3,
	OPERATION_VIEW    = 1 << 4,
	OPERATION_PICK    = 1 << 5
};

class MainApp;

/**
 * @brief Handles operations requested though App Control
 */
class OperationController
{
public:
	virtual ~OperationController();

	/**
	 * @brief Create operation controller
	 * @param[in]   application     Main application instance
	 */
	void create(MainApp *application);

	/**
	 * @brief Request the controller to handle the operation
	 */
	void request(Operation operation, app_control_h request);

	/**
	 * @brief Called after create() is called
	 */
	virtual void onCreate() { }

	/**
	 * @brief Called when application receives pause event
	 */
	virtual void onPause() { }

	/**
	 * @brief Called when application receives resume event
	 */
	virtual void onResume() { }

	/**
	 * @brief Check if operation is supported by the controller
	 */
	bool isOperationSupported(Operation operation) const;

	/**
	 * @brief Get operation from string representation
	 * @param[in]   operation   Operation string representation
	 * @return Operation value
	 */
	static Operation getOperation(const char *operation);

protected:
	/**
	 * @brief Create operation controller
	 * @param[in]   supportedOperations     Supported operations mask
	 * @see Operation
	 */
	OperationController(int supportedOperations);

	/**
	 * @return Main application instance
	 */
	MainApp *getApplication() const;

	/**
	 * @return Last received request
	 */
	app_control_h getRequest() const;

	/**
	 * @brief Send failure reply to the last request
	 */
	void replyFailure();

	/**
	 * @brief Called when application receives App Control request
	 */
	virtual void onRequest(Operation operation, app_control_h request) = 0;

private:
	int m_SupportedOperations;
	MainApp *m_Application;
	app_control_h m_Request;
};

#endif /* OPERATION_CONTROLLER_H */
