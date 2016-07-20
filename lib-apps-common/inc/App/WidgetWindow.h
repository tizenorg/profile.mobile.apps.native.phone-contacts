/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef APP_WIDGET_WINDOW_H
#define APP_WIDGET_WINDOW_H

#include "Ui/Window.h"
#include <widget_app.h>

namespace App
{
	class EXPORT_API WidgetWindow : public Ui::Window
	{
	public:
		/**
		 * @brief Create WidgetWindow.
		 * @param[in]   context     Widget context
		 */
		WidgetWindow(widget_context_h context);

	protected:
		/**
		 * @see Window::onWindowCreate()
		 */
		virtual Evas_Object *onWindowCreate();

	private:
		widget_context_h m_Context;
	};
}

#endif /* APP_WIDGET_WINDOW_H */
