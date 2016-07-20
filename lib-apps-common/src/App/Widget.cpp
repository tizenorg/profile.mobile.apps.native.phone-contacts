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

#include "App/Widget.h"
#include "App/WidgetWindow.h"

using namespace App;

Widget::Widget()
	: m_Context(nullptr), m_Width(0), m_Height(0), m_Window(nullptr)
{
}

Widget::~Widget()
{
	delete m_Window;
}

void Widget::create(widget_context_h context, bundle *content)
{
	m_Context = context;
	m_Window = new WidgetWindow(m_Context);
	m_Window->create(nullptr);
	onCreate(content);
}

void Widget::resize(int width, int height)
{
	m_Width = width;
	m_Height = height;

	evas_object_resize(m_Window->getEvasObject(), m_Width, m_Height);
	evas_object_resize(m_Window->getBaseLayout(), m_Width, m_Height);

	onResize(m_Width, m_Height);
}

int Widget::getWidth() const
{
	return m_Width;
}

int Widget::getHeight() const
{
	return m_Height;
}

widget_size_type_e Widget::getType() const
{
	widget_size_type_e type = WIDGET_SIZE_TYPE_UNKNOWN;
	widget_service_get_size_type(m_Width, m_Height, &type);
	return type;
}

Ui::Window *Widget::getWindow() const
{
	return m_Window;
}

void Widget::saveContent(bundle *content)
{
	widget_app_context_set_content_info(m_Context, content);
}
