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

#ifndef _CT_INPUT_VIEW_RELATION_ITEM_H_
#define _CT_INPUT_VIEW_RELATION_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtInputDataTypeCtxPopup.h"
#include "CtInputDataTypeCustomPopup.h"
#include "CtInputViewControl.h"
#include "CtData.h"
#include "CtPath.h"
#include "ContactsCommon.h"

#define CONTACTS_BTN_ICON_SIZE	80

class CtInputViewRelationItem : public CtInputViewGenlistItem
{
public:
	CtInputViewRelationItem( CtInputViewControl* control, std::shared_ptr<CtData>& inputData, contacts_record_h record, const char* oldVal,
			const std::function<void ()>& itemChangedCb,
			const std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )>& itemDeletedCb,
			const std::function<void (Evas_Object*, Elm_Object_Item* )>& activateNextEntryCb)
		: CtInputViewGenlistItem(oldVal)
	{
		__control = control; __inputData = inputData; __record = record;
		__itemChangedCb = itemChangedCb; __itemDeletedCb = itemDeletedCb; __activateNextEntryCb = activateNextEntryCb;
	};
	virtual ~CtInputViewRelationItem() { WHIT(); }
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "input.relationship";
		itc.func.content_get = __getContentRelation;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual const char* getEntryObjPartName()
	{
		return "elm.icon.entry";
	}

private:
	static Evas_Object* __getContentRelation(void *data, Evas_Object *obj, const char *part)
	{
		WHIT();
		CtInputViewRelationItem *item = (CtInputViewRelationItem*)data;
		item->focus();
		//
		if (0 == strcmp(part, "elm.icon.entry")) {
			WEditfield *editfield = item->createEditField(obj, V_("IDS_PB_OPT_RELATIONSHIP_ABB2"), false);
			Evas_Object *entry = editfield->getEntry();

			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_EMAIL);

			evas_object_smart_callback_add(entry, "changed", __relationshipChangedCb, item);
			evas_object_smart_callback_add(entry, "preedit,changed", __relationshipChangedCb, item);
			evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
			if (INPUT_ADDED_ITEM_INDEX_RELATIONSHIP == item->__control->getAddedIndex()) {
				evas_object_event_callback_add(entry, EVAS_CALLBACK_SHOW, __entryShowCb, NULL);
			}

			const char *address = item->__inputData->getRelationshipRelationship(item->__record);
			setEntryText(entry, address);
			return editfield->getEvasObj();
		}
		else if (0 == strcmp(part, "elm.icon.2")) {
			Evas_Object *btn_minus = elm_button_add(obj);
			elm_object_style_set(btn_minus, "icon_expand_delete");
			evas_object_smart_callback_add(btn_minus, "clicked", __relationshipDeleteCb, item);

			return btn_minus;
		}
		else if (0 == strcmp(part, "elm.icon.1")) {
			Evas_Object *btn_type = elm_button_add(obj);
			elm_object_style_set(btn_type, "custom_dropdown");
			evas_object_show(btn_type);

			int relationshipType = item->__inputData->getRelationshipType(item->__record);
			std::string typeStr;
			item->__inputData->getRelationshipTypeStr(item->__record, relationshipType, typeStr);
			elm_object_text_set(btn_type, typeStr.c_str());

			evas_object_smart_callback_add(btn_type, "clicked", __dataTypeClickedCb, item);
			return btn_type;
		} else if (0 == strcmp(part, "elm.icon.3")) {
			Evas_Object *image = elm_image_add(obj);
			const std::string edjPath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);
			elm_image_file_set(image, edjPath.c_str(), GROUP_BTN_IC_MULTIPLE);
			evas_object_size_hint_min_set(image, ContactsCommon::getScaledWidth(CONTACTS_BTN_ICON_SIZE), ContactsCommon::getScaledHeight(CONTACTS_BTN_ICON_SIZE));
			evas_object_propagate_events_set(image, EINA_FALSE);
			evas_object_smart_callback_add(image, "clicked", __relationshipSelectContact, item);
			return image;
		}
		return NULL;
	}

	static void __activatedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewRelationItem* item = (CtInputViewRelationItem*)data;
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
		CtInputViewRelationItem* item = (CtInputViewRelationItem*)data;
		//
		std::string typeStr;
		if( item->__inputData->getRelationshipType( item->__record) == CONTACTS_RELATIONSHIP_TYPE_CUSTOM)
		{
			item->__inputData->getRelationshipTypeStr( item->__record, CONTACTS_RELATIONSHIP_TYPE_CUSTOM, typeStr);
		}
		CtInputDataTypeCtxPopup* popup = new CtInputDataTypeCtxPopup(elm_object_item_widget_get(item->getElmObjectItem()), obj, _contacts_relationship._uri, typeStr.c_str());

		popup->setOnSelectCb( [popup,item,obj](int type){
			if( CONTACTS_RELATIONSHIP_TYPE_CUSTOM == type )
			{
				item->__showDataTypeInputPopup();
				return;
			}
			item->__inputData->setRelationshipType( item->__record, type, NULL);
			std::string typeStr;
			 item->__inputData->getRelationshipTypeStr( item->__record, type, typeStr);
			elm_object_text_set( obj, typeStr.c_str());
			popup->destroy();
		});

		item->attachToView( popup );
	}

	void __showDataTypeInputPopup()
	{
		CtInputDataTypeCustomPopup* popup = new CtInputDataTypeCustomPopup();
		popup->setOnResultCb([this](const char* text){
			__inputData->setRelationshipType( __record, CONTACTS_RELATIONSHIP_TYPE_CUSTOM, (char*)text );
			elm_genlist_item_fields_update( getElmObjectItem(), "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		});
		attachToView( popup );
	}
	static void __relationshipChangedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewRelationItem* item = (CtInputViewRelationItem*)data;
		char *text = NULL;
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		item->__inputData->setRelationshipRelationship(item->__record, text);
		item->updateChangeState(text);
		item->__itemChangedCb();
		SAFE_FREE(text);
	}
	static void __relationshipDeleteCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewRelationItem* item = (CtInputViewRelationItem*)data;
		item->updateChangeState(NULL);
		item->__itemDeletedCb( item->getElmObjectItem(), item->getIsChanged(), item->__record );
		elm_object_item_del( item->getElmObjectItem() );
	}
	static void __relationshipSelectContact(void *data, Evas_Object *obj, void *event_info)
	{
		launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_SINGLE,
				APP_CONTROL_DATA_TYPE_ID, __contactPickCb, data);
	}

	static void __contactPickCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
	{
		CtInputViewRelationItem *item = (CtInputViewRelationItem*)data;
		char **personIds = NULL;
		int count = 0;
		int ret = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &personIds, &count);

		if (ret == APP_CONTROL_ERROR_NONE && personIds && personIds[0]) {
			contacts_record_h record = NULL;
			char *displayName = NULL;

			int err = contacts_db_get_record(_contacts_person._uri, atoi(personIds[0]), &record);
			WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_get_record() failed(%d)", err);

			err = contacts_record_get_str_p(record, _contacts_person.display_name, &displayName);
			WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_str_p() failed(%d)", err);

			ctCommonRemoveNewline(displayName);
			item->__inputData->setRelationshipRelationship(item->__record, displayName);
			elm_genlist_item_fields_update(item->getElmObjectItem(), "elm.icon.entry", ELM_GENLIST_ITEM_FIELD_CONTENT);

			contacts_record_destroy(record, true);

			for (int i = 0; i < count; ++i) {
				free(personIds[i]);
			}
		}

		free(personIds);
	}

private:
	CtInputViewRelationItem();

	CtInputViewControl* __control;
	std::shared_ptr<CtData> __inputData;
	contacts_record_h __record;

	std::function<void ()> __itemChangedCb;
	std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )> __itemDeletedCb;
	std::function<void (Evas_Object*, Elm_Object_Item*)> __activateNextEntryCb;

};

#endif /* _CT_INPUT_VIEW_RELATION_ITEM_H_ */
