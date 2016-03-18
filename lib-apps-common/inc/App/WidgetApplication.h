/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef APP_WIDGET_APPLICATION_H
#define APP_WIDGET_APPLICATION_H

#include <widget_app.h>

namespace App
{
	class Widget;

	/**
	 * @brief Widget application lifecycle controller.
	 */
	class EXPORT_API WidgetApplication
	{
	public:
		/**
		 * @brief Run application main loop.
		 * @param[in]   argc    argc parameter received in main()
		 * @param[in]   argv    argv parameter received in main()
		 */
		int run(int argc, char **argv);

	protected:
		virtual ~WidgetApplication() { }

		/**
		 * @brief Called when application is created.
		 */
		virtual void onCreate() { }

		/**
		 * @brief Called when application is terminated.
		 */
		virtual void onTerminate() { }

		/**
		 * @return Create widget instance.
		 */
		virtual Widget *createWidget() = 0;

	private:
		widget_class_h onWidgetClassCreate();

		static Widget *getWidget(widget_context_h context);

		static int onWidgetCreate(widget_context_h context,
				bundle *content, int width, int height, void *data);
		static int onWidgetDestroy(widget_context_h context,
				widget_app_destroy_type_e reason, bundle *content, void *data);

		static int onWidgetPause(widget_context_h context, void *data);
		static int onWidgetResume(widget_context_h context, void *data);

		static int onWidgetResize(widget_context_h context,
				int width, int height, void *data);
		static int onWidgetUpdate(widget_context_h context,
				bundle *content, int force, void *data);
	};
}

#endif /* APP_WIDGET_APPLICATION_H */
