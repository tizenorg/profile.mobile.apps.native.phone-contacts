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

#ifndef _WWIDGETAPP_H_
#define _WWIDGETAPP_H_

#include "WApp.h"

class WWidget;

class WAPP_ASSIST_EXPORT WWidgetApp : public WApp
{
public:
	WWidgetApp();

protected:
	/**
	 * @see WApp::onStart()
	 */
	virtual int onStart(int argc, char *argv[]);

	/**
	 * @brief Called when new widget instance requested.
	 * @return New widget instance.
	 */
	virtual WWidget *onWidgetCreate() = 0;

private:
	friend class __WWidgetAppImpl;
};

#endif /* _WWIDGETAPP_H_ */
