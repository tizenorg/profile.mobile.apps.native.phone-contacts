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
using namespace std::placeholders;

BasicInfoItem::BasicInfoItem(LogGroup *group)
	: m_Group(group)
{
	m_Log = m_Group->getLogList().back();
	m_GroupChangeCbHandle = m_Group->addChangeCallback(std::bind(&BasicInfoItem::onGroupChanged, this, _1));
}

BasicInfoItem::~BasicInfoItem()
{
	if (m_Group) {
		m_Group->removeChangeCallback(m_GroupChangeCbHandle);
	}
}


void BasicInfoItem::setBackCallback(BackCallback callback)
{
	m_OnBackPressed = std::move(callback);
}

void BasicInfoItem::updateGroup(LogGroup *group)
{
	m_Group = group;
	if (m_Group) {
		m_Log = m_Group->getLogList().back();
		m_GroupChangeCbHandle = m_Group->addChangeCallback(std::bind(&BasicInfoItem::onGroupChanged, this, _1));
	}
}

Elm_Genlist_Item_Class *BasicInfoItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(LOGS_DETAILS_ITEM_STYLE);
	return &itc;
}

char *BasicInfoItem::getText(Evas_Object *parent, const char *part)
{
	if (isSavedLog() && strcmp(part, PART_NAME) == 0) {
		return Utils::safeDup(m_Log->getName());
	}

	return nullptr;
}

Evas_Object *BasicInfoItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_BACK_BTN) == 0) {
		return createBackButton(parent);
	} else if (strcmp(part, PART_THUMBNAIL) == 0) {
		auto control = Ui::Thumbnail::create(parent, Ui::Thumbnail::SizeLarge, m_Log->getImagePath());
		return control->getEvasObject();
	} else if (!isSavedLog() && strcmp(part, PART_UNSAVED_BTNS) == 0) {
		return createUnsavedButtons(parent);
	}
	return nullptr;
}

Eina_Bool BasicInfoItem::getState(Evas_Object *parent, const char *part)
{
	return isSavedLog();
}

void BasicInfoItem::onSelected()
{
	if (isSavedLog()) {
		m_AppControl = App::requestContactDetails(m_Log->getPersonId());
		m_AppControl.launch();
	}
}

bool BasicInfoItem::isSavedLog()
{
	return m_Log->getPersonId() > 0;
}

Evas_Object *BasicInfoItem::createBackButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "naviframe/back_btn/default");
	evas_object_propagate_events_set(button, EINA_FALSE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&BasicInfoItem::onBackPressed), this);
	return button;
}

Evas_Object *BasicInfoItem::createUnsavedButtons(Evas_Object *parent)
{
	Evas_Object *box = elm_box_add(parent);
	elm_box_padding_set(box, UNSAVED_BTN_PAD_BETWEEN, 0);
	elm_box_horizontal_set(box, EINA_TRUE);

	createUnsavedButton(box, "IDS_KPD_OPT_CREATE_CONTACT_ABB2",
			(Evas_Smart_Cb) makeCallback(&BasicInfoItem::onCreatePressed));

	createUnsavedButton(box, "IDS_KPD_OPT_UPDATE_CONTACT_ABB",
			(Evas_Smart_Cb) makeCallback(&BasicInfoItem::onUpdatePressed));

	return box;
}

Evas_Object *BasicInfoItem::createUnsavedButton(Evas_Object *box, const char *text, Evas_Smart_Cb func)
{
	Evas_Object *button = elm_button_add(box);
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
	if (m_OnBackPressed) {
		m_OnBackPressed();
	}
}

void BasicInfoItem::onCreatePressed()
{
	m_AppControl = App::requestContactCreate(m_Log->getNumber());
	m_AppControl.launch();
}

void BasicInfoItem::onUpdatePressed()
{
	m_AppControl = App::requestContactEdit(0, m_Log->getNumber());
	m_AppControl.launch();
}

void BasicInfoItem::onGroupChanged(int type)
{
	if (!(type & LogGroup::ChangeRemoved)) {
		m_Log = m_Group->getLogList().back();
		if (type & LogGroup::ChangePerson) {
			elm_genlist_item_fields_update(getObjectItem(), STATE_SAVED, ELM_GENLIST_ITEM_FIELD_STATE);
			elm_genlist_item_fields_update(getObjectItem(), PART_UNSAVED_BTNS, ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
		if (type & LogGroup::ChangeName) {
			elm_genlist_item_fields_update(getObjectItem(), PART_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
		}
		if (type & LogGroup::ChangeImage) {
			elm_genlist_item_fields_update(getObjectItem(), PART_THUMBNAIL, ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

