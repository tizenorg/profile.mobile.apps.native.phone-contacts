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

#ifndef _OPERATION_DEFAULT_CONTROLLER_H_
#define _OPERATION_DEFAULT_CONTROLLER_H_

#include "OperationController.h"
#include <string>

class CtTabView;
class PhDialerView;

class OperationDefaultController : public OperationController
{
public:
	OperationDefaultController();

private:
	virtual void onCreate();
	virtual void onAppControl(Operation operation, app_control_h request);
	CtTabView *createTabView();

	static std::string getPhoneNumber(app_control_h appControl);
	static unsigned getBadgeCount(const char *package);
	static int getLastTab();

	CtTabView *m_TabView;
	PhDialerView *m_DialerView;
};

#endif /* _OPERATION_DEFAULT_CONTROLLER_H_ */
