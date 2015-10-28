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

#ifndef _CT_INPUT_VIEW_MSISDN_ITEM_H_
#define _CT_INPUT_VIEW_MSISDN_ITEM_H_

#include "CtInputViewGenlistItem.h"
#include "CtInputViewControl.h"
#include "CtData.h"

class CtInputViewMsisdnItem : public CtInputViewGenlistItem
{
public:
	CtInputViewMsisdnItem( std::shared_ptr<CtData>& inputData, contacts_record_h record)
	{

		__inputData = inputData; __record = record;
	};
	virtual ~CtInputViewMsisdnItem() {WHIT();}
public:
	virtual Elm_Genlist_Item_Class* getItemClassStatic() {
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "2line.bottom";
		itc.func.text_get = __getTextNumber;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

protected:
	static char* __getTextNumber(void *data, Evas_Object *obj, const char *part)
	{
		WHIT();
		WDEBUG("part %s", part);
		CtInputViewMsisdnItem *item = (CtInputViewMsisdnItem *)data;
		contacts_record_h defaultRecord = item->__record;
		if (0 == strcmp(part, "elm.text.sub.left.top")) {
			std::string typeStr;
			int type = item->__inputData->getNumberType(defaultRecord);
			item->__inputData->getNumberTypeStr(defaultRecord, type, typeStr);
			WDEBUG("type %d text %s", type, typeStr.c_str());
			return SAFE_STRDUP(typeStr.c_str());
		} else if (0 == strcmp(part, "elm.text.main.left.bottom")) {
			return elm_entry_utf8_to_markup(item->__inputData->getNumberNumber(defaultRecord));
		}
		return NULL;
	}

protected:
	CtInputViewMsisdnItem();

	std::shared_ptr<CtData> __inputData;
	contacts_record_h __record;

};

#endif /* _CT_INPUT_VIEW_MSISDN_ITEM_H_ */
