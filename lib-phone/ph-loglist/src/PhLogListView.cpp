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

#include <string>

#include "ContactsDebug.h"
#include "ContactsLocalization.h"

#include "PhLogIcu.h"
#include "PhLogGenlistHelper.h"

#include "PhLogListView.h"
#include "PhCommon.h"
#include "SystemSettingsMgr.h"
#include "PhLogDbMgr.h"

#define PH_TEXT_MAX_LEN (1024)

PhLogListView::PhLogListView():
__glHelper(NULL)
{
	WHIT();

	PhLogIcu::setLocale();
	PhLogIcu::setTimeFormat();
	PhLogIcu::setDateFormat();
	PhLogIcu::setDefaultTimezoneId();
}

PhLogListView::~PhLogListView()
{
	WHIT();
}

PhLogGenlistHelper *PhLogListView::getGlHelper()
{
	if (!__glHelper)
	{
		WASSERT_EX(getEvasObj() != NULL, "getGlHelper failed. (parent layout is null).");
		__glHelper = new PhLogGenlistHelper(getEvasObj());
		__glHelper->create(getEvasObj(), NULL);
		__glHelper->setName("__genlistHelper");
		WDEBUG("Created PhLogGenlistHelper");
	}
	return __glHelper;
}

void PhLogListView::setGenlistLayout()
{
	WPRET_M( !getGlHelper(), "getGlHelper() is failed");

	Evas_Object * obj  = elm_object_part_content_get(getEvasObj(), "elm.swallow.content");
	if( __glHelper->getEvasObj() != obj)
	{
		obj  = elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
		if(obj)
			evas_object_del(obj);
		elm_object_part_content_set(getEvasObj(), "elm.swallow.content", __glHelper->getEvasObj());
	}
}

void PhLogListView::setNoContentLayout()
{
	WENTER();

	Evas_Object * obj = elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
	if (obj)
	{
		evas_object_del(obj);
		__glHelper = NULL;
	}

	Evas_Object *noContentsLayout = elm_layout_add(getEvasObj());
	elm_layout_theme_set(noContentsLayout, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContentsLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContentsLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_domain_translatable_part_text_set(noContentsLayout, "elm.text", TEXT_DOMAIN, "IDS_CLOG_BODY_NO_LOGS");
	elm_object_domain_translatable_part_text_set(noContentsLayout, "elm.help.text", TEXT_DOMAIN, "IDS_LOGS_BODY_AFTER_YOU_MAKE_OR_RECEIVE_CALLS_THEY_WILL_BE_LOGGED_HERE");
	elm_layout_signal_emit(noContentsLayout, "text,disabled", "");
	elm_layout_signal_emit(noContentsLayout, "align.center", "elm");

	elm_object_part_content_set(getEvasObj(), "elm.swallow.content", noContentsLayout);

}

void PhLogListView::onTabSelect(Elm_Object_Item *naviItem)
{
	WHIT();
	elm_object_focus_set(getEvasObj(), EINA_TRUE);
}

void PhLogListView::onTabUnselect(Elm_Object_Item *naviItem)
{
	WHIT();
	destroyPopup();
}

void PhLogListView::formatTimeDateChangedCb(system_settings_key_e key, void *data)
{
	WENTER();
	WPRET_M(NULL == data, "data is NULL");
	PhLogListView* p = (PhLogListView*)data;

	PhLogIcu::setLocale();
	PhLogIcu::setTimeFormat();
	PhLogIcu::setDateFormat();
	PhLogIcu::setDefaultTimezoneId();
	p->updateList();
}

void PhLogListView::onCreated()
{
	WHIT();
	int ret = SystemSettingsMgr::getInstance().registerChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, formatTimeDateChangedCb, this);
	WPWARN(-1 == ret, "system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR) Failed");

	ret = SystemSettingsMgr::getInstance().registerChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, formatTimeDateChangedCb, this);
	WPWARN(-1 == ret, "system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY) Failed");

	ret = SystemSettingsMgr::getInstance().registerChangedCallback(SYSTEM_SETTINGS_KEY_TIME_CHANGED, formatTimeDateChangedCb, this);
	WPWARN(-1 == ret, "system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED) Failed");

	PhLogDbMgr::getInstance()->attach(this);
}

void PhLogListView::onDestroy()
{
	WHIT();

	int ret = SystemSettingsMgr::getInstance().unregisterChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR, formatTimeDateChangedCb);
	WPWARN(-1 == ret, "system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR) Failed");

	ret = SystemSettingsMgr::getInstance().unregisterChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, formatTimeDateChangedCb);
	WPWARN(-1 == ret, "system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY) Failed");

	ret = SystemSettingsMgr::getInstance().unregisterChangedCallback(SYSTEM_SETTINGS_KEY_TIME_CHANGED, formatTimeDateChangedCb);
	WPWARN(-1 == ret, "system_settings_unset_changed_cb(SYSTEM_SETTINGS_KEY_TIME_CHANGED) Failed");

	PhLogDbMgr::getInstance()->detach(this);
}
