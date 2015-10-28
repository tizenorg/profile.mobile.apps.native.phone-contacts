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

#include "WControl.h"
#include "WView.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"

#include <Elementary.h>
#include <memory.h>

#define __CLASS_NAME	"WControl"


class __WControlImpl
{
public:
	__WControlImpl();
	~__WControlImpl();
public:
	//
	Evas_Object* __obj;
	char* __name;

	std::function<Evas_Object* (Evas_Object*, void*)> __createHandler;
	std::function<void ()> __destroyedHandler;

	std::shared_ptr<IWUiObject>* __selfPtr;
	std::weak_ptr<IWUiObject>* __popupMonitor;
	//
	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);
	void __attachPopup( WControl *popup );

};
__WControlImpl::__WControlImpl()
{
	__obj = NULL;
	__name = NULL;
	__createHandler = NULL;
	__destroyedHandler = NULL;
	__selfPtr = NULL;
	__popupMonitor = NULL;
}

__WControlImpl::~__WControlImpl()
{
	if(__name)
		free(__name);

	if( __popupMonitor )
	{
		if( auto p = __popupMonitor->lock() )
		{
			p->destroy();
		}
	}
	delete __popupMonitor;
	delete __selfPtr;
}

void __WControlImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	auto ctrl = (WControl*)data;

	ctrl->onDestroy();

	if( ctrl->__pv->__destroyedHandler )
	{
		ctrl->__pv->__destroyedHandler();
	}

	delete ctrl;
}

void __WControlImpl::__attachPopup( WControl *popup )
{
	Evas_Object* parent = NULL;
	Evas_Object* obj = __obj;
	do
	{
		WView* view = WView_getInstanceFromEvasObj( obj );
		if( view != NULL )
		{
			parent = view->getWindow()->getBaseLayoutEvasObj();
			break;
		}
	}
	while( (obj = elm_object_parent_widget_get( obj )) != NULL );

	if( parent == NULL )
	{
		WERROR("Cannot find parent View!");
		// If the pop-up has been already created, the following statement will just return without creating pop-up.
		popup->create( elm_object_top_widget_get( __obj ), NULL);
	}
	else
	{
		// If the pop-up has been already created, the following statement will just return without creating pop-up.
		popup->create( parent, NULL );
	}
}

WControl::WControl()
{
	__pv = new __WControlImpl();
}

WControl::~WControl()
{
	if( __pv->__name)
	{
		WDEBUG( "name=%s", __pv->__name );
	}
	else
	{
		WHIT();
	}
	//
	delete __pv;
}
const char* WControl::getClassName()
{
	return __CLASS_NAME;
}

bool WControl::create( Evas_Object* parent, void* param )
{
	if( __pv->__name)
		WDEBUG( "name=%s", __pv->__name );
	else
		WHIT();

	if( __pv->__obj )
	{
		WDEBUG("Already created!");
		return true;
	}

	if( __pv->__createHandler )
		__pv->__obj = __pv->__createHandler( parent, param );
	else
		__pv->__obj = onCreate( parent, param );

	if( __pv->__obj == NULL)
		return false;

	if( __pv->__name )  // for easy debugging
	{
		evas_object_name_set( __pv->__obj, __pv->__name );
	}

	evas_object_data_set( __pv->__obj, WKEY_CONTROL_INSTANCE, this );
	evas_object_event_callback_add( __pv->__obj, EVAS_CALLBACK_DEL, __WControlImpl::__objDelCb, this);
	//
	// Do not place this before "evas_object_event_callback_add",
	// The reason is that if additional del callback is added at child class, it should be called first to keep state the class object is still alive.
	onCreated();
	return true;
}

void WControl::destroy()
{
	if(__pv->__obj)
	{
		evas_object_del(__pv->__obj);
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	}
	else
	{
		onDestroy();

		if( __pv->__destroyedHandler )
		{
			__pv->__destroyedHandler();
		}

		delete this;
	}
}

Evas_Object* WControl::getEvasObj()
{
	return __pv->__obj;
}

const Evas_Object* WControl::getConstEvasObj() const
{
	return __pv->__obj;
}

void WControl::setName(const char* name)
{
	if( __pv->__name)
	{
		free( __pv->__name);
		__pv->__name = NULL;
	}
	if( name )
	{
		__pv->__name = (char*)malloc( strlen(name)+1);
		strcpy( __pv->__name, name);
	}
}

const char* WControl::getName()
{
	return __pv->__name;
}

std::weak_ptr<IWUiObject> WControl::getWeakPtr()
{
	if( __pv->__selfPtr )
	{
		return std::weak_ptr<IWUiObject>(*__pv->__selfPtr);
	}
	__pv->__selfPtr = new std::shared_ptr<IWUiObject>( this,[](IWUiObject* p){} );
	return std::weak_ptr<IWUiObject>(*__pv->__selfPtr);
}

void WControl::attachPopup( WControl* popup )
{
	__pv->__attachPopup(popup);

	// destroy old popup after new popup created
	destroyPopup();

	if( __pv->__popupMonitor == NULL)
		__pv->__popupMonitor = new std::weak_ptr<IWUiObject>;
	*__pv->__popupMonitor = popup->getWeakPtr();
}

void WControl::attachHiddenPopup( WControl *popup )
{
	__pv->__attachPopup(popup);
	evas_object_hide(popup->getEvasObj());
}

void WControl::destroyPopup()
{
	if( __pv->__popupMonitor == NULL ) return;

	if( auto p = __pv->__popupMonitor->lock() )
	{
		p->destroy();
		__pv->__popupMonitor->reset();
	}
}

void WControl::setOnCreate( const std::function<Evas_Object* (Evas_Object*, void*)>& handlerFunc )
{
	__pv->__createHandler = handlerFunc;
}

void WControl::setOnDestroy( const std::function<void ()>& handlerFunc )
{
	__pv->__destroyedHandler = handlerFunc;
}

WControl* WControl_getInstanceFromEvasObj(Evas_Object* obj)
{
	return (WControl*)evas_object_data_get(obj, WKEY_CONTROL_INSTANCE);
}

