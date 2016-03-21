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

#include "App/WidgetApplication.h"
#include "App/Widget.h"
#include "Utils/Callback.h"

using namespace App;

int WidgetApplication::run(int argc, char **argv)
{
	widget_app_lifecycle_callback_s cbs;
	cbs.create = makeCallback(&WidgetApplication::onWidgetClassCreate);
	cbs.terminate = makeCallback(&WidgetApplication::onTerminate);

	return widget_app_main(argc, argv, &cbs, this);
}

widget_class_h WidgetApplication::onWidgetClassCreate()
{
	widget_instance_lifecycle_callback_s cbs;
	cbs.create = &WidgetApplication::onWidgetCreate;
	cbs.destroy = &WidgetApplication::onWidgetDestroy;
	cbs.pause = &WidgetApplication::onWidgetPause;
	cbs.resume = &WidgetApplication::onWidgetResume;
	cbs.resize = &WidgetApplication::onWidgetResize;
	cbs.update = &WidgetApplication::onWidgetUpdate;

	onCreate();
	return widget_app_class_create(cbs, this);
}

Widget *App::WidgetApplication::getWidget(widget_context_h context)
{
	Widget *widget= nullptr;
	widget_app_context_get_tag(context, (void **) &widget);
	return widget;
}

int WidgetApplication::onWidgetCreate(widget_context_h context,
		bundle *content, int width, int height, void *data)
{
	WidgetApplication *app = (WidgetApplication *) data;
	Widget *widget = app->createWidget();
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget_app_context_set_tag(context, widget);
	widget->create(context, content);
	widget->resize(width, height);
	return WIDGET_ERROR_NONE;
}

int WidgetApplication::onWidgetDestroy(widget_context_h context,
		widget_app_destroy_type_e reason, bundle *content, void *data)
{
	Widget *widget = getWidget(context);
	widget->onDestroy(reason);
	delete widget;
	return WIDGET_ERROR_NONE;
}

int WidgetApplication::onWidgetPause(widget_context_h context, void *data)
{
	getWidget(context)->onPause();
	return WIDGET_ERROR_NONE;
}

int WidgetApplication::onWidgetResume(widget_context_h context, void *data)
{
	getWidget(context)->onResume();
	return WIDGET_ERROR_NONE;
}

int WidgetApplication::onWidgetResize(widget_context_h context,
		int width, int height, void *data)
{
	getWidget(context)->resize(width, height);
	return WIDGET_ERROR_NONE;
}

int WidgetApplication::onWidgetUpdate(widget_context_h context,
		bundle *content, int force, void *data)
{
	getWidget(context)->onUpdate(force);
	return WIDGET_ERROR_NONE;
}
