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

#include "Ui/Control.h"
#include "Utils/Callback.h"

using namespace Ui;

Control::Control()
	: m_Object(nullptr)
{
}

Control::~Control()
{
	destroyEvasObject();
}

Evas_Object *Control::create(Evas_Object *parent)
{
	if (!m_Object) {
		setEvasObject(onCreate(parent));
	}

	return m_Object;
}

Evas_Object *Control::getEvasObject() const
{
	return m_Object;
}

void Control::setEvasObject(Evas_Object *object)
{
	m_Object = object;
	evas_object_event_callback_add(object, EVAS_CALLBACK_FREE,
			makeCallback(&Control::onDestroy), this);
}

Evas_Object *Control::resetEvasObject()
{
	Evas_Object *object = m_Object;
	evas_object_event_callback_del(m_Object, EVAS_CALLBACK_FREE,
			makeCallback(&Control::onDestroy));
	m_Object = nullptr;
	return object;
}

void Control::destroyEvasObject()
{
	if (m_Object) {
		evas_object_del(resetEvasObject());
	}
}

void Control::onDestroy(Evas *e, Evas_Object *obj, void *event_info)
{
	resetEvasObject();
	delete this;
}
