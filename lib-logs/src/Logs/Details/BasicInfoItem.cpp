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

#include "Logs/Details/BasicInfoItem.h"

#include "Ui/Genlist.h"
#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"
#include "App/AppControlRequest.h"
#include "AppsCommonButtons.h"

#include "Logs/Model/LogGroup.h"
#include "Logs/Model/Log.h"

#include "LogsDetailsItemLayout.h"
#include "LogsDetailsItemLayoutMetrics.h"

using namespace Logs::Details;
using namespace Logs::Model;

BasicInfoItem::BasicInfoItem(LogGroup *group)
	: m_Group(group)
{
	if (isSavedLog()) {
		Ui::GenlistItem::setSelectCallback(std::bind(&BasicInfoItem::onItemSelected, this));
	}
}

Elm_Genlist_Item_Class *BasicInfoItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(LOGS_DETAILS_ITEM_STYLE);
	return &itc;
}

char *BasicInfoItem::getText(Evas_Object *parent, const char *part)
{
	const Log *log = m_Group->getLogList().back();
	const char *name = log->getName();
	if (strcmp(part, PART_NAME) == 0) {
		if (isSavedLog()) {
			return strdup(name);
		}
	}

	return nullptr;
}

Evas_Object *BasicInfoItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_BACK_BTN) == 0) {
		return createBackButton(parent);
	} else if (strcmp(part, PART_THUMBNAIL) == 0) {
		const Log *log = m_Group->getLogList().back();
		auto control = Ui::Thumbnail::create(parent, Ui::Thumbnail::SizeLarge);
		control->setImagePath(log->getImagePath());
		return control->getEvasObject();
	} else if (strcmp(part, PART_UNSAVED_BTNS) == 0) {
		if (!isSavedLog()) {
			return createUnsavedButtons(parent);
		}
	}
	return nullptr;
}

Eina_Bool BasicInfoItem::getState(Evas_Object *parent, const char *part)
{
	if (strcmp(part, STATE_SAVED) == 0) {
		return isSavedLog();
	}
	return EINA_FALSE;
}

bool BasicInfoItem::isSavedLog()
{
	const Log *log = m_Group->getLogList().back();
	const char *name = log->getName();
	return name != nullptr;
}

Evas_Object *BasicInfoItem::createBackButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "naviframe/back_btn/default");
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&BasicInfoItem::onBackPressed), this);
	return button;
}

Evas_Object *BasicInfoItem::createUnsavedButtons(Evas_Object *parent)
{
	Evas_Object *box = elm_box_add(parent);
	elm_box_padding_set(box, UNSAVED_BTN_PAD_BETWEEN, 0);
	elm_box_horizontal_set(box, EINA_TRUE);

	createUnsavedButton(parent, box, "IDS_LOGS_BUTTON_CREATE_CONTACT_ABB",
			(Evas_Smart_Cb) makeCallback(&BasicInfoItem::onUpdatePressed));

	createUnsavedButton(parent, box, "IDS_LOGS_BUTTON_UPDATE_CONTACT_ABB2",
			(Evas_Smart_Cb) makeCallback(&BasicInfoItem::onUpdatePressed));

	return box;
}

Evas_Object *BasicInfoItem::createUnsavedButton(Evas_Object *parent, Evas_Object *box, const char *text,
		Evas_Smart_Cb func)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "custom_default");

	Evas_Object *edje = elm_layout_edje_get(button);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_NORMAL, COLOR_UNSAVED_BUTTON_NORMAL,
			0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_PRESSED, COLOR_UNSAVED_BUTTON_PRESSED,
			0, 0, 0, 0, 0, 0, 0, 0);

	elm_object_translatable_text_set(button, text);
	evas_object_smart_callback_add(button, "clicked", func, this);

	evas_object_show(button);
	elm_box_pack_end(box, button);
	return button;
}

void BasicInfoItem::onBackPressed(Evas_Object *button, void *eventInfo)
{
	delete getParent();
}

void BasicInfoItem::onCreatePressed()
{
	const Log *log = m_Group->getLogList().back();
	m_AppControl = App::requestContactCreate(log->getNumber());
	m_AppControl.launch();
}

void BasicInfoItem::onUpdatePressed()
{
	const Log *log = m_Group->getLogList().back();
	m_AppControl = App::requestContactEdit(0, log->getNumber());
	m_AppControl.launch();
}

void BasicInfoItem::onItemSelected()
{
	const Log *log = m_Group->getLogList().back();
	m_AppControl = App::requestContactDetails(log->getPersonId());
	m_AppControl.launch();
}
