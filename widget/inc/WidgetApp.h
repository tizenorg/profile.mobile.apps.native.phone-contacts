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

#ifndef _WIDGETAPP_H_
#define _WIDGETAPP_H_

#include "WWidgetApp.h"

/**
 * @brief Contacts homescreen widget application.
 */
class WAPP_ASSIST_EXPORT WidgetApp
	: public WWidgetApp
{
private:
	virtual bool onCreate();
	virtual void onTerminate();
	virtual WWidget *onWidgetCreate();
	static void onLanguageChanged(app_event_info_h event, void *data);
};

#endif /* _WIDGETAPP_H_ */
