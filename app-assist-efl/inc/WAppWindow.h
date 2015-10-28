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

#ifndef _WAPP_WINDOW_H_
#define _WAPP_WINDOW_H_

#include "WWindow.h"

/**
 * @class	WAppWindow
 * @brief	This class is managing main application window and base layout.
 */
class WAPP_ASSIST_EXPORT WAppWindow : public WWindow
{
	public:
		/**
		 * Constructor
		 * Initializes this instance of WWindow with the specified value.
		 *
		 * @param[in]	name	The window name
		 * @param[in]	winType The type of the window
		 */
		WAppWindow(const char* name, Elm_Win_Type winType);

		/**
		 * @see WWindow::getClassName()
		 */
		virtual const char* getClassName() const;

		/**
		 * Gets the window name
		 *
		 * @return The name of the window
		 */
		const char* getName() const;

		/**
		 * Gets the type of the window
		 *
		 * @return The type of the window
		 */
		Elm_Win_Type getWinType() const;

	protected:
		/**
		 * @see WWindow::doCreate()
		 */
		virtual Evas_Object* doCreate(Evas_Object*& win, Evas_Object*& conf, Evas_Object*& layout);
};

#endif /* _WAPP_WINDOW_H_ */
