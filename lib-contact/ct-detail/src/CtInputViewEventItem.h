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

#ifndef _CT_INPUT_VIEW_EVENT_ITEM_H_
#define _CT_INPUT_VIEW_EVENT_ITEM_H_

#include <memory>
#include "CtInputViewControl.h"
#include "CtInputViewGenlistItem.h"
#include "CtDetailDataBase.h"
#include "WControl.h"
#include "CtData.h"

class CtInputData;

class CtInputViewEventItem : public CtInputViewGenlistItem
{
public:
	CtInputViewEventItem( WView* view, std::shared_ptr<CtData>& inputData, contacts_record_h record, const char* oldVal, std::shared_ptr<bool>& birthdayExist,
			const std::function<void ()>& itemChangedCb,
			const std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )>& itemDeletedCb );
	virtual ~CtInputViewEventItem();
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	virtual bool getIsEmpty();

private:
	virtual void onLanguageChanged() {};
private:
	void __destroyPopup();
	void __showDataTypeInputPopup();

	static Evas_Object* __getContentEvent(void *data, Evas_Object *obj, const char *part);
	static char* __eventDateFormatGet();
	static void __eventDateBtnClickedCb(void *data, Evas_Object *obj, void *event_info);
	static void __dataTypeClickedCb(void *data, Evas_Object *obj, void *event_info);
	static void __eventDeleteCb(void *data, Evas_Object *obj, void *event_info);
private:
	CtInputViewEventItem();

	WView* __view;
	std::shared_ptr<CtData> __inputData;
	contacts_record_h __record;
	WUiObjectPtr __popup;
	contacts_event_type_e __type;
	std::shared_ptr<bool> __birthdayExist;

	std::function<void ()> __itemChangedCb;
	std::function<void (Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record )> __itemDeletedCb;
};

#endif /* _CT_INPUT_VIEW_EVENT_ITEM_H_ */
