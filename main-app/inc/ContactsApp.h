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

#ifndef _CONTACTS_APP_H_
#define _CONTACTS_APP_H_

#include "WApp.h"
#include <string>

class OperationController;

class ContactsApp : public WApp
{
public:
	ContactsApp();
	virtual ~ContactsApp();

private:
	virtual bool onCreate();
	virtual void onAppControl(app_control_h request, bool firstLaunch);
	virtual void onPause();
	virtual void onResume();
	virtual void onTerminate();
	static void onLanguageChanged(app_event_info_h event, void *data);

	OperationController *m_Controller;
};

#endif // _CONTACTS_APP_H_
