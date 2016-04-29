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
#include "Utils/Logger.h"

#define CONTROL_DATA_KEY "Ui::Control"

using namespace Ui;

Control::Control(int type)
	: m_Type(type), m_Object(nullptr)
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
		onCreated();
	}

	return m_Object;
}

int Control::getType() const
{
	return m_Type;
}

Evas_Object *Control::getEvasObject() const
{
	return m_Object;
}

Control *Control::findParent(int type) const
{
	return findParent(getEvasObject(), type);
}

Control *Control::findParent(Evas_Object *object, int type)
{
	while (object) {
		Control *control = getControl(object);
		if (control && control->getType() == type) {
			return control;
		}
		object = elm_object_parent_widget_get(object);
	}

	return nullptr;
}

Control *Control::getControl(Evas_Object *object)
{
	return (Control *) evas_object_data_get(object, CONTROL_DATA_KEY);
}

void Control::setEvasObject(Evas_Object *object)
{
	m_Object = object;
	if (!evas_object_data_get(m_Object, CONTROL_DATA_KEY)) {
		evas_object_data_set(m_Object, CONTROL_DATA_KEY, this);
	}

	evas_object_event_callback_add(m_Object, EVAS_CALLBACK_DEL,
			(Evas_Object_Event_Cb) makeCallback(&Control::onDestroy), this);
	evas_object_event_callback_add(m_Object, EVAS_CALLBACK_FREE,
			makeCallback(&Control::onDestroyed), this);
}

Evas_Object *Control::resetEvasObject()
{
	Evas_Object *object = m_Object;
	evas_object_event_callback_del_full(m_Object, EVAS_CALLBACK_DEL,
			(Evas_Object_Event_Cb) makeCallback(&Control::onDestroy), this);
	evas_object_event_callback_del_full(m_Object, EVAS_CALLBACK_FREE,
			makeCallback(&Control::onDestroyed), this);
	m_Object = nullptr;
	return object;
}

void Control::destroyEvasObject()
{
	if (m_Object) {
		onDestroy();
		evas_object_del(resetEvasObject());
	}
}

void Control::onDestroyed(Evas *e, Evas_Object *obj, void *event_info)
{
	resetEvasObject();
	delete this;
}
