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

#include "WAppImpl.h"
#include "WWidgetApp.h"
#include "WWidget.h"
#include "WDebugBase.h"

#include <widget_app.h>

namespace
{
	inline WWidget *getWidget(widget_context_h context)
	{
		void *tag = nullptr;
		widget_app_context_get_tag(context, &tag);
		return static_cast<WWidget*>(tag);
	}
}

class __WWidgetAppImpl : public __WAppImpl
{
public:
	static widget_class_h onCreate(void *data);

	static int onWidgetCreate(widget_context_h context, bundle *content, int w, int h, void *data);
	static int onWidgetDestroy(widget_context_h context, widget_app_destroy_type_e reason, bundle *content, void *data);
	static int onWidgetPause(widget_context_h context, void *data);
	static int onWidgetResume(widget_context_h context, void *data);
	static int onWidgetResize(widget_context_h context, int w, int h, void *data);
	static int onWidgetUpdate(widget_context_h context, bundle *content, int force, void *data);

private:
	friend WWidgetApp;
};

widget_class_h __WWidgetAppImpl::onCreate(void *data)
{
	WHIT();
	widget_instance_lifecycle_callback_s cbs;
	cbs.create = __WWidgetAppImpl::onWidgetCreate;
	cbs.destroy = __WWidgetAppImpl::onWidgetDestroy;
	cbs.pause = __WWidgetAppImpl::onWidgetPause;
	cbs.resume = __WWidgetAppImpl::onWidgetResume;
	cbs.resize = __WWidgetAppImpl::onWidgetResize;
	cbs.update = __WWidgetAppImpl::onWidgetUpdate;

	static_cast<WWidgetApp*>(data)->onCreate();
	return widget_app_class_create(cbs, data);
}

int __WWidgetAppImpl::onWidgetCreate(widget_context_h context, bundle *content, int w, int h, void *data)
{
	WHIT();
	WWidget *widget = static_cast<WWidgetApp*>(data)->onWidgetCreate();
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget_app_context_set_tag(context, widget);
	widget->create(context, content, w, h);
	return WIDGET_ERROR_NONE;
};

int __WWidgetAppImpl::onWidgetDestroy(widget_context_h context, widget_app_destroy_type_e reason, bundle *content, void *data)
{
	WWidget *widget = getWidget(context);
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget->onDestroy(reason);
	delete widget;
	return WIDGET_ERROR_NONE;
}

int __WWidgetAppImpl::onWidgetPause(widget_context_h context, void *data)
{
	WWidget *widget = getWidget(context);
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget->onPause();
	return WIDGET_ERROR_NONE;
};

int __WWidgetAppImpl::onWidgetResume(widget_context_h context, void *data)
{
	WWidget *widget = getWidget(context);
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget->onResume();
	return WIDGET_ERROR_NONE;
};

int __WWidgetAppImpl::onWidgetResize(widget_context_h context, int w, int h, void *data)
{
	WWidget *widget = getWidget(context);
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget->resize(w, h);
	return WIDGET_ERROR_NONE;
};

int __WWidgetAppImpl::onWidgetUpdate(widget_context_h context, bundle *content, int force, void *data)
{
	WWidget *widget = getWidget(context);
	if (!widget) {
		return WIDGET_ERROR_FAULT;
	}

	widget->onUpdate(force);
	return WIDGET_ERROR_NONE;
};

WWidgetApp::WWidgetApp()
	: WApp(new __WWidgetAppImpl())
{
}

int WWidgetApp::onStart(int argc, char *argv[])
{
	WHIT();
	widget_app_lifecycle_callback_s cbs;
	cbs.create = __WWidgetAppImpl::onCreate;
	cbs.terminate = __WWidgetAppImpl::__onTerminate;

	return widget_app_main(argc, argv, &cbs, this);
}
