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

#ifndef _CT_DETAIL_VIEW_ADDRESS_ITEM_H_
#define _CT_DETAIL_VIEW_ADDRESS_ITEM_H_

#include "WGenlistItem.h"
#include "CtDetailDataBase.h"

class CtDetailViewAddressItem : public WGenlistItem
{
public:
	CtDetailViewAddressItem(CtDetailDataBase *data, contacts_record_h record)
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
		itc.item_style = "multiline";
		itc.func.text_get = getTextAddress;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

private:
	static char *getTextAddress(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewAddressItem *item = (CtDetailViewAddressItem *)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.text")) {
			char text[CT_TEXT_MAX_LEN] = {0,};
			int type = item->m_DetailData->getAddressType( item->m_Record );
			std::string typeStr;

			item->m_DetailData->getAddressTypeStr( item->m_Record, type, typeStr);
			snprintf(text, sizeof(text), "<font_size=32><font color=#4D4D4D>%s</font></font_size>", typeStr.c_str());
			return SAFE_STRDUP(text);

		} else if (0 == strcmp(part, "elm.text.multiline")) {
			char text[CT_TEXT_MAX_LEN] = {0,};
			char *markupText=NULL;

			item->m_DetailData->getAddress( item->m_Record, text, sizeof(text));
			markupText = elm_entry_utf8_to_markup(text);
			snprintf(text, sizeof(text), "<font_size=40><font color=#000000>%s</font></font_size>", markupText);
			free(markupText);

			return SAFE_STRDUP(text);
		}
		return NULL;
	}

private:
	CtDetailViewAddressItem();
	CtDetailDataBase *m_DetailData;
	contacts_record_h m_Record;
};


#endif /* _CT_DETAIL_VIEW_ADDRESS_ITEM_H_ */
