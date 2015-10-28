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

#include <dlfcn.h>
#include "WUiTimer.h"

#include "ContactsAppControl.h"
#include "ContactsDebug.h"

#include "PhPath.h"
#include "PhCommon.h"

#include "PhLogUtil.h"
#include "PhLogDetailButtonInfoControl.h"

PhLogDetailButtonInfoControl::PhLogDetailButtonInfoControl(Evas_Object* window, Evas_Object* naviframe):
__messageIcon(NULL),
__voiceIcon(NULL),
__dlHandle(NULL),
__msgHandler(NULL)
{
}

PhLogDetailButtonInfoControl::~PhLogDetailButtonInfoControl()
{
	WHIT();
}

Evas_Object* PhLogDetailButtonInfoControl::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	Evas_Object *baseLayout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/detail/info/button");
	WPRET_VM(NULL == baseLayout, NULL, "PhCommon::createLayout() return NULL");

	//message button
	Evas_Object *messageButton = PhCommon::createButton(baseLayout, "transparent", NULL, [](void *data, Evas_Object *obj, void *event_info){
		PhLogDetailButtonInfoControl *p = (PhLogDetailButtonInfoControl*)data;
		if (p->__msgHandler)
			p->__msgHandler(p->__number);
	}, this);
	WPRET_VM(NULL == messageButton, NULL, "PhCommon::createButton() return NULL");
	elm_layout_content_set(baseLayout,"elm.swallow.message", messageButton);

	__messageIcon = PhCommon::createLayout(messageButton, LOG_LIST_EDJ, PH_LOG_ICON_DETAIL_MESSAGE);
	WPRET_VM(NULL == __messageIcon, NULL, "PhCommon::createImage() return NULL");
	elm_object_content_set(messageButton, __messageIcon);

	//voice call button
	Evas_Object *voiceButton = PhCommon::createButton(baseLayout, "transparent", NULL, [](void *data, Evas_Object *obj, void *event_info) {
		PhLogDetailButtonInfoControl *p = (PhLogDetailButtonInfoControl*)data;
		int err = launchCall(p->__number.c_str());
		WPWARN(err != APP_CONTROL_ERROR_NONE, "launchCall() failed(%d)", err);
	}, this);
	WPRET_VM(NULL == voiceButton, NULL, "PhCommon::createButton() return NULL");
	elm_layout_content_set(baseLayout, "elm.swallow.voice", voiceButton);

	__voiceIcon = PhCommon::createLayout(voiceButton, LOG_LIST_EDJ, PH_LOG_ICON_DETAIL_VOICE_CALL);
	WPRET_VM(NULL == __voiceIcon, NULL, "PhCommon::createImage() return NULL");
	elm_object_content_set(voiceButton, __voiceIcon);

#if 1
	WINFO("dlopen - request");
	WUiTimer::addTimer(1.5, [](void* data)->Eina_Bool {
		PhLogDetailButtonInfoControl* p = (PhLogDetailButtonInfoControl*)data;
		WINFO("dlopen - try");
		if((p->__dlHandle = dlopen("/usr/apps/org.tizen.message-lite/lib/ug/libmsg-composer-efl.so", RTLD_NOW)) == NULL)
		{
			WWARNING("%s : dlopen Fail {%s}\n", "/usr/apps/org.tizen.message-lite/lib/ug/libmsg-composer-efl.so", dlerror());
		}
		else
		{
			WDEBUG("dlopen!");
			dlerror();  // error clear
		}
		return ECORE_CALLBACK_CANCEL;
	}, this, getWeakPtr());
#else
	WINFO("dlopen - try");
	if((__dlHandle = dlopen("/usr/apps/org.tizen.message-lite/lib/ug/libmsg-composer-efl.so", RTLD_NOW)) == NULL)
	{
		WWARNING("%s : dlopen Fail {%s}\n", "/usr/apps/org.tizen.message-lite/lib/ug/libmsg-composer-efl.so", dlerror());
	}
	else
	{
		WDEBUG("dlopen!");
		dlerror();  // error clear
	}
#endif
	return baseLayout;
}
void PhLogDetailButtonInfoControl::onDestroy()
{
	WHIT();
	if(__dlHandle)
	{
		WDEBUG("dlclose!");
		dlclose(__dlHandle);
		__dlHandle = NULL;
	}
}

void PhLogDetailButtonInfoControl::update(const std::string& number)
{
	WHIT();

	__number = number;
}

void PhLogDetailButtonInfoControl::setOnMsgHandler( const std::function<void (const std::string& number)>& handlerFunc )
{
	__msgHandler = handlerFunc;
}
