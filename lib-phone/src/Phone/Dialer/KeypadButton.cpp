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

#include "Phone/Dialer/KeypadButton.h"
#include "App/Path.h"
#include "Utils/Logger.h"

#include "DialerLayout.h"

#include <feedback.h>

using namespace Phone::Dialer;

struct Phone::Dialer::KeypadButtonData
{
	char value;
	const char *group;
	feedback_pattern_e pattern;
};

namespace
{
	const KeypadButtonData keyData[] = {
		{ '1', GROUP_BUTTON_1, FEEDBACK_PATTERN_KEY1 },
		{ '2', GROUP_BUTTON_2, FEEDBACK_PATTERN_KEY2 },
		{ '3', GROUP_BUTTON_3, FEEDBACK_PATTERN_KEY3 },
		{ '4', GROUP_BUTTON_4, FEEDBACK_PATTERN_KEY4 },
		{ '5', GROUP_BUTTON_5, FEEDBACK_PATTERN_KEY5 },
		{ '6', GROUP_BUTTON_6, FEEDBACK_PATTERN_KEY6 },
		{ '7', GROUP_BUTTON_7, FEEDBACK_PATTERN_KEY7 },
		{ '8', GROUP_BUTTON_8, FEEDBACK_PATTERN_KEY8 },
		{ '9', GROUP_BUTTON_9, FEEDBACK_PATTERN_KEY9 },
		{ '*', GROUP_BUTTON_ASTERISK, FEEDBACK_PATTERN_KEY_STAR },
		{ '0', GROUP_BUTTON_0, FEEDBACK_PATTERN_KEY0 },
		{ '#', GROUP_BUTTON_SHARP, FEEDBACK_PATTERN_KEY_SHARP },
	};
}

KeypadButton::KeypadButton(Id id)
	: m_Id(id), m_Data(&keyData[id])
{
}

Evas_Object *KeypadButton::onCreate(Evas_Object *parent)
{
	static const std::string path = App::getResourcePath(DIALER_KEYPAD_EDJ);
	Evas_Object *layout = elm_layout_add(parent);
	Eina_Bool res = elm_layout_file_set(layout, path.c_str(), m_Data->group);
	WARN_IF(res != EINA_TRUE, "elm_layout_file_set() failed");
	evas_object_event_callback_add(layout, EVAS_CALLBACK_MOUSE_DOWN,
			(Evas_Object_Event_Cb) feedback_play, (void *) m_Data->pattern);

	return layout;
}

char KeypadButton::getValue() const
{
	return m_Data->value;
}

KeypadButton::Id KeypadButton::getId() const
{
	return m_Id;
}
