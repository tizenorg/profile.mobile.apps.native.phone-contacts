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

#ifndef _CT_DETAIL_VIEW_EMAIL_ITEM_H_
#define _CT_DETAIL_VIEW_EMAIL_ITEM_H_

#include "WGenlistItem.h"
#include "ContactsAppControl.h"
#include "CtDetailDataBase.h"

#define ICON_EMAIL_SIZE 80
#define ICON_EMAIL_COLOR 61, 185, 204, 255

class CtDetailViewEmailItem : public WGenlistItem
{
public:
	CtDetailViewEmailItem(CtDetailDataBase *data, contacts_record_h record, bool myProfile = false)
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
		itc.func.content_get = getContentEmail;
		itc.func.text_get = getTextEmail;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual void onSelect(void *data, Evas_Object *obj, void *event_info)
	{
		WPRET_M(!data, "data is NULL");
		CtDetailViewEmailItem *item = (CtDetailViewEmailItem *)data;
		WPRET_M(!item->m_DetailData, "m_DetailData is NULL");

		int err = launchMessageComposer("mailto:", item->m_DetailData->getEmail(item->m_Record));
		WPWARN(err != APP_CONTROL_ERROR_NONE, "launchMessageComposer() failed(%d)", err);
	}

private:
	static Evas_Object *getContentEmail(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewEmailItem *item = (CtDetailViewEmailItem *)data;

		if (0 == strcmp(part, "elm.swallow.end")) {
			if (item->m_MyProfile) {
				return NULL;
			}

			Evas_Object *iconEmail = elm_image_add(obj);
			elm_image_file_set(iconEmail, ContactsCommon::getResourcePath(CT_IMG_ICON_DETAIL_EMAIL).c_str(), nullptr);
			evas_object_color_set(iconEmail, ICON_EMAIL_COLOR);
			elm_image_aspect_fixed_set(iconEmail, EINA_TRUE);
			evas_object_size_hint_min_set(iconEmail, ContactsCommon::getScaledWidth(ICON_EMAIL_SIZE), ContactsCommon::getScaledHeight(ICON_EMAIL_SIZE));

			return iconEmail;
		}
		return NULL;
	}

	static char *getTextEmail(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewEmailItem *item = (CtDetailViewEmailItem *)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.text.sub")) {
			int type = item->m_DetailData->getEmailType(item->m_Record);
			std::string typeStr;
			item->m_DetailData->getEmailTypeStr(item->m_Record, type, typeStr);
			return SAFE_STRDUP(typeStr.c_str());
		} else if (0 == strcmp(part, "elm.text")) {
			return elm_entry_utf8_to_markup(item->m_DetailData->getEmail(item->m_Record));
		}
		return NULL;
	}

private:
	CtDetailViewEmailItem();
	CtDetailDataBase *m_DetailData;
	contacts_record_h m_Record;
	bool m_MyProfile;
};

#endif /* _CT_DETAIL_VIEW_EMAIL_ITEM_H_ */
