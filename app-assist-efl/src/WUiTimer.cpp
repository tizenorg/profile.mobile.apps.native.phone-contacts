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

#include "WUiTimer.h"
#include <Ecore.h>
#include "WDebugInternal.h"
#include "WDefineInternal.h"

class __WUiTimerImpl
{
public:
	__WUiTimerImpl();
	~__WUiTimerImpl();
public:
	//
	void* __timer;
	WUiTimer::TimerType __type;

	void* __userData;
	std::weak_ptr<IWUiObject>* __uiObjMonitor;

	std::function<Eina_Bool (void*)> __timerFunc;
	std::function<void (void*)> __jobFunc;
	//
	void __deleteTimer();
	static Eina_Bool __timerCb(void* data);
	static void __jobCb(void* data);

};
__WUiTimerImpl::__WUiTimerImpl()
{
	__timer = NULL;
	__type = WUiTimer::IDLER;
	__userData = NULL;
	__uiObjMonitor = NULL;
}

__WUiTimerImpl::~__WUiTimerImpl()
{
	__deleteTimer();
	delete __uiObjMonitor;

}

void __WUiTimerImpl::__deleteTimer()
{
	if( __timer )
	{
		switch( __type )
		{
		case WUiTimer::IDLER:
			ecore_idler_del( (Ecore_Idler*)__timer);
			break;
		case WUiTimer::ANIMATOR:
			ecore_animator_del( (Ecore_Animator*)__timer);
			break;
		case WUiTimer::TIMER:
			ecore_timer_del( (Ecore_Timer*)__timer);
			break;
		case WUiTimer::JOB:
			ecore_job_del( (Ecore_Job*)__timer);
			break;
		default:
			break;
		}
		__timer = NULL;
	}
}

Eina_Bool __WUiTimerImpl::__timerCb( void* data )
{
	auto p = (WUiTimer*)data;

	if( p->__pv->__uiObjMonitor && p->__pv->__uiObjMonitor->expired() )
	{
		p->__pv->__timer = NULL;
		delete p;
		return ECORE_CALLBACK_CANCEL;
	}
	Eina_Bool ret = p->__pv->__timerFunc( p->__pv->__userData );
	if( ret == ECORE_CALLBACK_CANCEL )
	{
		p->__pv->__timer = NULL;
		delete p;
	}
	return ret;
}

void __WUiTimerImpl::__jobCb( void* data )
{
	auto p = (WUiTimer*)data;

	if( p->__pv->__uiObjMonitor && p->__pv->__uiObjMonitor->expired())
	{
		p->__pv->__timer = NULL;
		delete p;
		return;
	}
	p->__pv->__jobFunc( p->__pv->__userData );
	p->__pv->__timer = NULL;
	delete p;
}

WUiTimer::WUiTimer()
{
	WHIT();
	__pv = new __WUiTimerImpl();
}

WUiTimer::~WUiTimer()
{
	WHIT();
	delete __pv;
}

WUiTimer::TimerType WUiTimer::getType()
{
	return __pv->__type;
}

void WUiTimer::destroy()
{
	delete this;
}

WUiTimer* WUiTimer::addIdler( const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj )
{
	WUiTimer* timer = new WUiTimer();
	timer->__pv->__type = WUiTimer::IDLER;
	if( !monitorObj.expired() )
	{
		timer->__pv->__uiObjMonitor = new std::weak_ptr<IWUiObject>;
		*timer->__pv->__uiObjMonitor = monitorObj;
	}
	else
	{
		WDEBUG( "No monitored object is set!");
	}

	timer->__pv->__timerFunc = timerFunc;
	timer->__pv->__userData = userData;
	Ecore_Idler* ecoreIdler = ecore_idler_add( __WUiTimerImpl::__timerCb, timer );
	if( ecoreIdler == NULL )
	{
		WERROR("failed to add ecore idler!");
		delete timer;
		return NULL;
	}
	timer->__pv->__timer = ecoreIdler;
	return timer;
}
WUiTimer* WUiTimer::addAnimator( const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj )
{
	WUiTimer* timer = new WUiTimer();
	timer->__pv->__type = WUiTimer::ANIMATOR;
	if( !monitorObj.expired() )
	{
		timer->__pv->__uiObjMonitor = new std::weak_ptr<IWUiObject>;
		*timer->__pv->__uiObjMonitor = monitorObj;
	}
	else
	{
		WDEBUG( "No monitored object is set!");
	}
	timer->__pv->__timerFunc = timerFunc;
	timer->__pv->__userData = userData;
	Ecore_Animator* ecoreAnimator = ecore_animator_add( __WUiTimerImpl::__timerCb, timer );
	if( ecoreAnimator == NULL )
	{
		WERROR("failed to add ecore animator!");
		delete timer;
		return NULL;
	}
	timer->__pv->__timer = ecoreAnimator;
	return timer;
}
WUiTimer* WUiTimer::addTimer( double interval, const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj )
{
	WUiTimer* timer = new WUiTimer();
	timer->__pv->__type = WUiTimer::TIMER;
	if( !monitorObj.expired() )
	{
		timer->__pv->__uiObjMonitor = new std::weak_ptr<IWUiObject>;
		*timer->__pv->__uiObjMonitor = monitorObj;
	}
	else
	{
		WDEBUG( "No monitored object is set!");
	}
	timer->__pv->__timerFunc = timerFunc;
	timer->__pv->__userData = userData;
	Ecore_Timer* ecoreTimer = ecore_timer_add( interval, __WUiTimerImpl::__timerCb, timer );
	if( ecoreTimer == NULL )
	{
		WERROR("failed to add ecore timer!");
		delete timer;
		return NULL;
	}
	timer->__pv->__timer = ecoreTimer;

	return timer;
}
WUiTimer* WUiTimer::addJob( const std::function<void (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj )
{
	WUiTimer* timer = new WUiTimer();
	timer->__pv->__type = WUiTimer::JOB;
	if( !monitorObj.expired() )
	{
		timer->__pv->__uiObjMonitor = new std::weak_ptr<IWUiObject>;
		*timer->__pv->__uiObjMonitor = monitorObj;
	}
	else
	{
		WDEBUG( "No monitored object is set!");
	}
	timer->__pv->__jobFunc = timerFunc;
	timer->__pv->__userData = userData;
	Ecore_Job* ecoreJob = ecore_job_add( __WUiTimerImpl::__jobCb, timer );
	if( ecoreJob == NULL )
	{
		WERROR("failed to add ecore job!");
		delete timer;
		return NULL;
	}
	timer->__pv->__timer = ecoreJob;

	return timer;
}
