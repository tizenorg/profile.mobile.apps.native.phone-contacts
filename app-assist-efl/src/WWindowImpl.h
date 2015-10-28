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

#ifndef _WWINDOW_IMPL_H_
#define _WWINDOW_IMPL_H_

#include <Elementary.h>

class IWUiObject;
class __WWindowImpl
{
public:
	__WWindowImpl();
	virtual ~__WWindowImpl();

public:
	//
	Evas_Object* __win;
	Evas_Object* __baseLayout;
	Evas_Object* __conformant;
	Evas_Object* __mainObj;
	IWUiObject* __baseObj;

	static void __delReqCb( void* data, Evas_Object* obj, void* event_info);
	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);
};

#endif
