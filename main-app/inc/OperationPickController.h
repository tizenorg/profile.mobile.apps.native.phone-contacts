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

#ifndef _OPERATION_PICK_CONTROLLER_H_
#define _OPERATION_PICK_CONTROLLER_H_

#include "OperationController.h"
#include "CtListView.h"

class WNaviframe;

class OperationPickController : public OperationController
{
public:
	OperationPickController();
	virtual ~OperationPickController();

private:
	virtual void onCreate();
	virtual void onAppControl(Operation operation, app_control_h request);
	static  void onLowered(void *data, Evas_Object *obj, void *event_info);

	static CtListSelectMode getSelectMode(app_control_h request);
	static CtListResultType getResultType(app_control_h request);

	void replyFailure();
	void replySingleResult(const char *data, CtListResultType type);
	void replyMultiResult(const char **data, size_t count);

	WNaviframe *m_Naviframe;
	app_control_h m_Request;
	CtListSelectMode m_SelectMode;
	CtListResultType m_ResultType;
};

#endif /* _OPERATION_PICK_CONTROLLER_H_ */
