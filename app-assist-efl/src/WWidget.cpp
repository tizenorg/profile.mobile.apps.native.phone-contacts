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

#include "WWidget.h"
#include "WWidgetWindow.h"

#include <string>

class __WWidgetImpl
{
public:
	__WWidgetImpl();
	~__WWidgetImpl();

private:
	friend WWidget;

	widget_context_h __context;
	int __width;
	int __height;
	WWindow *__win;
};

__WWidgetImpl::__WWidgetImpl()
	: __context(NULL), __width(0), __height(0), __win(NULL)
{
}

__WWidgetImpl::~__WWidgetImpl()
{
	__win->destroy();
}

WWidget::WWidget()
	: __pv(new __WWidgetImpl())
{
}

WWidget::~WWidget()
{
	delete __pv;
}

int WWidget::getWidth() const
{
	return __pv->__width;
}

int WWidget::getHeight() const
{
	return __pv->__height;
}

widget_size_type_e WWidget::getType() const
{
	widget_size_type_e type = WIDGET_SIZE_TYPE_UNKNOWN;
	widget_service_get_size_type(getWidth(), getHeight(), &type);
	return type;
}

WWindow *WWidget::getWindow() const
{
	return __pv->__win;
}

void WWidget::saveContent(bundle *content)
{
	widget_app_context_set_content_info(__pv->__context, content);
}

void WWidget::create(widget_context_h context, bundle *content, int w, int h)
{
	__pv->__context = context;
	__pv->__width = w;
	__pv->__height = h;
	__pv->__win = new WWidgetWindow(context, w, h);
	__pv->__win->create();
	onCreate(content, w, h);
}

void WWidget::resize(int w, int h)
{
	__pv->__width = w;
	__pv->__height = h;

	evas_object_resize(__pv->__win->getEvasObj(), w, h);
	evas_object_resize(__pv->__win->getBaseLayoutEvasObj(), w, h);

	onResize(w, h);
}

void WWidget::onCreate(bundle *content, int w, int h)
{
}

void WWidget::onDestroy(widget_app_destroy_type_e reason)
{
}

void WWidget::onPause()
{
}

void WWidget::onResume()
{
}

void WWidget::onResize(int w, int h)
{
}

void WWidget::onUpdate(int force)
{
}
