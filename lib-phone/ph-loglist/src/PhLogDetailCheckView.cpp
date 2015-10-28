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

#include "WPopup.h"

#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"

#include "PhPath.h"
#include "PhCommon.h"

#include "PhLogIcu.h"
#include "PhLogUtil.h"
#include "PhLogDbMgr.h"
#include "PhLogDefines.h"

#include "PhLogDetailCheckView.h"
#include "SystemSettingsMgr.h"

PhLogDetailCheckView::PhLogDetailCheckView(std::string number, std::string callDesc):
__itcGroupIndex(NULL),
__itcDeleteList(NULL),
__itcSelectAll(NULL),
__logList(NULL),
__checkAllButton(NULL),
__deleteButton(NULL)
{
	WHIT();

	__checkedCount = 0;
	__totalCount = 0;

	__enableTitleButton = true;
	__enableCheckAllButton = true;
	__checkedList = new std::list<PhLogDbMgr::RecordInfo*>;

	WDEBUG("Start to search Number : %s , callDesc : %s",__number.c_str(),__callDesc.c_str());

	__number = number;
	__callDesc = callDesc;
}


PhLogDetailCheckView::~PhLogDetailCheckView()
{
	WHIT();

	STD_LIST_FREE(__logList)

	if(__checkedList) delete __checkedList;

	elm_genlist_item_class_free(__itcGroupIndex);
	elm_genlist_item_class_free(__itcDeleteList);
	elm_genlist_item_class_free(__itcSelectAll);
}

void PhLogDetailCheckView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();
	WPRET_M( NULL == naviItem, "Invalid param : naviItem");

	if(__enableTitleButton)
		__enableTitleBtn(naviItem);

	__updateTitleInfo();
}

void PhLogDetailCheckView::onSetAsPageContent(Elm_Object_Item* parentNaviItem)
{
	WHIT();
	WPRET_M( NULL == parentNaviItem, "Invalid param : naviItem");

	if(__enableTitleButton)
		__enableTitleBtn(parentNaviItem);
}

void PhLogDetailCheckView::__initializeTitleInfo()
{
	WHIT();
	__checkedCount = 0;
	__totalCount = 0;

	char buf[PH_TEXT_SHORT_LEN] = {0,};
	snprintf(buf, sizeof(buf), T_("IDS_LOGS_HEADER_PD_SELECTED_ABB3"), __checkedCount);
	elm_object_item_part_text_set(getNaviItem(), "elm.text.title", buf );

	if (__enableTitleButton && __deleteButton)
			elm_object_disabled_set(__deleteButton, EINA_FALSE);

	if (NULL == __checkedList)
		__checkedList = new std::list<PhLogDbMgr::RecordInfo*>;
	else
		__checkedList->clear();
}

void PhLogDetailCheckView::__updateTitleInfo()
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

void PhLogDetailCheckView::__enableTitleBtn(Elm_Object_Item* naviItem)
{
	WHIT();
	Evas_Object * cancelButton = PhCommon::createButton(getNaviframe()->getEvasObj(), "naviframe/title_left", T_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"), [](void *data, Evas_Object *obj, void *event_info)->void {
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogDetailCheckView* p = (PhLogDetailCheckView *) data;

		WDEBUG("Close Delete View");
		p->popOut();
	}, this);
	WPRET_M( NULL == cancelButton, "PhCommon::createButton() is failed" );
	elm_object_item_part_content_set(naviItem, "title_left_btn", cancelButton);

	__deleteButton = PhCommon::createButton(getNaviframe()->getEvasObj(), "naviframe/title_right", T_("IDS_TPLATFORM_ACBUTTON_DELETE_ABB"), [](void *data, Evas_Object *obj, void *event_info)->void {
		WPRET_M( NULL == data, "Invalid Param : data");
		PhLogDetailCheckView* p = (PhLogDetailCheckView *) data;
		PhLogDbMgr::getInstance()->deleteLogList(p->__checkedList, p->__totalCount);
		if(elm_check_state_get(p->__checkAllButton) == EINA_TRUE)
		{
			WDEBUG("Need to delete detail + detail check view..");
			Eina_List *itemList = elm_naviframe_items_get(p->getNaviframe()->getEvasObj());
			Elm_Object_Item *it = (Elm_Object_Item*)eina_list_data_get(eina_list_prev(eina_list_last(itemList)));
			if(it) elm_object_item_del(it);
			eina_list_free(itemList);
		}
		WDEBUG("Close Delete View & Delete logs");
		p->popOut();
	}, this);
	WPRET_M( NULL == __deleteButton, "PhCommon::createButton() is failed" );
	elm_object_item_part_content_set(naviItem, "title_right_btn", __deleteButton);
}

void PhLogDetailCheckView::updateList(void)
{
	WHIT();
	std::list<PhLogDbMgr::RecordInfo*> *__oldLogList = __logList;

	__logList = PhLogDbMgr::getInstance()->loadDetailList( __number.c_str(), __callDesc.c_str());

	if(!__logList || __logList->empty())
		setNoContentLayout();
	else
	{
		setGenlistLayout();
		__setList();
	}
	STD_LIST_FREE(__oldLogList)
//	WDEBUG("exit :__logList : %p", __logList);
}


void PhLogDetailCheckView::__setList(void)
{
	WHIT();
	WPRET_M( NULL == __logList, "__logList is NULL");

	__initializeTitleInfo();

	if (__enableCheckAllButton) {
		getGlHelper()->appendCheckItem(__itcSelectAll, this, NULL, ELM_GENLIST_ITEM_NONE, PhLogDetailCheckView::__checkAllClickedCb, this);
	}

	PhLogDbMgr::RecordInfo *prevRecordInfo = NULL;
	for (auto it=__logList->begin(); it != __logList->end(); it++)
	{
		PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)(*it);

		if( !prevRecordInfo || !PhLogUtil::isGroupedbyDate(prevRecordInfo->logTime,recordInfo->logTime) ) {
			Elm_Object_Item *item = getGlHelper()->appendCheckItem(__itcGroupIndex, recordInfo, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_NONE);
		}

		getGlHelper()->appendCheckItem(__itcDeleteList, recordInfo, NULL, ELM_GENLIST_ITEM_NONE, PhLogDetailCheckView::__selectItemCb, this);
		prevRecordInfo = recordInfo;
	}
	getGlHelper()->appendFinish();

	__updateTitleInfo();

	return;
}

Evas_Object* PhLogDetailCheckView::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	Evas_Object* layout = PhCommon::createLayout(parent, LOG_LIST_EDJ, "log/list/check/main");
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	__setGenlistItcs();

	return layout;
}

void PhLogDetailCheckView::onCreated()
{
	WHIT();
	PhLogListView::onCreated();
	updateList();
}

void PhLogDetailCheckView::onDbChanged()
{
}

void PhLogDetailCheckView::__setGenlistItcs()
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

			if (!strcmp(part, "elm.text")) {
				return PhLogIcu::getTimeString(recordInfo->logTime);
			}
			else if (!strcmp(part, "elm.text.sub")) {
				if(PhLogUtil::isMissedCallType(recordInfo->logType))
					return strdup(T_("IDS_LOGS_SBODY_MISSEDM_CALL_STATUS"));
				else if((PhLogUtil::isOutgoingCallType(recordInfo->logType) || PhLogUtil::isIncomingCallType(recordInfo->logType))
							&& (recordInfo->callDuration == 0))
					return strdup(T_("IDS_LOGS_SBODY_CANCELLED_M_STATUS"));
				else if(PhLogUtil::isRejectCallType(recordInfo->logType))
					return strdup(T_("IDS_LOGS_SBODY_CANCELLED_M_STATUS"));
				else
					return PhLogIcu::getCallDurationString(recordInfo->callDuration);
			}
			else
				return NULL;
		};
	__itcDeleteList->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*	{
			PhLogGenlistHelper::CheckedItemData *itemData = (PhLogGenlistHelper::CheckedItemData*)data;
			PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;

			if (!strcmp(part, "elm.swallow.icon")) {
				Evas_Object *icon = PhLogUtil::getLogTypeIcon(obj, recordInfo->logType);
				WPRET_VM( NULL == icon, NULL, "PhLogUtil::getLogTypeIcon() is failed");
				elm_image_aspect_fixed_set(icon, EINA_TRUE);
				evas_object_size_hint_min_set(icon, ContactsCommon::getScaledWidth(ICON_LOG_DETAIL_WIDTH), ContactsCommon::getScaledHeight(ICON_LOG_DETAIL_HEIGHT));

				return icon;
			}
			else if (0 == strcmp(part, "elm.swallow.end")) {
				itemData->checkButton = elm_check_add(obj);
				WPRET_VM(NULL == itemData->checkButton, NULL, "elm_check_add() return NULL");
				elm_check_state_set(itemData->checkButton, itemData->isChecked);
				evas_object_repeat_events_set(itemData->checkButton, EINA_TRUE);
				evas_object_propagate_events_set(itemData->checkButton, EINA_FALSE);
				evas_object_freeze_events_set (itemData->checkButton, EINA_TRUE);
				return itemData->checkButton;
			}
			else {
				return NULL;
			}

		};
	__itcDeleteList->func.state_get = NULL;
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
		PhLogDetailCheckView *view = (PhLogDetailCheckView*)itemData->data;
		if (0 == strcmp(part, "elm.swallow.end")) {
			Evas_Object* check = elm_check_add(obj);
			elm_check_state_set(check, itemData->isChecked);
			evas_object_repeat_events_set(check, EINA_TRUE);
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_freeze_events_set (check, EINA_TRUE);
			view->__checkAllButton = check;
			return check;
		}
		else {
			return NULL;
		}
	};
	__itcSelectAll->func.del = [](void* data, Evas_Object* obj) {
		PhLogGenlistHelper::CheckedItemData* itemData = (PhLogGenlistHelper::CheckedItemData *)data;
		delete itemData;
	};

}

void PhLogDetailCheckView::__checkAllClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M( NULL == event_info,"event_info is NULL");
	WPRET_M( NULL == data,"data is NULL");

	Elm_Object_Item* item = (Elm_Object_Item*)event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);

	PhLogDetailCheckView* p = (PhLogDetailCheckView *) data;

	Eina_Bool checkAllBtnState = elm_check_state_get(p->__checkAllButton);

	elm_check_state_set(p->__checkAllButton, !checkAllBtnState);

	if ( !checkAllBtnState)
		__checkAllItems(p->getGlHelper());
	else
		__uncheckAllItems(p->getGlHelper());
}

void PhLogDetailCheckView::__checkAllItems(PhLogGenlistHelper* glHelper)
{
	WHIT();
	WPRET_M( NULL == glHelper , "glHelper is NULL");

	Evas_Object *genlist = glHelper->getEvasObj();
	WPRET_M( NULL == genlist , "genlist is NULL");

	Elm_Object_Item *item = elm_genlist_first_item_get(genlist);
	while(item)
	{
		PhLogGenlistHelper::CheckedItemData *itemData =  (PhLogGenlistHelper::CheckedItemData *)elm_object_item_data_get(item);

		PhLogDetailCheckView* p = (PhLogDetailCheckView *)itemData->p;
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

void PhLogDetailCheckView::__uncheckAllItems(PhLogGenlistHelper* glHelper)
{
	WHIT();
	WPRET_M( NULL == glHelper , "glHelper is NULL");

	Evas_Object *genlist = glHelper->getEvasObj();
	WPRET_M( NULL == genlist , "genlist is NULL");

	Elm_Object_Item *item = elm_genlist_first_item_get(genlist);
	while(item)
	{
		PhLogGenlistHelper::CheckedItemData *itemData =  (PhLogGenlistHelper::CheckedItemData *)elm_object_item_data_get(item);

		PhLogDetailCheckView* p = (PhLogDetailCheckView *)itemData->p;
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

void PhLogDetailCheckView::__checkItem(PhLogGenlistHelper::CheckedItemData * itemData)
{
	WHIT();
	WPRET_M( NULL == itemData , "Invalid Param : itemData");

	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	WPRET_M( NULL == recordInfo , "recordInfo is NULL");

	PhLogDetailCheckView *p = (PhLogDetailCheckView *)itemData->p;
	WPRET_M( NULL == p , "p is NULL");

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

	elm_check_state_set(itemData->checkButton, itemData->isChecked);
	WHIT();

	p->__updateTitleInfo();
}

void PhLogDetailCheckView::__uncheckItem(PhLogGenlistHelper::CheckedItemData * itemData)
{
	WHIT();
	WPRET_M( NULL == itemData , "Invalid Param : itemData");

	PhLogDbMgr::RecordInfo *recordInfo = (PhLogDbMgr::RecordInfo*)itemData->data;
	WPRET_M( NULL == recordInfo , "recordInfo is NULL");

	PhLogDetailCheckView *p = (PhLogDetailCheckView *)itemData->p;
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


void PhLogDetailCheckView::__selectItemCb(void *data, Evas_Object *obj, void *event_info)
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
