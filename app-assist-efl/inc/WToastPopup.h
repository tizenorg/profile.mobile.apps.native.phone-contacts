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

#ifndef _WTOAST_POPUP_H_
#define _WTOAST_POPUP_H_

#include "WControl.h"

/**
* @class	WToastPopup
* @brief	This class is the pop-up class to make easy to create toast pop-up.
*
* The %WToastPopup is the toast pop-up class.
* This is not cross-process system pop-up.
* For this purpose, use the system API "notification_status_message_post" instead.
*/
class WAPP_ASSIST_EXPORT WToastPopup : public WControl
{
public:
	/**
	 * This is the constructor with message text. Time-out time is set as default value.
	 *
	 * @param[in]	text	message text string
	 */
	WToastPopup(const char* text);
	/**
	 * This is the constructor with message text and time-out time.
	 *
	 * @param[in]	text	message text string
 	 * @param[in]	timeout	Time-out time to be destroyed. Unit is second.
	 */
	WToastPopup(const char* text, double timeout );
private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* param);
	virtual void onDestroy();
private:
	virtual ~WToastPopup();
private:
	WDECLARE_PRIVATE_IMPL(WToastPopup);
	WDISABLE_COPY_AND_ASSIGN(WToastPopup);
};

#endif
