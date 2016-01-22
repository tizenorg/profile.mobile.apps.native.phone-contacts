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

//#include <ui_extension.h>
#include <notification.h>

#include <sstream>
#include <system_settings.h>
#include "badge.h"

#include "WMenuPopup.h"

#include "ContactsAppControl.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "ContactsThumbnail.h"
#include "WPopup.h"
#include "PhPath.h"
#include "PhCommon.h"

#include "PhLogIcu.h"
#include "PhLogUtil.h"
#include "PhLogDbMgr.h"

#include "PhLogDropDownCtxPopup.h"
#include "PhLogDetailView.h"
#include "PhLogMainListCheckView.h"
#include "PhLogMainListView.h"
#include "PhLogDefines.h"
#include "SystemSettingsMgr.h"

PhLogMainListView::PhLogMainListView():
__logList(NULL),
__dbUpdateTimer(NULL),
__isActive(false),
__isScrollStopped(true),
__isUpdateImmediate(true)
{
	WHIT();
	__currentListType = PhLogDbMgr::LIST_TYPE_ALL_CALL;
	__enableViewByButton = true;

	elm_theme_extension_add(NULL, ContactsCommon::getResourcePath(LOG_LIST_EDJ).c_str());
	__initIcuPattern();
}

PhLogMainListView::~PhLogMainListView()
{
	STD_LIST_FREE(__logList)

	elm_genlist_item_class_free(__itcGroupIndex);
	elm_genlist_item_class_free(__itcMainList);
}

void PhLogMainListView::updateList()
{
	#define LOGS_FIRST_PAGE_PRELOAD_COUNT	8
	WDEBUG("enter :__logList : %p", __logList);

	std::list<PhLogDbMgr::RecordInfo*> *__oldLogList = __logList;
	int limit = (__logList)? 0 : LOGS_FIRST_PAGE_PRELOAD_COUNT;
	__logList = PhLogDbMgr::getInstance()->loadLogList(__currentListType, limit);

	if(!__logList || __logList->empty())
	{
		Evas_Object * obj = elm_object_part_content_get(getEvasObj(), "elm.swallow.content");
		if (obj && getGlHelper()->getEvasObj() == obj)
		{
			elm_object_part_content_unset(getEvasObj(), "elm.swallow.content");
			elm_genlist_clear(obj);
			evas_object_hide(obj);
		}
		setNoContentLayout();
	}
	else
	{
		setGenlistLayout();
		__setList();
	}

	if(__oldLogList == NULL)
	{
		WUiTimer::addIdler([](void* data)->Eina_Bool {
			PhLogMainListView* p = (PhLogMainListView*)data;
			WINFO("First Full db loading..");
			p->updateList();
			return ECORE_CALLBACK_CANCEL;
		}, this, getWeakPtr());	
	}

	STD_LIST_FREE(__oldLogList)
//	WDEBUG("exit :__logList : %p", __logList);
}

void PhLogMainListView::__genlistUnswallowCb(void *data, Evas_Object* obj, void *event_info)
{
	//WHIT();
 	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	PhLogMainListView *p = (PhLogMainListView *)data;
	WPRET_M(NULL == item, "parameter(item) is NULL");
	PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)elm_object_item_data_get(item);
	std::vector<Evas_Object *>* cList = (std::vector<Evas_Object *>*)itemData->contents;
	WPRET_M(NULL == cList, "parameter(cList) is NULL");

	if(0 == p->__logCacheList.size()) return;	// genlist already destroyed case

	for (unsigned i=0; i<cList->size(); i++)
	{
		Evas_Object* content = cList->at(i);
		std::map<Evas_Object*,std::string>::iterator it = p->__logCacheList.find(content);
		if(it != p->__logCacheList.end())
		{
			if((it->second).back() != '*')
				WWARNING("invalid delete try!!!!!");
			else
				(it->second).erase((it->second).end() -1);
			evas_object_hide(content);
		}
		else
			evas_object_del(content);
	}
	cList->clear();
}

Evas_Object* PhLogMainListView::__createListContents(std::string contentsType)
{
	Evas_Object *genlist = getGlHelper()->getEvasObj();
	Evas_Object *icon = NULL;
	Evas_Object *layout = NULL;
	WPRET_VM( NULL == genlist, NULL, "genlist is NULL");

	if(contentsType.find("list/B/log/thumbnail/") != std::string::npos)
	{
		for(int j=CONTACTS_PLOG_TYPE_VOICE_INCOMING ; j<CONTACTS_PLOG_TYPE_VIDEO_BLOCKED ;j++)
		{
			std::stringstream keyString;
			keyString << "list/B/log/thumbnail/" <<	PhLogUtil::getLogTypeImageName(j);

			if(keyString.str() != contentsType)
				continue;


			Evas_Object *thumbnail = createThumbnail(genlist, THUMBNAIL_98, NULL, true);
			WPRET_VM( NULL == thumbnail,NULL, "elm_image_add() is failed");
			icon = PhLogUtil::getLogTypeIcon(genlist, j);
			WPRET_VM( NULL == icon, NULL,"PhLogUtil::GetLogTypeIcon() is failed");
			evas_object_size_hint_min_set(icon, ContactsCommon::getScaledWidth(LOG_LIST_CALL_TYPE_SIZE), ContactsCommon::getScaledHeight(LOG_LIST_CALL_TYPE_SIZE));
			layout = PhCommon::createLayout(genlist, LOG_LIST_EDJ, "list/B/log/thumbnail");
			WPRET_VM( NULL == layout,NULL, "PhLogUtil::createLayout() is failed");
			elm_layout_content_set(layout, "elm.swallow.content", thumbnail);
			elm_layout_content_set(layout, "elm.swallow.call.type", icon);
			//WDEBUG("%s : %x", keyString.str().c_str(), layout);
			break;
		}
	}
	else if(contentsType == "list/C/log/log_info")
	{
		// create info button
		layout = PhCommon::createImage(genlist, LOG_LIST_EDJ, "list/C/log/log_info");
		evas_object_size_hint_min_set(layout, ContactsCommon::getScaledWidth(LOG_LIST_INFO_BTN_SIZE), ContactsCommon::getScaledHeight(LOG_LIST_INFO_BTN_SIZE));
		WPRET_VM( NULL == layout,NULL, "PhCommon::createLayout() is failed");
		evas_object_repeat_events_set(layout, EINA_FALSE);
		evas_object_propagate_events_set(layout, EINA_FALSE);
	}
	else
		WWARNING("Type error %s", contentsType.c_str());

	evas_object_hide(layout);
	return layout;
}

Evas_Object* PhLogMainListView::__getCachedContents(std::string keyString)
{
	std::map<Evas_Object*,std::string>::iterator iTer;
	for (iTer = __logCacheList.begin(); iTer != __logCacheList.end(); iTer ++)
	{
        if (iTer->second == keyString)
        {
        	Evas_Object* contents = iTer->first;
			(iTer->second).push_back('*');
        	return contents;
        }
	}
	Evas_Object* newContents = __createListContents(keyString);
	__logCacheList[newContents] = keyString+"*";
	WDEBUG("Cache Added : %s (total count : %d)", keyString.c_str(), __logCacheList.size());
	return newContents;
}

void PhLogMainListView::__setList(void)
{
	WHIT();
	WPRET_M( NULL == __logList, "__logList is NULL");
	elm_genlist_clear(getGlHelper()->getEvasObj());
	static bool initGenlistCallback = false;

	if(initGenlistCallback == false)
	{
		Evas_Object *genlist = getGlHelper()->getEvasObj();
		evas_object_smart_callback_add(genlist, "content,unswallowed", __genlistUnswallowCb, this);

		evas_object_smart_callback_add(genlist, "scroll,anim,start", [](void *data, Evas_Object *obj, void *event_info){
				//WDEBUG("scroll,anim,start");
				bool* __isScrollStopped = (bool*)data;
				*__isScrollStopped = false;
 			}, &__isScrollStopped);
 		evas_object_smart_callback_add(genlist, "scroll,anim,stop", [](void *data, Evas_Object *obj, void *event_info){
				//WDEBUG("scroll,anim,stop");
 				PhLogMainListView* p = (PhLogMainListView*)data;
				p->__isScrollStopped = true;
				Eina_List *l = NULL;
				void *itm = NULL;
				Eina_List* realized_list = elm_genlist_realized_items_get(obj);
				EINA_LIST_FOREACH(realized_list, l, itm) {
					//WDEBUG("elm_genlist_realized_items_get %x", itm);
					Elm_Object_Item *it = (Elm_Object_Item*)itm;
					PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)elm_object_item_data_get(it);
					PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
					if(itemData->thumbnailUpdateTimer)
					{
						ecore_timer_del(itemData->thumbnailUpdateTimer);
						itemData->thumbnailUpdateTimer = NULL;
					}

					if(!recordInfo->imgPath || strlen(recordInfo->imgPath) == 0)
						continue;

				 	itemData->thumbnailUpdateTimer = ecore_timer_add(0.3, [](void* data)->Eina_Bool {
						//WDEBUG("realize timer");
						Elm_Object_Item *item = (Elm_Object_Item *)data;
						PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)elm_object_item_data_get(item);
						itemData->thumbnailUpdateTimer = NULL;
						elm_genlist_item_update(item);
				 		return ECORE_CALLBACK_CANCEL;
					}, it);
				}
 			}, this);
		evas_object_smart_callback_add(genlist, "unrealized", [](void *data, Evas_Object *obj, void *event_info){
			//WDEBUG("unrealized");
			Elm_Object_Item *item = (Elm_Object_Item *)event_info;
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)elm_object_item_data_get(item);

			if(itemData->thumbnailUpdateTimer)
				ecore_timer_del(itemData->thumbnailUpdateTimer);
			itemData->thumbnailUpdateTimer = NULL;
		}, NULL);
		initGenlistCallback = true;
	}

	PhLogDbMgr::RecordInfo *prevRecordInfo = NULL;
	for (auto it=__logList->begin(); it != __logList->end(); it++)
	{
		PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)(*it);

		if( PhLogUtil::isGrouped(prevRecordInfo,recordInfo))
		{
			if( NULL == prevRecordInfo->groupedList )
				prevRecordInfo->groupedList = new std::list<PhLogDbMgr::RecordInfo*>;

			prevRecordInfo->groupedList->push_back(recordInfo);
		}
		else
		{
			if( !prevRecordInfo || !PhLogUtil::isGroupedbyDate(prevRecordInfo->logTime, recordInfo->logTime) ) {
				Elm_Object_Item *item = getGlHelper()->appendItem(__itcGroupIndex, recordInfo, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
				elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
			}

			Elm_Object_Item* item = getGlHelper()->appendItem(
	                        __itcMainList,
	                        recordInfo,
	                        NULL,
	                        ELM_GENLIST_ITEM_NONE,
	                        PhLogMainListView::__selectItemCb,
	                        this);

			if (!recordInfo->number) {
				elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
			}
			else {
				elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DEFAULT);
			}

			prevRecordInfo = recordInfo;
		}
	}

	getGlHelper()->appendFinish();

	return;
}

void PhLogMainListView::__initIcuPattern()
{
	WUiTimer::addTimer(1.5,[](void* data)->Eina_Bool {
		WDEBUG("__initIcuPattern");

		return ECORE_CALLBACK_CANCEL;
	}, NULL, getWeakPtr());	
}

Evas_Object* PhLogMainListView::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();
	Evas_Object* layout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/list/check/main");
	WPRET_VM( NULL == layout, NULL, "PhCommon::createLayout() is failed");

	__setGenlistItcs();
	
	return layout;
}

void PhLogMainListView::onCreated(void)
{
	WHIT();
	PhLogListView::onCreated();
	updateList();
}

void PhLogMainListView::onDestroy()
{
	WHIT();

	if (getWindow()) {
		evas_object_smart_callback_del_full(getWindow()->getEvasObj(), "focus,in", __windowFocusInCb, this);
	}

	if(getGlHelper()->getEvasObj())
		evas_object_smart_callback_del(getGlHelper()->getEvasObj(), "content,unswallowed", __genlistUnswallowCb);

	for (std::map<Evas_Object* ,std::string>::iterator iTer = __logCacheList.begin(); iTer != __logCacheList.end(); iTer ++)
		evas_object_del(iTer->first);
	__logCacheList.clear();
	PhLogListView::onDestroy();

}

void PhLogMainListView::onSetAsPageContent(Elm_Object_Item* parentNaviItem)
{
	WHIT();

	if (getWindow()) {
		evas_object_smart_callback_add(getWindow()->getEvasObj(), "focus,in", __windowFocusInCb, this);
	}
}

void PhLogMainListView::onDbChanged()
{
	WENTER();
	if(__dbUpdateTimer)
	{
		__dbUpdateTimer->destroy();
		__dbUpdateTimer = NULL;
	}
	if(__isUpdateImmediate == true)
	{
		updateList();
		__isUpdateImmediate = false;
		WUiTimer::addTimer(3.0, [](void* data)->Eina_Bool {
			PhLogMainListView* p = (PhLogMainListView*)data;
			WINFO("dbchange-fist timer");
			p->__isUpdateImmediate = true;
			return ECORE_CALLBACK_CANCEL;
		}, this, getWeakPtr());	
	}
	else
	{
		__dbUpdateTimer = WUiTimer::addTimer(1.0, [](void* data)->Eina_Bool {
			PhLogMainListView* p = (PhLogMainListView*)data;
			WINFO("dbchange update timer");
			p->updateList();
			p->__dbUpdateTimer = NULL;
			return ECORE_CALLBACK_CANCEL;
		}, this, getWeakPtr());	
	}
}

void PhLogMainListView::onTabSelect(Elm_Object_Item *naviItem)
{
	WDEBUG("onTabSelect");

	__isActive = true;
	destroyPopup();
	elm_object_focus_set(getEvasObj(), EINA_TRUE);
	__setMoreBtn(naviItem);
	__resetMissedCallCount();
}

void PhLogMainListView::onTabUnselect(Elm_Object_Item *naviItem)
{
	WHIT();
	__isActive = false;

	// NOTE: jaehwa
	// Moved terminating app control to CtTabView
	// destroy app control only if tab changed
	// PhCommon::terminateChildAppControl();

      destroyPopup();
}

bool PhLogMainListView::onTabPop()
{
	WHIT();
	destroyPopup();
	return TRUE;
}

int PhLogMainListView::__getListPosition()
{
	WHIT();
	Evas_Coord_Rectangle rect = {0};
	evas_object_geometry_get(getGlHelper()->getEvasObj(), &rect.x, &rect.y, &rect.w, &rect.h);

	int posret;
	Elm_Object_Item* it = elm_genlist_at_xy_item_get(getGlHelper()->getEvasObj(), rect.x, rect.y + 30, &posret);
	return (it)? elm_genlist_item_index_get(it) : 0;
}

void PhLogMainListView::__resetMissedCallCount()
{
	WHIT();
	unsigned count = 0;
	if(badge_get_count(PHONE_PKG, &count) == BADGE_ERROR_NONE && count > 0) {
		badge_set_count(PHONE_PKG, 0);
	}

	notification_delete_all(NOTIFICATION_TYPE_NOTI);
	PhLogDbMgr::getInstance()->updateMissedLogDbAsSeen(0, CONTACTS_PLOG_TYPE_NONE);
}

void PhLogMainListView::__selectDetailButtonCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M( NULL == data, "Invalid Param : data");
	PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	PhLogMainListView* p = (PhLogMainListView *)itemData->p;

	WNaviframe* frame = (WNaviframe*)p->getNaviframe();
	frame->push(new PhLogDetailView(recordInfo));
	////CONTACTS_LOGGING_LOG(LOGGING_LOG_DETAIL_LOG);
}

void PhLogMainListView::__setGenlistItcs()
{
	WHIT();
	__itcGroupIndex = elm_genlist_item_class_new();
	WPRET_M(!__itcGroupIndex, "elm_genlist_item_class_new() failed");
	__itcGroupIndex->item_style = "group_index";
	__itcGroupIndex->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
			if (!strcmp(part, "elm.text")) {
				PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
				PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;

				time_t itemTime = recordInfo->logTime;
				time_t nowTime = time(NULL);

				struct tm itemDate;
				struct tm nowDate;

				localtime_r(&itemTime, &itemDate);
				localtime_r(&nowTime, &nowDate);

				if( itemDate.tm_year == nowDate.tm_year &&
					itemDate.tm_mon == nowDate.tm_mon &&
					itemDate.tm_mday == nowDate.tm_mday )
					return elm_entry_utf8_to_markup(T_("IDS_LOGS_HEADER_TODAY"));
				else if( itemDate.tm_year == nowDate.tm_year &&
						 itemDate.tm_mon == nowDate.tm_mon &&
						 ((itemDate.tm_mday)+1) == nowDate.tm_mday )
					return elm_entry_utf8_to_markup(T_("IDS_LOGS_HEADER_YESTERDAY"));
				else
					return PhLogIcu::getGroupIndexDateString(recordInfo->logTime);
			}
			else
				return NULL;
		};
	__itcGroupIndex->func.content_get = NULL;
	__itcGroupIndex->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::ItemData* itemData = (PhLogGenlistHelper::ItemData *)data;
		delete itemData;
	};

	__itcMainList = elm_genlist_item_class_new();
	WPRET_M(!__itcMainList, "elm_genlist_item_class_new() failed");
	__itcMainList->item_style = "type1";
	__itcMainList->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
			if (0 == strcmp(part, "elm.text"))
				return elm_entry_utf8_to_markup(PhLogUtil::getDisplayText(recordInfo->personId, recordInfo->displayName, recordInfo->number,recordInfo->callDesc));
			else if ((strcmp(part, "elm.text.sub") == 0))
			{
				if(recordInfo->personId)
					return elm_entry_utf8_to_markup(recordInfo->number);
				else
					return elm_entry_utf8_to_markup(T_("IDS_LOGS_SBODY_UNSAVED_M_STATUS"));
			}
			else if ((strcmp(part, "elm.text.sub.end") == 0))
			{
				char buf[PH_TEXT_SHORT_LEN] = {0,};
				snprintf(buf, sizeof(buf), "<font_size=%d>%s</font_size>", LOGLIST_SUB_TEXT_SIZE, recordInfo->displayLogTime);
				return strdup(buf);
			}
			else if ((strcmp(part, "elm.text.end") == 0))
			{
				char buf[PH_TEXT_SHORT_LEN] = {0,};
				if(recordInfo->groupedList == NULL)
					return NULL;
				else
				{
					snprintf(buf, sizeof(buf), "(%d)", (int) (recordInfo->groupedList->size()+1));
					return elm_entry_utf8_to_markup(buf);
				}
			}
			else
				return NULL;
		};
	__itcMainList->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*	{
			PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
			PhLogMainListView* p = (PhLogMainListView *)itemData->p;
			std::vector<Evas_Object*>* cList = (std::vector<Evas_Object*>*)itemData->contents;
			if(cList == NULL)
				itemData->contents = cList = new std::vector<Evas_Object*>;

			if (0 == strcmp(part, "elm.swallow.icon"))
			{
				//WDEBUG("itemData %x", itemData);
				//WDEBUG("elm.icon.1");
				char* imagePath = NULL;
				if((recordInfo->imgPath && (strlen(recordInfo->imgPath) != 0)) && p->__isScrollStopped == true)
					imagePath = recordInfo->imgPath;

				if(!imagePath || strlen(imagePath) == 0)
				{
					std::stringstream keyString;
					Evas_Object* contents;
					keyString << "list/B/log/thumbnail/" << PhLogUtil::getLogTypeImageName(recordInfo->logType);
					if((contents = p->__getCachedContents(keyString.str())) != NULL)
					{
						cList->push_back(contents);
						return contents;
					}
				}

				Evas_Object *thumbnail = createThumbnail(obj, THUMBNAIL_98, recordInfo->imgPath, true);
				Evas_Object *icon = PhLogUtil::getLogTypeIcon(obj, recordInfo->logType);
				WPRET_VM( NULL == icon, NULL, "PhLogUtil::GetLogTypeIcon() is failed");
				evas_object_size_hint_min_set(icon, ContactsCommon::getScaledWidth(LOG_LIST_CALL_TYPE_SIZE), ContactsCommon::getScaledHeight(LOG_LIST_CALL_TYPE_SIZE));

				Evas_Object *layout = PhCommon::createLayout(obj, LOG_LIST_EDJ, "list/B/log/thumbnail");
				WPRET_VM( NULL == layout, NULL, "PhLogUtil::createLayout() is failed");

				elm_layout_content_set(layout, "elm.swallow.content", thumbnail);
				elm_layout_content_set(layout, "elm.swallow.call.type", icon);

				cList->push_back(layout);
				return layout;
			}
			else if (0 == strcmp(part, "elm.swallow.end"))
			{
				Evas_Object* contents = p->__getCachedContents("list/C/log/log_info");
				WPRET_VM( NULL == contents, NULL, "__getCachedContents() is failed");
				evas_object_smart_callback_del(contents, "clicked", __selectDetailButtonCb);
				evas_object_smart_callback_add(contents, "clicked", __selectDetailButtonCb, itemData);
				cList->push_back(contents);

				return contents;
			} else {
				return NULL;
			}

		};
	__itcMainList->func.state_get = NULL;
	__itcMainList->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::ItemData* itemData = (PhLogGenlistHelper::ItemData *)data;
		if(itemData->contents)
			delete (std::vector<Evas_Object*>*)itemData->contents;
		//WDEBUG("delcallback item %x", itemData);
		delete itemData;
	};
	//uxt_genlist_set_content_reuse_enabled(__itcMainList, EINA_TRUE);

}

void PhLogMainListView::__selectItemCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M( NULL == event_info,"event_info is NULL");
	WPRET_M( NULL == data,"data is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);

	PhLogGenlistHelper::ItemData *itemData = (PhLogGenlistHelper::ItemData *)elm_object_item_data_get(item);
	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	WDEBUG("address prt itemData : %p recordInfo : %p",itemData, recordInfo );

	int err = launchCall(recordInfo->number);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "launchCall() failed(0x%x)", err);
}

void PhLogMainListView::__setMoreBtn(Elm_Object_Item *naviItem)
{
	WHIT();
	WPRET_M(!naviItem, "Invalid Param : naviItem");

	enableMoreButton(naviItem, [](void *data, Evas_Object *obj, void *event_info){
		PhLogMainListView* p = static_cast<PhLogMainListView*>(data);

		Evas_Object* win = NULL;
		if (p->getWindow())
			win = p->getWindow()->getEvasObj();

		WPRET_M(NULL == win, "win is NULL");

		WMenuPopup* popup = new WMenuPopup();
		popup->prepare( win, p->getNaviframe()->getEvasObj() );

		if( p->__logList && !p->__logList->empty())
		{
			popup->appendItem(T_("IDS_CLOG_OPT_VIEW_BY"), [p](){
				p->__createViewBy();
			});

			popup->appendItem(T_("IDS_LOGS_HEADER_DELETE"), [p](){
				WNaviframe* frame = (WNaviframe*)p->getNaviframe();
				frame->push(new PhLogMainListCheckView(p->__currentListType, p->__getListPosition()));
			});
		} else if (p->__currentListType == PhLogDbMgr::ListType::LIST_TYPE_MISSED_CALL) {
			popup->appendItem(T_("IDS_CLOG_OPT_VIEW_BY"), [p](){
				p->__createViewBy();
			});
		}

		popup->appendItem(T_("IDS_KPD_OPT_CALL_SETTINGS_ABB"), [p](){
			PhCommon::launchCallSetting();
			////CONTACTS_LOGGING_LOG(LOGGING_LOG_CALL_SETTINGS);
		});
		p->attachPopup(popup);
	}, this);
}

void PhLogMainListView::__createViewBy()
{
	WENTER();
	Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
	WPRET_M(!itc, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")) {
			if (PhLogDbMgr::LIST_TYPE_ALL_CALL == (long) data )
				return SAFE_STRDUP(T_("IDS_LOGS_BODY_ALL_CALLS"));
			else if (PhLogDbMgr::LIST_TYPE_MISSED_CALL == (long) data)
				return SAFE_STRDUP(T_("IDS_LOGS_NPBODY_MISSED_CALLS_ABB"));
		}
		return NULL;
	};

	itc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		PhLogMainListView* p = static_cast<PhLogMainListView*>(evas_object_data_get(obj, "LogMainList"));
		WPRET_VM(!p, NULL, "LogMainList is NULL");

		if(!strcmp(part, "elm.swallow.end")) {

			Evas_Object* radio = elm_radio_add(obj);
			Evas_Object *radioGroup = (Evas_Object*)evas_object_data_get(obj, "radioGroup");
			elm_object_style_set(radio, "list");
			elm_radio_group_add(radio, radioGroup);
			elm_radio_state_value_set(radio, ((long) data));
			evas_object_propagate_events_set(radio, EINA_TRUE);

			if(p->__currentListType == (long) data) {
				elm_radio_value_set(radio, (long) data);
			}
			return radio;
		}
		return NULL;
	};

	WPopup* popup = new WPopup();
	popup->setTitle(T_("IDS_CLOG_OPT_VIEW_BY"));
	popup->setContent([itc, this](Evas_Object* parent)->Evas_Object*
	{
		Evas_Object* genlist = elm_genlist_add(parent);
		evas_object_data_set(genlist, "LogMainList", this);
		elm_object_style_set( genlist, "popup");
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE );

		Evas_Object* radioGroup = elm_radio_add(genlist);
		elm_object_style_set(radioGroup, "list");
		elm_radio_state_value_set(radioGroup, -1);
		elm_radio_value_set(radioGroup, -1);
		evas_object_data_set(genlist, "radioGroup", radioGroup);

		elm_genlist_item_append(genlist, itc, (void *)PhLogDbMgr::ListType::LIST_TYPE_ALL_CALL, NULL,
				ELM_GENLIST_ITEM_NONE,this->__onViewByCallsCb, this);
		elm_genlist_item_append(genlist, itc, (void *)PhLogDbMgr::ListType::LIST_TYPE_MISSED_CALL,
				NULL, ELM_GENLIST_ITEM_NONE,this->__onViewByCallsCb, this);
		elm_object_content_set(parent, genlist);
		evas_object_show(genlist);
		return genlist;
	});

	this->attachPopup(popup);
	elm_genlist_item_class_free(itc);
}

void PhLogMainListView::__onViewByCallsCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	PhLogMainListView* p = static_cast<PhLogMainListView*>(data);
	Evas_Object *radioGroup = (Evas_Object*)evas_object_data_get(obj, "radioGroup");
	PhLogDbMgr::ListType index = (PhLogDbMgr::ListType)((long) elm_object_item_data_get((Elm_Object_Item *) event_info));
	p->__currentListType = index;
	elm_radio_value_set(radioGroup, p->__currentListType);
	p->updateList();
	p->destroyPopup();
}

void PhLogMainListView::__windowFocusInCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	PhLogMainListView* p = (PhLogMainListView*)data;
	WPRET_M(!p->__isActive, "PhLogMainListView is not active");

	p->__resetMissedCallCount();
}
