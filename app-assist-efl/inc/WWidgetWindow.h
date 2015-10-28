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

#ifndef _WWIDGET_WINDOW_H_
#define _WWIDGET_WINDOW_H_

#include <widget_app.h>
#include "WWindow.h"

/**
 * @class   WWidgetWindow
 * @brief   This class is managing main window of the widget.
 * @see     WWidget
 */
class WAPP_ASSIST_EXPORT WWidgetWindow : public WWindow
{
public:
	/**
	 * @brief Create WWidgetWindow.
	 * @param[in]   context     Widget context
	 * @param[in]   width       Window width
	 * @param[in]   height      Window height
	 */
	WWidgetWindow(widget_context_h context, int width, int height);

	/**
	 * @see WWindow::getClassName()
	 */
	virtual const char *getClassName() const;

protected:
	/**
	 * @see WWindow::doCreate()
	 */
	virtual Evas_Object *doCreate(Evas_Object *&win, Evas_Object *&conf, Evas_Object *&layout);
};

#endif /* _WWIDGET_WINDOW_H_ */
