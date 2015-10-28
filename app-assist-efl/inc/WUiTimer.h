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

#ifndef _WUI_TIMER_H_
#define _WUI_TIMER_H_

#include <functional>
#include <Eina.h>
#include "WDefine.h"
#include "IWUiObject.h"

/**
* @class	WUiTimer
* @brief	This class wraps ecore timer module.
*
*
* The %WUiTimer is the class managing ecore timer
*/
class WAPP_ASSIST_EXPORT WUiTimer
{
public:
	enum TimerType{
		IDLER,		///< Can be called multiple times with in one rendering cycle, based on system load.
		ANIMATOR, 	///< Called just once per rendering cycle.
		TIMER,		///< Called just once per rendering cycle, if the "set time" is elapsed before next vsync.
		JOB			///< Called just once per registration prior to timer, idler and animator.
	};
	/**
	 * Returns type of timer.
	 *
	 * @return timer type
	 */
	TimerType getType();
	/**
	 * Deletes this instance.
	 *
	 */
	void destroy();
	/**
	 * Creates a IDLER type timer. It uses ecore_idler internally.
	 * It can be called multiple times with in one rendering cycle, based on system load.
	 *
	 * @param[in]	timerFunc	std::function object of worker function
	 * @param[in]	userData	user data
	 * @param[in]	monitorObj	UI object, like WView or WControl object, is to be monitored.
	 * 							If it has been deleted,when callback function is called, the callback function will not do anything and just delete itself.
	 *
	 * @return WUiTimer instance created. If failed, NULL will be returned.
	 */
	static WUiTimer* addIdler( const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj);
	/**
	 * Creates a ANIMATOR type timer. It uses ecore_animator internally.
	 * It will be called just once per rendering cycle.
	 *
	 * @param[in]	timerFunc	std::function object of worker function
	 * @param[in]	userData	user data
	 * @param[in]	monitorObj	UI object, like WView or WControl object, is to be monitored.
	 * 							If it has been deleted,when callback function is called, the callback function will not do anything and just delete itself.
	 *
	 * @return WUiTimer instance created. If failed, NULL will be returned.
	 */
	static WUiTimer* addAnimator( const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj );
	/**
	 * Creates a timer. It uses ecore_timer internally.
	 * It will be called just once per rendering cycle, if the "set time" is elapsed before next vsync.
	 *
	 * @param[in]	interval	The interval in seconds.
	 * @param[in]	timerFunc	std::function object of worker function
	 * @param[in]	userData	user data
	 * @param[in]	monitorObj	UI object, like WView or WControl object, is to be monitored.
	 * 							If it has been deleted,when callback function is called, the callback function will not do anything and just delete itself.
	 *
	 * @return WUiTimer instance created. If failed, NULL will be returned.
	 */
	static WUiTimer* addTimer( double interval, const std::function<Eina_Bool (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj );
	/**
	 * Creates a JOB type timer. It uses ecore_job internally.
	 * It will be called just once per registration prior to timer, idler and animator.
	 *
	 * @param[in]	timerFunc	std::function object of worker function
	 * @param[in]	userData	user data
	 * @param[in]	monitorObj	UI object, like WView or WControl object, is to be monitored.
	 * 							If it has been deleted,when callback function is called, the callback function will not do anything and just delete itself.
	 *
	 * @return WUiTimer instance created. If failed, NULL will be returned.
	 */
	static WUiTimer* addJob( const std::function<void (void*)>& timerFunc, void* userData, WUiObjectPtr monitorObj );

private:
	WUiTimer();
	~WUiTimer();

private:
	WDECLARE_PRIVATE_IMPL(WUiTimer);
	WDISABLE_COPY_AND_ASSIGN(WUiTimer);

};


#endif /* _WUI_TIMER_H_ */
