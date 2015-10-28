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

#ifndef _CT_INPUT_VIEW_ADDRESS_ITEM_H_
#define _CT_INPUT_VIEW_ADDRESS_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtInputDataTypeCtxPopup.h"
#include "CtInputDataTypeCustomPopup.h"
#include "CtInputViewControl.h"
#include "CtData.h"

class CtInputViewAddressItem : public CtInputViewGenlistItem
{
public:
	CtInputViewAddressItem( std::shared_ptr<CtData>& inputData, contacts_record_h record, const char* oldVal, int addedIndex,
			const std::function<void ()>& itemChangedCb,
			const std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )>& itemDeletedCb,
			const std::function<void (Evas_Object*, Elm_Object_Item* )>& activateNextEntryCb) : CtInputViewGenlistItem(oldVal)
	{
		__inputData = inputData; __record = record; __addedIndex = addedIndex;
		__itemChangedCb = itemChangedCb; __itemDeletedCb = itemDeletedCb; __activateNextEntryCb = activateNextEntryCb;
	};
	virtual ~CtInputViewAddressItem() {WHIT();}
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "input.3icon";
		itc.func.content_get = __getContentAddress;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual const char* getEntryObjPartName()
	{
		return "elm.icon.entry";
	}

private:
	static Evas_Object* __getContentAddress(void *data, Evas_Object *obj, const char *part)
	{
		WHIT();
		CtInputViewAddressItem *item = (CtInputViewAddressItem*)data;

		if (0 == strcmp(part, "elm.icon.entry")) {
			WEditfield *editfield = item->createEditField(obj, V_("IDS_PB_BODY_ADDRESS_ABB2"), true);
			Evas_Object *entry = editfield->getEntry();

			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);
			elm_entry_input_panel_imdata_set(entry, (void*)"type=post&allowEmoji=true", 25);

			evas_object_smart_callback_add(entry, "changed", __addressChangedCb, item);
			evas_object_smart_callback_add(entry, "preedit,changed", __addressChangedCb, item);
			if (INPUT_ADDED_ITEM_INDEX_ADDRESS == item->__addedIndex ) {
				evas_object_event_callback_add(entry, EVAS_CALLBACK_SHOW, __entryShowCb, NULL);
			}
			char address[CT_TEXT_MAX_LEN] = {0,};
			item->__inputData->getAddressAddress( item->__record, address, sizeof(address));
			setEntryText(entry, address);
			return editfield->getEvasObj();
		}
		else if (0 == strcmp(part, "elm.icon.2")) {
			Evas_Object *btn_minus = elm_button_add(obj);
			elm_object_style_set(btn_minus, "icon_expand_delete");
			evas_object_smart_callback_add(btn_minus, "clicked", __addressDeleteCb, item);

			return btn_minus;
		}
		else if (0 == strcmp(part, "elm.icon.1")) {
			Evas_Object *btn_type = elm_button_add(obj);
			elm_object_style_set(btn_type, "custom_dropdown");
			evas_object_show(btn_type);

			int addressType = item->__inputData->getAddressType( item->__record);
			std::string typeStr;
			item->__inputData->getAddressTypeStr( item->__record, addressType, typeStr);
			elm_object_text_set(btn_type, typeStr.c_str());
			evas_object_smart_callback_add(btn_type, "clicked", __dataTypeClickedCb, item);
			return btn_type;
		}

		return NULL;
	}

	static void __activatedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewAddressItem* item = (CtInputViewAddressItem*)data;
		item->__activateNextEntryCb( obj, item->getElmObjectItem() );
	}
	static void __entryShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
	{
		WHIT();
		elm_object_focus_set(obj, EINA_TRUE);
		evas_object_event_callback_del(obj, EVAS_CALLBACK_SHOW, __entryShowCb);
	}

	static void __dataTypeClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		WHIT();
		CtInputViewAddressItem* item = (CtInputViewAddressItem*)data;

		std::string typeStr;
		if( item->__inputData->getAddressType( item->__record) == CONTACTS_ADDRESS_TYPE_CUSTOM)
		{
			item->__inputData->getAddressTypeStr( item->__record, CONTACTS_ADDRESS_TYPE_CUSTOM, typeStr);
		}
		CtInputDataTypeCtxPopup* popup = new CtInputDataTypeCtxPopup(elm_object_item_widget_get(item->getElmObjectItem()), obj, _contacts_address._uri, typeStr.c_str());

		popup->setOnSelectCb( [popup,item,obj](int type){
			if( CONTACTS_ADDRESS_TYPE_CUSTOM == type )
			{
				item->__showDataTypeInputPopup();
				return;
			}
			item->__inputData->setAddressType( item->__record, type, NULL);
			std::string typeStr;
			item->__inputData->getAddressTypeStr( item->__record, type, typeStr);
			elm_object_text_set( obj, typeStr.c_str());
			popup->destroy();
		});

		item->attachToView( popup );
	}

	void __showDataTypeInputPopup()
	{
		CtInputDataTypeCustomPopup* popup = new CtInputDataTypeCustomPopup();
		popup->setOnResultCb([this](const char* text){
			__inputData->setAddressType( __record, CONTACTS_ADDRESS_TYPE_CUSTOM, (char*)text );
			elm_genlist_item_fields_update( getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		});
		attachToView( popup );
	}
	static void __addressChangedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewAddressItem* item = (CtInputViewAddressItem*)data;
		char *text = NULL;
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		item->__inputData->setAddressAddress(item->__record, text);
		item->updateChangeState( text );
		item->__itemChangedCb();
		SAFE_FREE(text);
	}
	static void __addressDeleteCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewAddressItem* item = (CtInputViewAddressItem*)data;
		item->updateChangeState(NULL);
		item->__itemDeletedCb( item->getElmObjectItem(), item->getIsChanged(), item->__record );
		elm_object_item_del( item->getElmObjectItem() );
	}
private:
	CtInputViewAddressItem();

	std::shared_ptr<CtData> __inputData;
	contacts_record_h __record;
	int __addedIndex;
	//
	std::function<void ()> __itemChangedCb;
	std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )> __itemDeletedCb;
	std::function<void (Evas_Object*, Elm_Object_Item*)> __activateNextEntryCb;

};

#endif /* _CT_INPUT_VIEW_ADDRESS_ITEM_H_ */
