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

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include <notification.h>
#include "CtString.h"
#include "WEditfield.h"


CtInputViewGenlistItem::CtInputViewGenlistItem(const char* oldVal):
__isChanged(false), __isEmpty(true), __notificationTimer(NULL), __entry(NULL), __focused(false)
{
	WHIT();
	__oldVal = SAFE_STRDUP(oldVal);
}

CtInputViewGenlistItem::CtInputViewGenlistItem():
__isChanged(false), __isEmpty(true), __oldVal(NULL), __notificationTimer(NULL), __entry(NULL), __focused(false)
{
	WHIT();
}

CtInputViewGenlistItem::~CtInputViewGenlistItem()
{
	WHIT();
	SAFE_FREE(__oldVal);
	if (__notificationTimer) {
		ecore_timer_del(__notificationTimer);
	}
}

void CtInputViewGenlistItem::onRealized()
{
	if (__entry && __focused) {
		elm_object_focus_set(__entry, EINA_TRUE);
		__focused = false;
	}
}

void CtInputViewGenlistItem::onUnrealized()
{
}

bool CtInputViewGenlistItem::getIsChanged()
{
	return __isChanged;
}

void CtInputViewGenlistItem::setIsChanged(bool isChanged)
{
	__isChanged = isChanged;
}

bool CtInputViewGenlistItem::getIsEmpty()
{
	return __isEmpty;
}

void CtInputViewGenlistItem::updateChangeState(const char *text)
{
	if (SAFE_STRLEN(text)) {
		__isEmpty = false;
		if( __oldVal && !strcmp(text, __oldVal) ) {
			__isChanged = false;
		}
		else {
			__isChanged = true;
		}
	}
	else {
		__isEmpty = true;
		if( __oldVal && __oldVal[0] != '\0' ) {
			__isChanged = true;
		}
		else {
			__isChanged = false;
		}
	}
}

WEditfield* CtInputViewGenlistItem::createEditField(Evas_Object *parent, const char *guideText, bool multiline, bool isMainField)
{
	WENTER();
	WEditfield *editfield = new WEditfield();
	editfield->create(parent, NULL);

	Evas_Object *entry = editfield->getEntry();

	if (guideText && *guideText)
		elm_object_domain_translatable_part_text_set(entry, "elm.guide", PACKAGE, guideText);

	if (!multiline)
		elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);

	Elm_Entry_Filter_Limit_Size filter;
	if (isMainField) {
		filter.max_char_count = CT_MAIN_NAME_EDITFIELD_MAX_CHAR;
	}
	else {
		filter.max_char_count = CT_EDITFIELD_MAX_CHAR;
	}
	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &filter);

	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
	evas_object_smart_callback_add(entry, "maxlength,reached", __editfieldTextMaxReachedCb, this);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);

	__entry = entry;

	WLEAVE();

	return editfield;
}

void CtInputViewGenlistItem::__editfieldTextMaxReachedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "parameter(data) is NULL");
	CtInputViewGenlistItem *itemData = (CtInputViewGenlistItem *)data;

	if (itemData->__notificationTimer) {
		return;
	}
//TODO : tranlation need
	itemData->__showTickerNotification(V_("IDS_PB_POP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
	itemData->__notificationTimer = ecore_timer_add(3.5, __notificationTimerCb, itemData);
}

Eina_Bool CtInputViewGenlistItem::__notificationTimerCb(void *data)
{
	WHIT();
	WPRET_VM(NULL == data, ECORE_CALLBACK_CANCEL, "parameter(data) is NULL");
	CtInputViewGenlistItem *itemData = (CtInputViewGenlistItem *)data;

	itemData->__notificationTimer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void CtInputViewGenlistItem::__showTickerNotification(const char *msg)
{
	WHIT();
	int err = notification_status_message_post(msg);
	WPWARN(CONTACTS_ERROR_NONE != err, "notification_status_message_post() Failed(%d)", err);
}

void CtInputViewGenlistItem::setEntryText(Evas_Object *entry, const char *text)
{
	WPRET_M(NULL == entry, "entry is NULL");
	WPRET_M(NULL == text, "text is NULL");
	char *input = NULL;
	input = elm_entry_utf8_to_markup(text);
	elm_entry_entry_set(entry, "");
	if (input && *input) {
		elm_entry_entry_insert(entry, input);
	}
	free(input);
}

const char* CtInputViewGenlistItem::getEntryObjPartName()
{
	return NULL;
}

Evas_Object* CtInputViewGenlistItem::getEntry()
{
	Evas_Object *entry = NULL;
	Evas_Object *layout = elm_object_item_part_content_get(getElmObjectItem(), getEntryObjPartName());
	if (layout) {
		entry = elm_object_part_content_get(layout, "elm.swallow.content");
	}

	return entry;
}

void CtInputViewGenlistItem::focus()
{
	Evas_Object *entry = getEntry();
	if (entry) {
		elm_object_focus_set(entry, EINA_TRUE);
	} else {
		__focused = true;
	}
}
