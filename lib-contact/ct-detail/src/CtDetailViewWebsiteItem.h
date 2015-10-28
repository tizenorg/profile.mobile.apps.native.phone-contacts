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

#ifndef _CT_DETAIL_VIEW_WEBSITE_ITEM_H_
#define _CT_DETAIL_VIEW_WEBSITE_ITEM_H_

#include "WGenlistItem.h"
#include "CtDetailDataBase.h"
#include "CtString.h"

#define ICON_WEBSITE_SIZE 80

class CtDetailViewWebsiteItem : public WGenlistItem
{
public:
	CtDetailViewWebsiteItem(CtDetailDataBase *data, contacts_record_h record, bool myProfile = false)
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
		itc.item_style = "type1";
		itc.func.content_get = getContentWebsite;
		itc.func.text_get = getTextWebsite;
		itc.func.del = WGENLIST_ITEM_DEL();
		return &itc;
	}

	virtual void onSelect(void *data, Evas_Object *obj, void *event_info)
	{
		WPRET_M(!m_DetailData, "m_DetailData is NULL");

		launchWebsite(m_DetailData, m_Record);
	}
private:
	static char *getTextWebsite(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewWebsiteItem *item = (CtDetailViewWebsiteItem *)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.text")) {
			return elm_entry_utf8_to_markup( item->m_DetailData->getUrl( item->m_Record));
		}
		return NULL;
	}

	static Evas_Object *getContentWebsite(void *data, Evas_Object *obj, const char *part)
	{
		WPRET_VM(!data, NULL, "data is NULL");
		CtDetailViewWebsiteItem *item = (CtDetailViewWebsiteItem *)data;
		WPRET_VM(!item->m_DetailData, NULL, "m_DetailData is NULL");

		if (0 == strcmp(part, "elm.swallow.end")) {
			if (item->m_MyProfile) {
				return NULL;
			}

			Evas_Object *iconWebsite = elm_image_add(obj);
			elm_image_file_set(iconWebsite, ContactsCommon::getResourcePath(CT_IMG_ICON_DETAIL_INTERNET).c_str(), nullptr);
			evas_object_color_set(iconWebsite, 61, 185, 204, 255);
			elm_image_aspect_fixed_set(iconWebsite, EINA_TRUE);
			evas_object_size_hint_min_set(iconWebsite, ContactsCommon::getScaledWidth(ICON_WEBSITE_SIZE), ContactsCommon::getScaledHeight(ICON_WEBSITE_SIZE));
			evas_object_smart_callback_add(iconWebsite, "clicked", onWebsiteIconClickedCb, item);

			return iconWebsite;
		}
		return NULL;
	}

	void launchWebsite(CtDetailDataBase *detailData, contacts_record_h record)
	{
		WHIT();
		app_control_h service;
		const char *website = detailData->getUrl( record );

		app_control_create(&service);
		app_control_set_operation(service, APP_CONTROL_OPERATION_VIEW );
		if (strstr(website, "http"))
			app_control_set_uri(service, website);
		else {
			char *new_url = g_strdup_printf("http://%s", website);
			app_control_set_uri(service, new_url);
			free(new_url);
		}
		app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);

		app_control_send_launch_request(service, NULL, NULL);
		app_control_destroy(service);
	}

	static void onWebsiteIconClickedCb(void *data, Evas_Object *obj, void *event_info)
	{
		WPRET_M(!data, "data is NULL");
		CtDetailViewWebsiteItem *item = (CtDetailViewWebsiteItem *)data;
		WPRET_M(!item->m_DetailData, "m_DetailData is NULL");

		item->launchWebsite(item->m_DetailData, item->m_Record);
	}

private:
	CtDetailViewWebsiteItem();
	CtDetailDataBase *m_DetailData;
	contacts_record_h m_Record;
	bool m_MyProfile;
};

#endif /* _CT_DETAIL_VIEW_WEBSITE_ITEM_H_ */
