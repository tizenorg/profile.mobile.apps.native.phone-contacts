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

#ifndef _WAPP_IMPL_H_
#define _WAPP_IMPL_H_

#include <app_control.h>

class WWindow;

class __WAppImpl
{
public:
	__WAppImpl();
	~__WAppImpl();

public:
	WWindow *__window;
	bool __firstLaunch;

	static bool __onCreate(void *data);
	static void __onTerminate(void *data);
	static void __onPause(void *data);
	static void __onResume(void *data);
	static void __onAppControl(app_control_h request, void *data);
};

#endif /* _WAPP_IMPL_H_ */
