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

#ifndef _WBUTTON_H_
#define _WBUTTON_H_

#include "WControl.h"
#include <functional>

/**
* @class	WButton
* @brief	Button adaptor class
*
*
* The %WButton class provides convenient press and longpress events handling.
*/
class WAPP_ASSIST_EXPORT WButton : public WControl
{
public:
	WButton();
	~WButton();

	/**
	 * @see WControl::create()
	 */
	virtual bool create(Evas_Object *parent, void *param);

	/**
	 * @see WControl::getClassName()
	 */
	const char *getClassName();

	/**
	 * @brief Set press-event callback.
	 */
	void setOnPressed(std::function<void(WButton&)> callback);

	/**
	 * @brief Set longpress-event callback.
	 * @details Callback should return true if the event was handled.
	 *          If callback returns false onPressed event will be delivered
	 *          as well when the button is released.
	 */
	void setOnLongpressed(std::function<bool(WButton&)> callback);

private:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);

	WDISABLE_COPY_AND_ASSIGN(WButton);
	WDECLARE_PRIVATE_IMPL(WButton);
};

#endif /* _WBUTTON_H_ */
