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

#ifndef _OPERATION_CONTROLLER_H_
#define _OPERATION_CONTROLLER_H_

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

class WApp;

/**
 * @brief Handles operations requested though App Control
 */
class OperationController
{
public:
	virtual ~OperationController() { }

	/**
	 * @bruef Create operation controller
	 * @param[in]   application     Main application instance
	 */
	void create(WApp *application);

	/**
	 * @brief Called after create() is called
	 */
	virtual void onCreate() { }

	/**
	 * @brief Called when application receives App Control request
	 */
	virtual void onAppControl(Operation operation, app_control_h request) = 0;

	/**
	 * @brief Called when application receives pause event
	 */
	virtual void onPause() { }

	/**
	 * @brief Called when application receives resume event
	 */
	virtual void onResume() { }

	/**
	 * @return Main application instance
	 */
	WApp *getApplication() const;

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
	OperationController(int supportedOperations);

private:
	int m_SupportedOperations;
	WApp *m_Application;
};

#endif /* _OPERATION_CONTROLLER_H_ */
