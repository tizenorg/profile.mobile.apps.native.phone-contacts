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

#include "WAppEventHandler.h"
#include <app.h>
#include "WDebugInternal.h"
#include "WDefineInternal.h"


class __WAppEventHandlerImpl
{
public:
	__WAppEventHandlerImpl();
	~__WAppEventHandlerImpl();
public:
	//
	app_event_handler_h __handle;
	//
	void* __userData;
	std::function<void (app_event_info_h, void*)> __eventHandlerFunc;

	static void __eventCb( app_event_info_h eventInfo, void* userData);
	void __removeEventHandler();
};

__WAppEventHandlerImpl::__WAppEventHandlerImpl():
	__handle(NULL),
	__userData(NULL)
{
}

__WAppEventHandlerImpl::~__WAppEventHandlerImpl()
{
	__removeEventHandler();
}


void __WAppEventHandlerImpl::__eventCb( app_event_info_h eventInfo, void* userData)
{
	auto p = (__WAppEventHandlerImpl*)userData;
	p->__eventHandlerFunc( eventInfo, p->__userData );
}

void __WAppEventHandlerImpl::__removeEventHandler()
{
	if( __handle )
	{
		ui_app_remove_event_handler( __handle );
		__handle = NULL;
		__userData = NULL;

	}
}

WAppEventHandler::WAppEventHandler()
{
	__pv = new __WAppEventHandlerImpl();
}

WAppEventHandler::~WAppEventHandler()
{
	delete __pv;
}


int WAppEventHandler::addEventHandler( app_event_type_e eventType, const std::function<void (app_event_info_h, void*)>& handlerFunc, void* userData )
{
	removeEventHandler();

	__pv->__eventHandlerFunc = handlerFunc;
	__pv->__userData = userData;

	return ui_app_add_event_handler( &__pv->__handle, eventType, __pv->__eventCb, __pv );
}

void WAppEventHandler::removeEventHandler()
{
	__pv->__removeEventHandler();
}

app_event_handler_h WAppEventHandler::getHandle()
{
	return __pv->__handle;
}


