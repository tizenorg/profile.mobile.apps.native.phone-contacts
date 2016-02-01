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

#include "App/Path.h"
#include "Ui/ProgressPopup.h"
#include "Utils/Logger.h"

#include "CommonProgressbarLayout.h"

#define BUFFER_SIZE        64

#define ZERO_PROGRESS      "0%"

using namespace Ui;

namespace
{
	const std::string layoutFilePath = App::getResourcePath(COMMON_PROGRESSBAR_LAYOUT_EDJ);
}

ProgressPopup::ProgressPopup(size_t maxValue)
	: m_MaxValue(maxValue), m_Progressbar(nullptr)
{
}

void ProgressPopup::updateProgressbar(size_t value)
{
	RETM_IF(value > m_MaxValue, "value is bigger than max value");
	double progressValue = (double)value / m_MaxValue;

	char progress[BUFFER_SIZE] = { 0, };
	snprintf(progress, sizeof(progress), "%d%%", (int)(100.0 * progressValue));

	char total[BUFFER_SIZE] = { 0, };
	snprintf(total, sizeof(total), "%d/%d", value, m_MaxValue);

	elm_progressbar_value_set(m_Progressbar, progressValue);
	elm_object_part_text_set(m_Progressbar, "elm.text.bottom.left", progress);
	elm_object_part_text_set(m_Progressbar, "elm.text.bottom.right", total);
}

Evas_Object *ProgressPopup::onCreate(Evas_Object *parent)
{
	Evas_Object *popup = Popup::onCreate(parent);

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutFilePath.c_str(), GROUP_PROGRESSBAR_LAYOUT);

	m_Progressbar = elm_progressbar_add(layout);
	elm_progressbar_pulse(m_Progressbar, EINA_TRUE);

	char total[BUFFER_SIZE] = { 0, };
	snprintf(total, sizeof(total), "%d/%d", 0, m_MaxValue);
	elm_object_part_text_set(m_Progressbar, "elm.text.bottom.left", ZERO_PROGRESS);
	elm_object_part_text_set(m_Progressbar, "elm.text.bottom.right", total);

	elm_object_part_content_set(layout, PART_PROGRESSBAR, m_Progressbar);
	elm_object_content_set(popup, layout);

	return popup;
}
