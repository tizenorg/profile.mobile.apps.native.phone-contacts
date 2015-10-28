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

#ifndef _CT_INPUT_VIEW_NOTE_ITEM_H_
#define _CT_INPUT_VIEW_NOTE_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtInputViewControl.h"
#include "CtData.h"
#include "CtString.h"

class CtInputViewNoteItem : public CtInputViewGenlistItem
{
public:
	CtInputViewNoteItem( std::shared_ptr<CtData>& inputData, contacts_record_h record, const char* oldVal, int addedIndex,
			const std::function<void ()>& itemChangedCb,
			const std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )>& itemDeletedCb,
			const std::function<void (Evas_Object*, Elm_Object_Item* )>& activateNextEntryCb) : CtInputViewGenlistItem(oldVal)
	{
		__inputData = inputData; __record = record; __addedIndex = addedIndex;
		__itemChangedCb = itemChangedCb; __itemDeletedCb = itemDeletedCb; __activateNextEntryCb = activateNextEntryCb;
	};
	virtual ~CtInputViewNoteItem() { WHIT(); }
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "input.2icon";
		itc.func.content_get = __getContentNote;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual const char* getEntryObjPartName()
	{
		return "elm.icon.entry";
	}

private:
	static Evas_Object* __getContentNote(void *data, Evas_Object *obj, const char *part)
	{
		//WHIT();
		CtInputViewNoteItem *item = (CtInputViewNoteItem*)data;

		if (0 == strcmp(part, "elm.icon.entry"))
		{
			WEditfield *editfield = item->createEditField(obj, V_("IDS_PB_BUTTON_NOTES"), true);
			Evas_Object *entry = editfield->getEntry();

			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);
			elm_entry_input_panel_imdata_set(entry, (void*)"allowEmoji=true", 15);

			evas_object_smart_callback_add(entry, "changed", __noteChangedCb, item);
			evas_object_smart_callback_add(entry, "preedit,changed", __noteChangedCb, item);
			evas_object_smart_callback_add(entry, "activated", __activatedCb, item);
			if (INPUT_ADDED_ITEM_INDEX_NOTE == item->__addedIndex) {
				evas_object_event_callback_add(entry, EVAS_CALLBACK_SHOW, __entryShowCb, NULL);
			}

			const char *note = item->__inputData->getNote( item->__record);
			setEntryText(entry, note);
			return editfield->getEvasObj();
		}
		else if (0 == strcmp(part, "elm.icon.2"))
		{
			Evas_Object *btn_minus = elm_button_add(obj);
			elm_object_part_content_set(obj, "elm.icon.edit", btn_minus);
			elm_object_style_set(btn_minus, "icon_expand_delete");
			evas_object_smart_callback_add(btn_minus, "clicked", __noteDeleteCb, item);

			return btn_minus;
		}

		return NULL;
	}

	static void __entryShowCb(void *data, Evas *e, Evas_Object *obj, void *event_info)
	{
		WHIT();
		elm_object_focus_set(obj, EINA_TRUE);
		evas_object_event_callback_del(obj, EVAS_CALLBACK_SHOW, __entryShowCb);
	}

	static void __noteChangedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewNoteItem* item = (CtInputViewNoteItem*)data;
		char *text = NULL;
		text = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
		item->__inputData->setNote(item->__record, text);
		item->updateChangeState( text );
		item->__itemChangedCb();
		SAFE_FREE(text);
	}

	static void __noteDeleteCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewNoteItem* item = (CtInputViewNoteItem*)data;
		item->updateChangeState(NULL);
		item->__itemDeletedCb( item->getElmObjectItem(), item->getIsChanged(), item->__record );
		elm_object_item_del( item->getElmObjectItem() );
	}

	static void __activatedCb(void *data, Evas_Object *obj, void *event_info)
	{
		CtInputViewNoteItem* item = (CtInputViewNoteItem*)data;
		item->__activateNextEntryCb( obj, item->getElmObjectItem() );
	}
private:
	CtInputViewNoteItem();

	std::shared_ptr<CtData> __inputData;
	contacts_record_h __record;
	int __addedIndex;

	std::function<void ()> __itemChangedCb;
	std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )> __itemDeletedCb;
	std::function<void (Evas_Object*, Elm_Object_Item*)> __activateNextEntryCb;
};

#endif /* _CT_INPUT_VIEW_NOTE_ITEM_H_ */
