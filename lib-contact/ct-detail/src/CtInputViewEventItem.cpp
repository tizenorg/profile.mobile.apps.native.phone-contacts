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

#include "CtCommon.h"
#include "CtInputViewEventItem.h"
#include "CtDetailDataBase.h"
#include "CtInputDataTypeCtxPopup.h"
#include "CtInputDataTypeCustomPopup.h"
#include "CtInputView.h"
#include "CtInputData.h"
#include "CtString.h"
#include "CtPath.h"
#include "CtThemeMgr.h"

#define BUTTON_COLOR_NORMAL 82, 198, 216, 255
#define BUTTON_COLOR_PRESSED 48, 139, 153, 255

#define EVENT_DATE_FORMAT "%b/%d/%Y"
#define DATE_TIME_FORMAT "%%c"

CtInputViewEventItem::CtInputViewEventItem( WView* view, std::shared_ptr<CtData>& inputData, contacts_record_h record, const char* oldVal, std::shared_ptr<bool>& birthdayExist,
		const std::function<void ()>& itemChangedCb,
		const std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )>& itemDeletedCb ) : CtInputViewGenlistItem(oldVal)
{
	__view = view;
	__inputData = inputData; __record = record; __birthdayExist = birthdayExist;
	__itemChangedCb = itemChangedCb; __itemDeletedCb = itemDeletedCb;
}

CtInputViewEventItem::~CtInputViewEventItem()
{
	WHIT();
	__destroyPopup();
}

Elm_Genlist_Item_Class* CtInputViewEventItem::getItemClassStatic()
{
	static Elm_Genlist_Item_Class itc = {};
	if( itc.item_style )
		return &itc;
	itc.item_style = "input.3icon";
	itc.func.content_get = __getContentEvent;
	itc.func.del = WGENLIST_ITEM_DEL();
	return &itc;
}

bool CtInputViewEventItem::getIsEmpty()
{
	return true;
}

Evas_Object* CtInputViewEventItem::__getContentEvent(void *data, Evas_Object *obj, const char *part)
{
	WHIT();
	CtInputViewEventItem *item = (CtInputViewEventItem*)data;

	struct tm time1 = {};
	time_t t;
	t = time(NULL);
	localtime_r(&t, &time1);

	if (0 == strcmp(part, "elm.icon.entry"))
	{
		Evas_Object *box = elm_box_add(obj);

		Evas_Object *dateTime = elm_button_add(obj);
		elm_object_style_set(dateTime, "custom_default");

		Evas_Object *edje = elm_layout_edje_get(dateTime);
		edje_object_color_class_set(edje, "button_normal", BUTTON_COLOR_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0);
		edje_object_color_class_set(edje, "button_pressed", BUTTON_COLOR_PRESSED, 0, 0, 0, 0, 0, 0, 0, 0);

		evas_object_smart_callback_add(dateTime, "clicked", __eventDateBtnClickedCb, item);
		int date = item->__inputData->getEventEvent(item->__record);

		time1.tm_year = date / 10000 - 1900;
		time1.tm_mon = (date % 10000) / 100 - 1;
		time1.tm_mday = date % 100;

		char text_str[CT_TEXT_SHORT_LEN] = { 0 };
		strftime(text_str, CT_TEXT_SHORT_LEN, EVENT_DATE_FORMAT, &time1);
		elm_object_text_set(dateTime, text_str);

		evas_object_show(dateTime);
		elm_box_pack_end(box, dateTime);

		return box;
	}
	else if (0 == strcmp(part, "elm.icon.2")) {
		Evas_Object *btn_minus = elm_button_add(obj);
		elm_object_style_set(btn_minus, "icon_expand_delete");
		evas_object_smart_callback_add(btn_minus, "clicked", __eventDeleteCb, item);

		return btn_minus;
	}
	else if (0 == strcmp(part, "elm.icon.1")) {
		Evas_Object *btn_type = elm_button_add(obj);
		elm_object_style_set(btn_type, "custom_dropdown");
		evas_object_show(btn_type);

		int eventType = item->__inputData->getEventType(item->__record);
		std::string typeStr;
		item->__inputData->getEventTypeStr(item->__record, eventType, typeStr);
		elm_object_text_set(btn_type, typeStr.c_str());

		evas_object_smart_callback_add(btn_type, "clicked", __dataTypeClickedCb, item);
		return btn_type;
	}

	return NULL;
}

void CtInputViewEventItem::__eventDateBtnClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewEventItem *item = (CtInputViewEventItem*)data;

	item->__destroyPopup();

	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_ST_BODY_SET_DATE");
	popup->addButton("IDS_PB_BUTTON_CANCEL", NULL);
	popup->addButton("IDS_PB_SK_SET", [obj, popup, item](bool* destroyPopup) {
		struct tm time1 = {};
		time_t t;
		t = time(NULL);
		localtime_r(&t, &time1);

		int year = 0;
		int month = 0;
		int day = 0;
		Evas_Object *datetimePickerBg = elm_object_content_get(popup->getEvasObj());
		Evas_Object *dateTime = elm_object_part_content_get(datetimePickerBg, "elm.swallow.content");
		elm_datetime_value_get(dateTime, &time1);
		year = time1.tm_year + 1900;
		month = time1.tm_mon + 1;
		day = time1.tm_mday;

		char text[CT_TEXT_SHORT_LEN] = {0};
		snprintf(text, sizeof(text), "%d", year * 10000 + month * 100 + day);
		char text_str[CT_TEXT_SHORT_LEN] = {0};
		strftime(text_str, CT_TEXT_SHORT_LEN, EVENT_DATE_FORMAT, &time1);
		item->__inputData->setEventEvent( item->__record, year * 10000 + month * 100 + day);
		item->updateChangeState(text);
		item->__itemChangedCb();
		elm_object_text_set( obj, text_str);

		*destroyPopup = true;
	});
	popup->setOnMoreProperties([obj, item](Evas_Object* popupObj ) {
		struct tm time1 = {};
		time_t t;
		t = time(NULL);
		localtime_r(&t, &time1);

		int date = item->__inputData->getEventEvent(item->__record);

		time1.tm_year = date / 10000 - 1900;
		time1.tm_mon = (date % 10000) / 100 - 1;
		time1.tm_mday = date % 100;

		Evas_Object* dateTime = elm_datetime_add(popupObj);
		elm_datetime_format_set(dateTime, DATE_TIME_FORMAT);
		elm_datetime_value_set(dateTime, &time1);

		std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
		Evas_Object* datetimePickerBg = elm_layout_add(popupObj);
		evas_object_size_hint_weight_set(datetimePickerBg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		CtThemeMgr::getInstance()->setObjectTheme(datetimePickerBg);
		elm_layout_file_set(datetimePickerBg, contactsEdjePath.c_str(), "datetimePickerBg");
		elm_object_part_content_set(datetimePickerBg, "elm.swallow.content", dateTime);
		evas_object_show(datetimePickerBg);

		elm_object_content_set(popupObj, datetimePickerBg);} );

	popup->create( item->__view->getWindow()->getBaseLayoutEvasObj(), NULL ); // Do not attach pop-up to view! It is not working, pop-up is destroyed too fast!

	item->__popup = popup->getWeakPtr();
}

void CtInputViewEventItem::__destroyPopup()
{
	if ( auto popup = __popup.lock() ) {
		popup->destroy();
	}
}

void CtInputViewEventItem::__dataTypeClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtInputViewEventItem* item = (CtInputViewEventItem*)data;
	std::string typeStr;

	int oldType = item->__inputData->getEventType( item->__record);
	if( oldType == CONTACTS_EVENT_TYPE_CUSTOM) {
		item->__inputData->getEventTypeStr( item->__record, CONTACTS_EVENT_TYPE_CUSTOM, typeStr);
	}

	CtInputDataTypeCtxPopup* popup = new CtInputDataTypeCtxPopup(elm_object_item_widget_get(item->getElmObjectItem()), obj, _contacts_event._uri, typeStr.c_str());
	if( *item->__birthdayExist && (oldType != CONTACTS_EVENT_TYPE_BIRTH) ) {
		popup->setRemoveBirthdayType();
	}

	popup->setOnSelectCb( [popup,item,obj,oldType](int type){

		if( CONTACTS_EVENT_TYPE_CUSTOM == type ) {
			item->__showDataTypeInputPopup();
			return;
		}

		if( type == CONTACTS_EVENT_TYPE_BIRTH ) {
			*item->__birthdayExist = true;
		}
		else {
			if( oldType == CONTACTS_EVENT_TYPE_BIRTH ) {
				*item->__birthdayExist = false;
			}
		}

		item->__inputData->setEventType( item->__record, type, NULL);
		std::string typeStr;
		item->__inputData->getEventTypeStr( item->__record, type, typeStr);
		elm_object_text_set( obj, typeStr.c_str());
		popup->destroy();
	});

	item->attachToView( popup );

}

void CtInputViewEventItem::__showDataTypeInputPopup()
{
	CtInputDataTypeCustomPopup* popup = new CtInputDataTypeCustomPopup();
	popup->setOnResultCb([this](const char* text){
		if( __inputData->getEventType( __record) == CONTACTS_EVENT_TYPE_BIRTH )
			*__birthdayExist = false;

		__inputData->setEventType( __record, CONTACTS_EVENT_TYPE_CUSTOM, (char*)text );
		elm_genlist_item_fields_update( getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
	});
	attachToView(popup);
}


void CtInputViewEventItem::__eventDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputViewEventItem* item = (CtInputViewEventItem*)data;
	if( item->__inputData->getEventType( item->__record) == CONTACTS_EVENT_TYPE_BIRTH )
		*item->__birthdayExist = false;
	item->__itemDeletedCb( item->getElmObjectItem(), item->getIsChanged(), item->__record );
	elm_object_item_del( item->getElmObjectItem() );
}
