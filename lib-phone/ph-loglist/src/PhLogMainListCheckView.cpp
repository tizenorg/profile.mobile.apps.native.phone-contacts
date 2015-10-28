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

#include <system_settings.h>
#include <unordered_set>

#include "WPopup.h"

#include "ContactsDebug.h"
#include "ContactsLocalization.h"
#include "ContactsThumbnail.h"

#include "PhPath.h"
#include "PhCommon.h"

#include "PhLogDefines.h"
#include "PhLogIcu.h"
#include "PhLogUtil.h"

#include "PhLogMainListCheckView.h"
#include "SystemSettingsMgr.h"

PhLogMainListCheckView::PhLogMainListCheckView(PhLogDbMgr::ListType currentListType, int listPosition):
__itcGroupIndex(NULL),
__itcDeleteList(NULL),
__itcSelectAll(NULL),
__logList(NULL),
__checkAllButton(NULL),
__deleteButton(NULL),
__checkedList(NULL),
__listInitialPosition(listPosition)
{
	WHIT();
	__currentListType = currentListType;

	__enableTitleButton = true;
	__enableCheckAllButton = true;

	__checkedCount = 0;
	__totalCount = 0;
}


PhLogMainListCheckView::~PhLogMainListCheckView()
{
	WHIT();

	STD_LIST_FREE(__logList)

	delete __checkedList;

	elm_genlist_item_class_free(__itcGroupIndex);
	elm_genlist_item_class_free(__itcDeleteList);
	elm_genlist_item_class_free(__itcSelectAll);
}

void PhLogMainListCheckView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();
	WPRET_M( NULL == naviItem, "Invalid param : naviItem");

	__enableTitleBtn();
	__updateTitleInfo();

	Elm_Object_Item *topItem = elm_genlist_nth_item_get(getGlHelper()->getEvasObj(), __listInitialPosition);
	//WDEBUG("postion %d", __listInitialPosition);
	if(__listInitialPosition != 1 && topItem)
		elm_genlist_item_show(topItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void PhLogMainListCheckView::onSetAsPageContent(Elm_Object_Item* parentNaviItem)
{
	WHIT();
}

void PhLogMainListCheckView::__initializeTitleInfo()
{
	WHIT();

	if(!__newRecord)
	{
		__checkedCount = 0;
		__totalCount = 0;
	}


	char buf[PH_TEXT_SHORT_LEN] = {0,};
	snprintf(buf, sizeof(buf), T_("IDS_LOGS_HEADER_PD_SELECTED_ABB3"), __checkedCount);
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", buf );

	if (__enableTitleButton && __deleteButton)
			elm_object_disabled_set(__deleteButton, EINA_FALSE);

	if (NULL == __checkedList)
		__checkedList = new std::list<PhLogDbMgr::RecordInfo*>;
	else
	{
		__checkedList->clear();
	}
}


void PhLogMainListCheckView::__updateTitleInfo()
{
	WHIT();
	char buf[PH_TEXT_SHORT_LEN] = {0,};
	snprintf(buf, sizeof(buf), T_("IDS_LOGS_HEADER_PD_SELECTED_ABB3"), __checkedCount);
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", buf );

	if (__deleteButton)
	{
		if(__checkedCount == 0)
			elm_object_disabled_set(__deleteButton, EINA_TRUE);
		else if (__checkedCount >0)
			elm_object_disabled_set(__deleteButton, EINA_FALSE);
		else
			WERROR("invalid __checkedCount : %d", __checkedCount);
	}

	if (__checkAllButton)
	{
		Elm_Object_Item *item = elm_genlist_first_item_get(getGlHelper()->getEvasObj());
		PhLogGenlistHelper::CheckedItemData* itemData =  (PhLogGenlistHelper::CheckedItemData*)elm_object_item_data_get(item);
		WPRET_M(!itemData, "itemData is NULL");

		if ( (unsigned int)__totalCount == __logList->size() &&  !__logList->empty()) {
			itemData->isChecked = EINA_TRUE;
			elm_check_state_set(__checkAllButton, EINA_TRUE);
		}
		else {
			itemData->isChecked = EINA_FALSE;
			elm_check_state_set(__checkAllButton, EINA_FALSE);
		}
	}
}

void PhLogMainListCheckView::__enableTitleBtn()
{
	WHIT();
	WPRET_M( !__enableTitleButton , "__enableTitleButton is disabled");

	Evas_Object * cancelButton = elm_object_item_part_content_unset(getNaviItem(), "title_cancel");
	if(cancelButton)
		evas_object_del(cancelButton);
	cancelButton = PhCommon::createButton(getNaviframe()->getEvasObj(), "naviframe/title_left", T_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"), [](void *data, Evas_Object *obj, void *event_info)->void {
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogMainListCheckView* p = (PhLogMainListCheckView *) data;

		WDEBUG("Close Delete View");
		p->popOut();
	}, this);
	WPRET_M( NULL == cancelButton, "PhCommon::createButton() is failed" );
	elm_object_item_part_content_set(getNaviItem(), "title_left_btn", cancelButton);

	__deleteButton = elm_object_item_part_content_unset(getNaviItem(), "title_right_btn");
	if(__deleteButton)
		evas_object_del(__deleteButton);

	__deleteButton = PhCommon::createButton(getNaviframe()->getEvasObj(), "naviframe/title_right", T_("IDS_TPLATFORM_ACBUTTON_DELETE_ABB"), [](void *data, Evas_Object *obj, void *event_info)->void {
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogMainListCheckView* p = (PhLogMainListCheckView *) data;

		elm_object_disabled_set(obj, EINA_TRUE);

		PhLogDbMgr::getInstance()->deleteLogList(p->__checkedList, p->__totalCount);
		p->popOut();

	}, this);
	WPRET_M( NULL == __deleteButton, "PhCommon::createButton() is failed" );
	elm_object_item_part_content_set(getNaviItem(), "title_right_btn", __deleteButton);
}

void PhLogMainListCheckView::updateList(void)
{
	WHIT();
	std::list<PhLogDbMgr::RecordInfo*> *__oldLogList = __logList;

	__logList = PhLogDbMgr::getInstance()->loadLogList(__currentListType);
	if(!__logList || __logList->empty())
		setNoContentLayout();
	else
	{
		__newRecord = NULL;
		if(__oldLogList)
		{
			std::unordered_set<int> logIds;
			for(auto oldRec: *__oldLogList)
			{
				logIds.insert(oldRec->logId);
			}
			for(auto newRec: *__logList)
			{
				if( !logIds.count(newRec->logId) )
				{
					__newRecord = newRec;
				}
			}
		}

		setGenlistLayout();
		__setList();
	}

	STD_LIST_FREE(__oldLogList)
//	WDEBUG("exit :__logList : %p", __logList);

}

void PhLogMainListCheckView::__setList()
{
	WHIT();
	WPRET_M( NULL == __logList, "__logList is NULL");

	std::unordered_set<int> checkedHashSet;
	if(__newRecord && __checkedList)
	{
		for(auto i: *__checkedList)
		{
			checkedHashSet.insert(i->logId);
			if( PhLogUtil::isGrouped(__newRecord, i))
			{
				--__checkedCount;
				__totalCount -=  i->groupedList ? (i->groupedList->size() + 1) : 1;
			}
		}
	}

	__initializeTitleInfo();

	if (__enableCheckAllButton) {
		getGlHelper()->appendCheckItem(__itcSelectAll, this, NULL, ELM_GENLIST_ITEM_NONE, PhLogMainListCheckView::__checkAllClickedCb, this);
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
			if( !prevRecordInfo || !PhLogUtil::isGroupedbyDate(prevRecordInfo->logTime,recordInfo->logTime) ) {
				Elm_Object_Item *item = getGlHelper()->appendCheckItem(__itcGroupIndex, recordInfo, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
				elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
			}
			Eina_Bool isChecked = EINA_FALSE;

			if(checkedHashSet.count(recordInfo->logId))
			{
				__checkedList->push_back(recordInfo);
				isChecked = EINA_TRUE;
			}
			getGlHelper()->appendCheckItem(__itcDeleteList, recordInfo, NULL, ELM_GENLIST_ITEM_NONE, PhLogMainListCheckView::__selectItemCb, this, isChecked);
			prevRecordInfo = recordInfo;
		}
	}

	getGlHelper()->appendFinish();

	__updateTitleInfo();

	return;
}

Evas_Object* PhLogMainListCheckView::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	Evas_Object* layout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/list/check/main");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_FILL);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(layout);

	__setGenlistItcs();
	return layout;
}

void PhLogMainListCheckView::onCreated()
{
	WHIT();
	PhLogListView::onCreated();
	updateList();
}

void PhLogMainListCheckView::onDbChanged()
{
	updateList();
}

void PhLogMainListCheckView::__setGenlistItcs()
{
	WHIT();

	__itcGroupIndex = elm_genlist_item_class_new();
	WPRET_M(!__itcGroupIndex, "elm_genlist_item_class_new() failed");
	__itcGroupIndex->item_style = "group_index";
	__itcGroupIndex->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
//		WDEBUG("part :%s", part);
		if (!strcmp(part, "elm.text")) {
			PhLogGenlistHelper::CheckedItemData *itemData = (PhLogGenlistHelper::CheckedItemData*)data;
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
		PhLogGenlistHelper::CheckedItemData* itemData = (PhLogGenlistHelper::CheckedItemData *)data;
		delete itemData;
	};

	__itcDeleteList = elm_genlist_item_class_new();
	WPRET_M(!__itcDeleteList, "elm_genlist_item_class_new() failed");
	__itcDeleteList->item_style = "type1";
	__itcDeleteList->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
		PhLogGenlistHelper::CheckedItemData *itemData = (PhLogGenlistHelper::CheckedItemData*)data;
		PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
//		WDEBUG("part :%s", part);
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
			char *logTime = PhLogIcu::getTimeString(recordInfo->logTime);
			snprintf(buf, sizeof(buf), "<font_size=%d>%s</font_size>", LOGLIST_SUB_TEXT_SIZE, logTime);
			free(logTime);
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
	__itcDeleteList->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*{
		PhLogGenlistHelper::CheckedItemData *itemData = (PhLogGenlistHelper::CheckedItemData*)data;
		PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
//		WDEBUG("part :%s", part);
		if (0 == strcmp(part, "elm.swallow.icon"))
		{
			Evas_Object *thumbnail = createThumbnail(obj, THUMBNAIL_98, recordInfo->imgPath, true);
			WPRET_VM( NULL == thumbnail, NULL, "elm_image_add() is failed");

			Evas_Object *icon = PhLogUtil::getLogTypeIcon(obj, recordInfo->logType);
			WPRET_VM( NULL == icon, NULL, "PhLogUtil::GetLogTypeIcon() is failed");
			evas_object_size_hint_min_set(icon, ContactsCommon::getScaledWidth(LOG_LIST_CALL_TYPE_SIZE), ContactsCommon::getScaledHeight(LOG_LIST_CALL_TYPE_SIZE));

			Evas_Object *layout = PhCommon::createLayout(obj, LOG_LIST_EDJ, "list/B/log/thumbnail");
			WPRET_VM( NULL == layout, NULL, "PhLogUtil::createLayout() is failed");

			elm_layout_content_set(layout, "elm.swallow.content", thumbnail);
			elm_layout_content_set(layout, "elm.swallow.call.type", icon);

			return layout;
		}
		else if (0 == strcmp(part, "elm.swallow.end"))
		{
			itemData->checkButton = elm_check_add(obj);
			WPRET_VM(NULL == itemData->checkButton, NULL, "elm_check_add() return NULL");
			elm_check_state_set(itemData->checkButton, itemData->isChecked);
			evas_object_repeat_events_set(itemData->checkButton, EINA_TRUE);
			evas_object_propagate_events_set(itemData->checkButton, EINA_FALSE);
			evas_object_freeze_events_set (itemData->checkButton, EINA_TRUE);

			return itemData->checkButton;
		} else {
			return NULL;
		}
	};
	__itcDeleteList->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::CheckedItemData* itemData = (PhLogGenlistHelper::CheckedItemData *)data;
		delete itemData;
	};

	__itcSelectAll = elm_genlist_item_class_new();
	WPRET_M(!__itcSelectAll, "elm_genlist_item_class_new() failed");
	__itcSelectAll->item_style = "type1";
	__itcSelectAll->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
		if (!strcmp(part, "elm.text")) {
			return strdup(T_("IDS_LOGS_MBODY_SELECT_ALL"));
		}
		else
			return NULL;
	};
	__itcSelectAll->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*{
		PhLogGenlistHelper::CheckedItemData* itemData = (PhLogGenlistHelper::CheckedItemData *)data;
		PhLogMainListCheckView *view = (PhLogMainListCheckView*)itemData->data;
		if (0 == strcmp(part, "elm.swallow.end"))
		{
			Evas_Object* check = elm_check_add(obj);
			elm_check_state_set(check, itemData->isChecked);
			evas_object_repeat_events_set(check, EINA_TRUE);
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_freeze_events_set (check, EINA_TRUE);
			view->__checkAllButton = check;
			return check;
		}
		else
			return NULL;
	};
	__itcSelectAll->func.del = [](void* data, Evas_Object* obj){
		PhLogGenlistHelper::CheckedItemData* itemData = (PhLogGenlistHelper::CheckedItemData *)data;
		delete itemData;
	};
}

void PhLogMainListCheckView::__checkAllClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M( NULL == event_info,"event_info is NULL");
	WPRET_M( NULL == data,"data is NULL");

	Elm_Object_Item* item = (Elm_Object_Item*)event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);

	PhLogMainListCheckView * p = (PhLogMainListCheckView *)data;
	Eina_Bool checkAllBtnState = elm_check_state_get(p->__checkAllButton);

	elm_check_state_set(p->__checkAllButton, !checkAllBtnState);

	if ( !checkAllBtnState)
		__checkAllItems(p->getGlHelper());
	else
		__uncheckAllItems(p->getGlHelper());
}

void PhLogMainListCheckView::__checkAllItems(PhLogGenlistHelper* glHelper)
{
	WHIT();
	WPRET_M( NULL == glHelper , "glHelper is NULL");

	Evas_Object *genlist = glHelper->getEvasObj();
	WPRET_M( NULL == genlist , "genlist is NULL");

	Elm_Object_Item *item = elm_genlist_first_item_get(genlist);
	while(item)
	{
		PhLogGenlistHelper::CheckedItemData *itemData =  (PhLogGenlistHelper::CheckedItemData *)elm_object_item_data_get(item);

		PhLogMainListCheckView* p = (PhLogMainListCheckView *)itemData->p;
		if (p) {
			if (p->__itcSelectAll == (Elm_Genlist_Item_Class*)elm_genlist_item_item_class_get(item)) {
				item = elm_genlist_item_next_get(item);
				continue;
			}
		}

		if (!itemData->isChecked)
			__checkItem(itemData);

		item = elm_genlist_item_next_get(item);
	}
}

void PhLogMainListCheckView::__uncheckAllItems(PhLogGenlistHelper* glHelper)
{
	WHIT();
	WPRET_M( NULL == glHelper , "glHelper is NULL");

	Evas_Object *genlist = glHelper->getEvasObj();
	WPRET_M( NULL == genlist , "genlist is NULL");

	Elm_Object_Item *item = elm_genlist_first_item_get(genlist);
	while(item)
	{
		PhLogGenlistHelper::CheckedItemData *itemData =  (PhLogGenlistHelper::CheckedItemData *)elm_object_item_data_get(item);

		PhLogMainListCheckView* p = (PhLogMainListCheckView *)itemData->p;
		if (p) {
			if (p->__itcSelectAll == (Elm_Genlist_Item_Class*)elm_genlist_item_item_class_get(item)) {
				item = elm_genlist_item_next_get(item);
				continue;
			}
		}

		if (itemData->isChecked)
			__uncheckItem(itemData);

		item = elm_genlist_item_next_get(item);
	}
}

void PhLogMainListCheckView::__checkItem(PhLogGenlistHelper::CheckedItemData * itemData)
{
	WHIT();
	WPRET_M( NULL == itemData , "Invalid Param : itemData");

	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	WPRET_M( NULL == recordInfo , "recordInfo is NULL");

	PhLogMainListCheckView *p = (PhLogMainListCheckView *)itemData->p;
	WPRET_M( NULL == p , "p is NULL : is Group index");

//	WDEBUG("number  : %s", recordInfo->number);
	p->__checkedList->push_back(recordInfo);
	p->__checkedCount++;
	if(recordInfo->groupedList)
		p->__totalCount += (recordInfo->groupedList->size()+1);
	else
		p->__totalCount++;

	//check button state set
	itemData->isChecked = EINA_TRUE;
	if(itemData->checkButton)
		elm_check_state_set(itemData->checkButton, itemData->isChecked);

	p->__updateTitleInfo();
}

void PhLogMainListCheckView::__uncheckItem(PhLogGenlistHelper::CheckedItemData * itemData)
{
	WHIT();
	WPRET_M( NULL == itemData , "Invalid Param : itemData");

	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	WPRET_M( NULL == recordInfo , "recordInfo is NULL");

	PhLogMainListCheckView *p = (PhLogMainListCheckView *)itemData->p;
	WPRET_M( NULL == p , "p is NULL");

	p->__checkedList->remove(recordInfo);
	p->__checkedCount--;

	if(recordInfo->groupedList)
		p->__totalCount -= (recordInfo->groupedList->size()+1);
	else
		p->__totalCount--;

	//check button state set
	itemData->isChecked = EINA_FALSE;
	if(itemData->checkButton)
		elm_check_state_set(itemData->checkButton, itemData->isChecked);

	p->__updateTitleInfo();
}


void PhLogMainListCheckView::__selectItemCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M( NULL == event_info,"event_info is NULL");
	WPRET_M( NULL == data,"data is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);

	PhLogGenlistHelper::CheckedItemData *itemData = (PhLogGenlistHelper::CheckedItemData *)elm_object_item_data_get(item);

	if(itemData->isChecked)
		__uncheckItem(itemData);
	else
		__checkItem(itemData);
}

void PhLogMainListCheckView::onTabSelect(Elm_Object_Item *naviItem)
{
	WHIT();
	elm_object_focus_set(getEvasObj(), EINA_TRUE);

	__enableTitleBtn();

	__updateTitleInfo();

}

void PhLogMainListCheckView::onTabUnselect(Elm_Object_Item *naviItem)
{
	WHIT();
	destroyPopup();
	__uncheckAllItems(getGlHelper());
}

bool PhLogMainListCheckView::onTabPop()
{
	WHIT();
	return TRUE;
}

