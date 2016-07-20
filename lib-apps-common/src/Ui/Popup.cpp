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

#include "Ui/Popup.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"

#include <efl_extension.h>

#define PART_POPUP_TITLE "title,text"

using namespace Ui;

namespace
{
	const char *buttonParts[POPUP_BUTTON_MAX_COUNT] = { "button1", "button2", "button3" };
}

Popup::Popup()
	: m_ButtonCount(0)
{
}

Popup *Popup::create(Evas_Object *parent, const char *title,
		const char *text, const char *button_text)
{
	Popup *popup = new Popup();
	popup->create(parent);
	popup->setTitle(title);
	popup->setText(text);
	popup->addButton(button_text);
	return popup;
}

void Popup::setTitle(const char *title)
{
	elm_object_translatable_part_text_set(getEvasObject(), PART_POPUP_TITLE, title);
}

void Popup::setText(const char *text)
{
	elm_object_translatable_text_set(getEvasObject(), text);
}

void Popup::setContent(Evas_Object *content)
{
	elm_object_part_content_set(getEvasObject(), "elm.swallow.content", content);
}

Evas_Object *Popup::addButton(const char *text, ButtonCallback callback)
{
	if (m_ButtonCount >= POPUP_BUTTON_MAX_COUNT) {
		return nullptr;
	}

	Evas_Object *button = elm_button_add(getEvasObject());
	elm_object_style_set(button, "bottom");
	elm_object_translatable_text_set(button, text);
	elm_object_part_content_set(getEvasObject(), buttonParts[m_ButtonCount], button);

	m_ButtonCbs[m_ButtonCount] = std::move(callback);
	evas_object_smart_data_set(button, &m_ButtonCbs[m_ButtonCount]);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&Popup::onButtonPressed), this);

	++m_ButtonCount;
	return button;
}

void Popup::setBackCallback(BackCallback callback)
{
	m_OnBack = std::move(callback);
}

Evas_Object *Popup::onCreate(Evas_Object *parent)
{
	Window *window = findParent<Window>(parent);
	if (window) {
		parent = window->getBaseLayout();
	}

	Evas_Object *popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			makeCallback(&Popup::onBackPressed), this);
	evas_object_show(popup);

	return popup;
}

void Popup::onButtonPressed(Evas_Object *obj, void *eventInfo)
{
	ButtonCallback &callback = *(ButtonCallback *) evas_object_smart_data_get(obj);
	if (!callback || callback()) {
		delete this;
	}
}

void Popup::onBackPressed(Evas_Object *obj, void *eventInfo)
{
	if (!m_OnBack || m_OnBack()) {
		delete this;
	}
}
