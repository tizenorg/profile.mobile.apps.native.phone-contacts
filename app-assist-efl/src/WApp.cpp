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

#include "WApp.h"
#include "WDebugInternal.h"

#include "WAppImpl.h"
#include "WWindow.h"

__WAppImpl::__WAppImpl()
	:__window(NULL), __firstLaunch(true)
{ }

__WAppImpl::~__WAppImpl()
{

}

bool __WAppImpl::__onCreate( void* data)
{
	WHIT();

	auto pApp = (WApp*)data;
	return pApp->onCreate();
}

void __WAppImpl::__onTerminate( void* data)
{
	WHIT();

	auto pApp = (WApp*)data;
	pApp->onTerminate();

	// To reverse destruction order
	// It will destroy View and then Naviframe instance before the window is destroyed.
	if (pApp->__pv->__window)
		pApp->__pv->__window->destroyBaseUiObject();

	// window evas object "del" callback will be called by system.
}

void __WAppImpl::__onPause( void* data)
{
	WHIT();

	auto pApp = (WApp*)data;
	pApp->onPause();

}

void __WAppImpl::__onResume( void* data)
{
	WHIT();

	auto pApp = (WApp*)data;
	pApp->onResume();

}

void __WAppImpl::__onAppControl( app_control_h request, void* data)
{
	WHIT();

	auto pApp = (WApp*)data;
	pApp->onAppControl( request, pApp->__pv->__firstLaunch );
	//
	pApp->__pv->__firstLaunch = false;

}


WApp::WApp()
{

#ifdef _UG_DISABLE
	WDEBUG("app-assist version=%s(Ug disabled)", WAppAssist_getVersion() );
#else
	WDEBUG("app-assist version=%s", WAppAssist_getVersion() );
#endif

	__pv = new __WAppImpl();
}


WApp::WApp(__WAppImpl *impl)
	: __pv(impl)
{
}

WApp::~WApp()
{
	WHIT();
	delete __pv;
}

int WApp::start( int argc, char* argv[] )
{
	return onStart( argc, argv );
}

WWindow* WApp::getWindow()
{
	return __pv->__window;
}

bool WApp::attachWindow(WWindow* win)
{
	WASSERT( __pv->__window == NULL);
	__pv->__window = win;
	return win->create();
}

void WApp::onAppControl(app_control_h request, bool firstLaunch)
{
	Evas_Object* win = getWindow()->getEvasObj();
	if( win )
	{
		elm_win_activate( win);
		evas_object_show( win);
	}
}

int WApp::onStart( int argc, char* argv[] )
{
	ui_app_lifecycle_callback_s cb;
	memset( &cb, 0, sizeof(cb));
	cb.create = __WAppImpl::__onCreate;
	cb.terminate = __WAppImpl::__onTerminate;
	cb.pause = __WAppImpl::__onPause;
	cb.resume = __WAppImpl::__onResume;
	cb.app_control = __WAppImpl::__onAppControl;

	return ui_app_main( argc, argv, &cb, this);

}

