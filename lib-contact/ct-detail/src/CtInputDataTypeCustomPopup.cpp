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

#include <Elementary.h>
#include <contacts.h>
//#include <efl_assist.h>
#include <notification.h>

#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "CtCommon.h"
#include "CtPath.h"
#include "CtType.h"
#include "CtInputDataTypeCustomPopup.h"
#include "CtThemeMgr.h"
#include "WEditfield.h"

CtInputDataTypeCustomPopup::CtInputDataTypeCustomPopup()
{
	__customEntry = NULL;
	__resultCb = NULL;
	__notificationTimer = NULL;
}

void CtInputDataTypeCustomPopup::setOnResultCb(std::function<void (const char* text)> resultCb)
{
	__resultCb = resultCb;
}
void CtInputDataTypeCustomPopup:: __entryShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	WHIT();
	elm_object_focus_set(obj, EINA_TRUE);
	evas_object_event_callback_del(obj, EVAS_CALLBACK_SHOW, __entryShowCb);
}

Evas_Object* CtInputDataTypeCustomPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	Evas_Object* popup = NULL;

	setTitle(V_("IDS_PB_HEADER_ENTER_CUSTOM_LABEL_ABB"));
	setContent( [this](Evas_Object* parent)->Evas_Object* {
		std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
		Evas_Object *layout = elm_layout_add(parent);
		CtThemeMgr::getInstance()->setObjectTheme(layout);
		elm_layout_file_set(layout, contactsEdjePath.c_str(), "popup_input_text");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_object_content_set(parent, layout);

		WEditfield *editfield = __createEditField(layout, V_("IDS_MF_POP_INPUT_TEXT"), false, NULL, NULL, NULL, NULL);
		Evas_Object *entry = editfield->getEntry();

		elm_object_part_content_set(layout, "elm.swallow.content" , editfield->getEvasObj());
		evas_object_smart_callback_add(entry, "changed", __dataTypeCustomEntryChangedCb, this);
		evas_object_smart_callback_add(entry, "preedit,changed", __dataTypeCustomEntryChangedCb, this);

		Evas_Object *clear_btn = editfield->getClearButton();
		evas_object_smart_callback_add(clear_btn, "clicked", __dataTypeCustomEntryChangedCb, this);
		evas_object_event_callback_add(entry, EVAS_CALLBACK_SHOW, __entryShowCb, NULL);
		//evas_object_smart_callback_add(entry, "activated", ct_common_entry_done_activated_cb, NULL);
		__customEntry = entry;
		return layout;
	});

	addButton( V_("IDS_PB_BUTTON_CANCEL"), NULL );
	addButton( V_("IDS_PB_SK_CREATE_ABB"), [this](bool* destroyPopup){
		__saveCustomDataType();
		*destroyPopup = true;
	});
	popup = WPopup::onCreate( parent, viewParam );
	Evas_Object* saveBtn = elm_object_part_content_get(popup, "button2");
	elm_object_disabled_set(saveBtn, EINA_TRUE);

	return popup;
}

void CtInputDataTypeCustomPopup::onDestroy()
{
	WHIT();
}

CtInputDataTypeCustomPopup::~CtInputDataTypeCustomPopup()
{
	if (__notificationTimer) {
		ecore_timer_del(__notificationTimer);
	}
}

WEditfield* CtInputDataTypeCustomPopup::__createEditField(Evas_Object *parent, const char *guideText, bool multiline,
		Evas_Smart_Cb unfocusedCallback, void *userData1, Evas_Smart_Cb focusedCallback, void *userData2)
{
	WHIT();
	WEditfield *editfield = new WEditfield();
	editfield->create(parent, NULL);

	Evas_Object *entry = editfield->getEntry();

	if (guideText && *guideText)
		elm_object_domain_translatable_part_text_set(entry, "elm.guide", PACKAGE, guideText);

	if (!multiline)
		elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);

	Elm_Entry_Filter_Limit_Size filter;
	filter.max_char_count = CT_EDITFIELD_MAX_CHAR;
	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &filter);

	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
	evas_object_smart_callback_add(entry, "maxlength,reached", __editfieldTextMaxReachedCb, this);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);

	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	elm_entry_input_panel_imdata_set(entry, (void*)"allowEmoji=true", 15);

//		if (unfocusedCallback)
//			evas_object_smart_callback_add(entry, "unfocused", unfocusedCallback, userData1);
//
//		if (focusedCallback)
//			evas_object_smart_callback_add(entry, "focused", focusedCallback, userData2);

	return editfield;
}

void CtInputDataTypeCustomPopup::__editfieldTextMaxReachedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	CtInputDataTypeCustomPopup *customPopup = (CtInputDataTypeCustomPopup*)data;

	if (customPopup->__notificationTimer) {
		return;
	}

	customPopup->__showTickerNotification(V_("IDS_PB_POP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
	customPopup->__notificationTimer = ecore_timer_add(3.5, __notificationTimerCb, customPopup);
}

Eina_Bool CtInputDataTypeCustomPopup::__notificationTimerCb(void *data)
{
	WHIT();
	WPRET_VM(NULL == data, ECORE_CALLBACK_CANCEL, "parameter(data) is NULL");
	CtInputDataTypeCustomPopup *customPopup = (CtInputDataTypeCustomPopup*)data;

	customPopup->__notificationTimer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void CtInputDataTypeCustomPopup::__showTickerNotification(const char *msg)
{
	WHIT();
	int err = notification_status_message_post(msg);
	WPWARN(CONTACTS_ERROR_NONE != err, "notification_status_message_post() Failed(%d)", err);
}

void CtInputDataTypeCustomPopup::__saveCustomDataType()
{
//	WHIT();
//	CtInputDataTypeCustomPopup *p = (CtInputDataTypeCustomPopup *)data;
//	int err = CONTACTS_ERROR_NONE;
//	Evas_Object *typeBtn = NULL;
	char *realStr = NULL;
	size_t i = 0;
//	char* temp = NULL;

	char *text = ctCommonMarkupToUtf8(elm_entry_entry_get(__customEntry));

	// remove  ' ' in the head of string
	for (i = 0; i < SAFE_STRLEN(text); i++) {
		if (' ' != text[i]){
			realStr = text + i;
			break;
		}
	}

	if (realStr != NULL) {
		// remove  ' ' at the end of string
		for (i = (SAFE_STRLEN(realStr) - 1); i > 0; i--) {
			if (' ' != realStr[i]){
				realStr[i+1] = '\0';
				break;
			}
		}
	}

	if( realStr && __resultCb)
		__resultCb( realStr );
	free(text);

//	if (0 == strcmp(p->__viewUri, _contacts_number._uri)) {
//
//		p->__inputData->setNumberType(p->__record, CONTACTS_NUMBER_TYPE_CUSTOM, realStr);
//	}
//	else if (0 == strcmp(p->__viewUri, _contacts_email._uri)) {
//		p->__inputData->setEmailType(p->__record, CONTACTS_EMAIL_TYPE_CUSTOM, realStr);
//	}
//	else if (0 == strcmp(p->__viewUri, _contacts_address._uri)) {
//		p->__inputData->setAddressType(p->__record, CONTACTS_ADDRESS_TYPE_CUSTOM, realStr);
//	}
//	else if (0 == strcmp(p->__viewUri, _contacts_messenger._uri)) {
//		p->__inputData->setMessengerType(p->__record, CONTACTS_MESSENGER_TYPE_CUSTOM, realStr);
//	}
//	else if (0 == strcmp(p->__viewUri, _contacts_relationship._uri)) {
//		p->__inputData->setRelationshipType(p->__record, CONTACTS_RELATIONSHIP_TYPE_CUSTOM, realStr);
//	}
//	else if (0 == strcmp(p->__viewUri, _contacts_event._uri)) {
//		p->__inputData->setEventType(p->__record, CONTACTS_EVENT_TYPE_CUSTOM, realStr);
//	}
//
//	elm_genlist_item_fields_update(p->__item, "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
//	p->destroy();

}

void CtInputDataTypeCustomPopup::__dataTypeCustomEntryChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	CtInputDataTypeCustomPopup *customPopup = (CtInputDataTypeCustomPopup*)data;
	Evas_Object *popup = customPopup->getEvasObj();
	Evas_Object *saveBtn = NULL;
	char *text;

	text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
	saveBtn = elm_object_part_content_get(popup, "button2");

	if (text && *text)
		elm_object_disabled_set(saveBtn, EINA_FALSE);
	else
		elm_object_disabled_set(saveBtn, EINA_TRUE);
	free(text);
}
//void CtInputDataTypeCustomPopup::__dataTypeCustomCancelCb(void *data, Evas_Object *obj, void *event_info)
//{
//	CtInputDataTypeCustomPopup *p = (CtInputDataTypeCustomPopup *)data;
//	p->destroy();
//}

