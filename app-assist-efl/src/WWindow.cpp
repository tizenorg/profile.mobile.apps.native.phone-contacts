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

#include "WWindow.h"
#include "WWindowImpl.h"
#include "IWUiObject.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"

#define __CLASS_NAME	"WWindow"

__WWindowImpl::__WWindowImpl()
	: __win(NULL),
	  __baseLayout(NULL),
	  __conformant(NULL),
	  __mainObj(NULL),
	  __baseObj(NULL)
{
}

__WWindowImpl::~__WWindowImpl()
{
}

void __WWindowImpl::__delReqCb( void* data, Evas_Object* obj, void* event_info)
{
	WHIT();
	elm_exit();
}

void __WWindowImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WHIT();

	auto win = (WWindow*)data;

	win->onDestroy();
	delete win;
}

WWindow::WWindow(__WWindowImpl *impl)
{
	__pv = impl;
}

WWindow::~WWindow()
{
	WHIT();
	delete __pv;
}

bool WWindow::create()
{
	WHIT();
	if (__pv->__win)
	{
		WDEBUG("Already created!");
		return true;
	}

	Evas_Object *win = NULL;
	Evas_Object *conf = NULL;
	Evas_Object *layout = NULL;

	Evas_Object *mainObj = doCreate(win, conf, layout);
	if (mainObj)
	{
		evas_object_data_set(mainObj, WKEY_WINDOW_INSTANCE, this);
		// When this callback function is called???????
		evas_object_smart_callback_add(mainObj, "delete,request", __WWindowImpl::__delReqCb, this);
		evas_object_event_callback_add(mainObj, EVAS_CALLBACK_DEL, __WWindowImpl::__objDelCb, this);

		evas_object_show(win);

		__pv->__win = win;
		__pv->__conformant = conf;
		__pv->__baseLayout = layout;
		__pv->__mainObj = mainObj;

		return true;
	}
	else
	{
		return false;
	}
}

void WWindow::destroy()
{
	if (__pv->__mainObj)
	{
		evas_object_del(__pv->__mainObj);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!

	}
	else
	{
		onDestroy();
		delete this;
	}
}

const char* WWindow::getClassName() const
{
	return __CLASS_NAME;
}

Evas_Object* WWindow::getEvasObj() const
{
	return __pv->__win;
}

Evas_Object* WWindow::getBaseLayoutEvasObj() const
{
	return __pv->__baseLayout;
}

Evas_Object* WWindow::getConformantEvasObj() const
{
	return __pv->__conformant;
}

bool WWindow::attachBaseUiObject( IWUiObject* baseObj, void* creationParam )
{

	WASSERT( __pv->__baseObj == NULL);

	if( baseObj->create( __pv->__baseLayout, creationParam) == false )
	{
		return false;
	}

	__pv->__baseObj = baseObj;

	evas_object_data_set( baseObj->getEvasObj(), WKEY_WINDOW_INSTANCE, this );

	return true;
}

IWUiObject* WWindow::getBaseUiObject() const
{
	return __pv->__baseObj;
}

IWUiObject* WWindow::detachBaseUiObject()
{
	IWUiObject* obj = __pv->__baseObj;
	__pv->__baseObj = NULL;
	return obj;
}

void WWindow::destroyBaseUiObject()
{
	if( __pv->__baseObj )
		__pv->__baseObj->destroy();

	__pv->__baseObj = NULL;
}

void WWindow::onDestroy()
{

}
