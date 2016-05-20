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

#ifndef _CT_DETAIL_VIEW_NUMBER_ITEM_H_
#define _CT_DETAIL_VIEW_NUMBER_ITEM_H_

#include "ContactsAppControl.h"
#include "ContactsNumberItemIcons.h"
#include "CtDetailDataBase.h"
#include "CtThemeMgr.h"
#include "WGenlistItem.h"

class CtDetailViewNumberItem : public WGenlistItem
{
public:
	CtDetailViewNumberItem(CtDetailDataBase *data, contacts_record_h record, bool myProfile = false)
	{
		m_DetailData = data;
		m_Record = record;
		m_MyProfile = myProfile;
	}

public:
	virtual Elm_Genlist_Item_Class *getItemClassStatic()
	{
		static Elm_Genlist_Item_Class itc = {};
		if( itc.item_style )
			return &itc;
		itc.item_style = "type2";
		itc.func.content_get = getContentNumber;
		itc.func.text_get = getTextNumber;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual void onSelect(void *data, Evas_Object *obj, void *event_info)
	{
		WPRET_M(!m_DetailData, "m_DetailData is NULL");

		int err = launchCall(m_DetailData->getNumber(m_Record));
		WPWARN(err != APP_CONTROL_ERROR_NONE, "launchCall() failed(0x%x)", err);
	}

	contacts_record_h getRecord()
	{
		return m_Record;
	}

protected:
	using WGenlistItem::onSelect;

private:
	static Evas_Object *getContentNumber(void *data, Evas_Object *obj, const char *part)
	{
		WHIT();
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewNumberItem *item = (CtDetailViewNumberItem *)data;

		if (0 == strcmp(part, "elm.swallow.end")){
			if (item->m_MyProfile) {
				return NULL;
			}

			return createNumberItemIconsLayout(obj, item->m_DetailData->getNumber(item->m_Record));
		}
		return NULL;
	}

	static char *getTextNumber(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewNumberItem *item = (CtDetailViewNumberItem*)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.text.sub")) {
			std::string typeStr;
			int type = item->m_DetailData->getNumberType(item->m_Record);
			item->m_DetailData->getNumberTypeStr(item->m_Record, type, typeStr);
			WDEBUG("type %d text %s", type, typeStr.c_str());
			return SAFE_STRDUP(typeStr.c_str());
		} else if (0 == strcmp(part, "elm.text")) {
			return elm_entry_utf8_to_markup( item->m_DetailData->getNumber( item->m_Record ));
		}
		return NULL;
	}

private:
	CtDetailViewNumberItem();
	CtDetailDataBase *m_DetailData;
	contacts_record_h m_Record;
	bool m_MyProfile;
};


#endif /* _CT_DETAIL_VIEW_NUMBER_ITEM_H_ */
