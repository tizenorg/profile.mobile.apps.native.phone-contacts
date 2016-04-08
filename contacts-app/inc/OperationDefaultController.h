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

#ifndef OPERATION_DEFAULT_CONTROLLER_H
#define OPERATION_DEFAULT_CONTROLLER_H

#include "OperationController.h"
#include <string>

namespace Ui
{
	class View;
	class Navigator;
}

class OperationDefaultController : public OperationController
{
public:
	OperationDefaultController();
	virtual ~OperationDefaultController() override;

private:
	enum TabId
	{
		TabFirst,
		TabDialer = TabFirst,
		TabLogs,
		TabContacts,
		TabMax
	};

	virtual void onCreate() override;
	virtual void onRequest(Operation operation, app_control_h request) override;

	static std::string getPhoneNumber(app_control_h appControl);
	static unsigned getBadgeCount(const char *package);

	void saveLastTab();
	static TabId getLastTab();

	Ui::Navigator *m_Navigator;
	Ui::View *m_Tabs[TabMax];
};

#endif /* OPERATION_DEFAULT_CONTROLLER_H */
