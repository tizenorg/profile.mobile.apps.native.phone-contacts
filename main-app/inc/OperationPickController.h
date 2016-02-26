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

#ifndef OPERATION_PICK_CONTROLLER_H
#define OPERATION_PICK_CONTROLLER_H

#include "OperationController.h"
#include "Contacts/Common/SelectMode.h"

#include <Evas.h>

namespace Ui
{
	class Navigator;
}

class OperationPickController : public OperationController
{
public:
	OperationPickController();
	virtual ~OperationPickController();

private:
	virtual void onCreate() override;
	virtual void onRequest(Operation operation, app_control_h request) override;
	bool onSelected(Contacts::SelectResults results);

	static Contacts::SelectMode getSelectMode(app_control_h request);
	static Contacts::ResultType getResultType(app_control_h request);
	static const char *getResultTypeString(Contacts::ResultType resultType);

	Ui::Navigator *m_Navigator;
	Contacts::SelectMode m_SelectMode;
	Contacts::ResultType m_ResultType;
};

#endif /* _OPERATION_PICK_CONTROLLER_H_ */
