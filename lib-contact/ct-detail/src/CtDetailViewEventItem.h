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

#ifndef _CT_DETAIL_VIEW_EVENT_ITEM_H_
#define _CT_DETAIL_VIEW_EVENT_ITEM_H_

#include "WGenlistItem.h"
#include "CtDetailDataBase.h"

class CtDetailViewEventItem : public WGenlistItem
{
public:
	CtDetailViewEventItem(CtDetailDataBase *data, contacts_record_h record)
	{
		m_DetailData = data;
		m_Record = record;
	}
public:
	virtual Elm_Genlist_Item_Class *getItemClassStatic()
	{
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "type2";
		itc.func.text_get = getTextEvent;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

private:

	static char *getTextEvent(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewEventItem *item = (CtDetailViewEventItem *)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.text.sub")) {
			int type = item->m_DetailData->getEventType( item->m_Record);
			std::string typeStr;
			item->m_DetailData->getEventTypeStr( item->m_Record, type, typeStr);
			return SAFE_STRDUP(typeStr.c_str());
		} else if (0 == strcmp(part, "elm.text")) {
			int date = item->m_DetailData->getEvent( item->m_Record);
			int day = date%100;
			int mon = (date%10000)/100;
			int year = date/10000;
			WPRET_VM(mon < 1 || 12 < mon || day < 1 || 31 < day, NULL, "Invalid date(mon=%d,day=%d)",mon,day);
			return item->m_DetailData->getDatetimeStr(item->m_DetailData->getDateFormater(), year, mon, day);
		}
		return NULL;
	}

private:
	CtDetailViewEventItem();
	CtDetailDataBase *m_DetailData;
	contacts_record_h m_Record;
};

#endif /* _CT_DETAIL_VIEW_EVENT_ITEM_H_ */
