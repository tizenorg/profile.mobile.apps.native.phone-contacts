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

#include "PhLogGenlistHelper.h"
#include "ContactsDebug.h"

#define PH_LOG_SCROLL_TIMER 0.1
#define PH_LOG_REALIZED_ITEM_COUNT_PER_PAGE 10

PhLogGenlistHelper::PhLogGenlistHelper(Evas_Object *parent):
__genlist(NULL),
__parent(parent),
__swipeHandler(NULL),
__scrollTimer(NULL),
__isClearRequested(false),
__smartAppendIndex(0)

{
	WHIT();
}

PhLogGenlistHelper::~PhLogGenlistHelper()
{
	WHIT();

}

Evas_Object* PhLogGenlistHelper::onCreate( Evas_Object* parent, void* param )
{
	WHIT();
	return __createGenlist();
}

void PhLogGenlistHelper::onDestroy()
{
	WHIT();
	if (__scrollTimer)
		ecore_timer_del(__scrollTimer);
}

void PhLogGenlistHelper::__swipeStartCb(void *data, Elm_Object_Item *it, const char *em, const char *src)
{
	WHIT();
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	if (p->__scrollTimer) {
		elm_object_item_signal_emit(it, "elm,swipe,revert", "");
		return;
	}
	Evas_Object *obj = elm_object_item_widget_get(it);
	elm_object_scroll_freeze_push(obj);
	elm_object_scroll_hold_push(p->__genlist);
}

void PhLogGenlistHelper::__swipeInvokeCb(void *data, Elm_Object_Item *it, const char *em, const char *src)
{
	WHIT();
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	Evas_Object *obj = elm_object_item_widget_get(it);
	elm_object_scroll_freeze_pop(obj);
	elm_object_scroll_hold_pop(p->__genlist);

	if(strcmp(em, "elm,swipe,stop,right") == 0) {
		p->__swipeHandler(SWIPE_TO_RIGHT, NULL);
	} else if(strcmp(em, "elm,swipe,stop,left") == 0) {
		p->__swipeHandler(SWIPE_TO_LEFT, NULL);
	}
}

void PhLogGenlistHelper::__swipeCancleCb(void *data, Elm_Object_Item *it, const char *em, const char *src)
{
	WHIT();
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;
	Evas_Object *obj = elm_object_item_widget_get(it);
	elm_object_scroll_freeze_pop(obj);
	elm_object_scroll_hold_pop(p->__genlist);
}

void PhLogGenlistHelper::__genlistRealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == event_info, "parameter(event_info) is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	Elm_Object_Item *tmp_item = NULL;

	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	tmp_item = elm_genlist_item_next_get(item);// hide bottom line of last itme in loglist group
	if( tmp_item == NULL  || ( ELM_GENLIST_ITEM_GROUP == elm_genlist_item_type_get(tmp_item)) )
		elm_object_item_signal_emit(item, "elm,state,bottomline,hide", "");
	else
		elm_object_item_signal_emit(item, "elm,state,bottomline,show", "");

	if(p->__swipeHandler)
	{
		elm_object_item_signal_callback_del(item, "elm,swipe,start", "", __swipeStartCb);
		elm_object_item_signal_callback_del(item, "elm,swipe,stop,left", "", __swipeInvokeCb);
		elm_object_item_signal_callback_del(item, "elm,swipe,stop,right", "", __swipeInvokeCb);
		elm_object_item_signal_callback_del(item, "elm,swipe,stop", "", __swipeCancleCb);

		elm_object_item_signal_callback_add(item, "elm,swipe,start", "", __swipeStartCb, data);
		elm_object_item_signal_callback_add(item, "elm,swipe,stop,left", "", __swipeInvokeCb, data);
		elm_object_item_signal_callback_add(item, "elm,swipe,stop,right", "", __swipeInvokeCb, data);
		elm_object_item_signal_callback_add(item, "elm,swipe,stop", "", __swipeCancleCb, data);
	}
}

void PhLogGenlistHelper::__genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	WPRET_M(NULL == item, "parameter(item) is NULL");

	elm_object_item_signal_callback_del(item, "elm,swipe,start", "", __swipeStartCb);
	elm_object_item_signal_callback_del(item, "elm,swipe,stop,left", "", __swipeInvokeCb);
	elm_object_item_signal_callback_del(item, "elm,swipe,stop,right", "", __swipeInvokeCb);
	elm_object_item_signal_callback_del(item, "elm,swipe,stop", "", __swipeCancleCb);
}

void PhLogGenlistHelper::__languageChangedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

//	phct_common_detail_log_init_time_format();
//	phct_common_detail_log_generate_date_format(log_d->formatter);

	elm_genlist_realized_items_update(p->__genlist);

//	if(log_d->view_type == LOG_VIEW_TYPE_UG_NUMBER_CHECK)
//		ph_log_check_update_selection_info(log_d);

}

void PhLogGenlistHelper::__longPressedCb(void *data, Evas_Object *obj, void *event_info)
{

}

Eina_Bool PhLogGenlistHelper::__scrollTimerCb(void *data)
{
	WPRET_VM(!data, EINA_FALSE, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	if (p->__scrollTimer)
		ecore_timer_del(p->__scrollTimer);
	p->__scrollTimer = NULL;
	return EINA_FALSE;
}

void PhLogGenlistHelper::__scrollCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	if (p->__scrollTimer)
		ecore_timer_del(p->__scrollTimer);
	p->__scrollTimer = ecore_timer_add(PH_LOG_SCROLL_TIMER, __scrollTimerCb, data);
}

void PhLogGenlistHelper::__mouseUpCb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	WPRET_M(!data, "data is null");
	PhLogGenlistHelper* p = (PhLogGenlistHelper*)data;

	elm_object_scroll_hold_pop(p->__genlist);
	elm_object_scroll_freeze_pop(p->__genlist);
}

Evas_Object* PhLogGenlistHelper::__createGenlist(void)
{
	WHIT();
	WASSERT_EX(__parent != NULL, "__parent is null.");

	__genlist = elm_genlist_add(__parent);
	WASSERT_EX(__genlist != NULL, "genlist is not created.");

	evas_object_size_hint_weight_set(__genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_genlist_mode_set(__genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(__genlist, EINA_TRUE);

	evas_object_smart_callback_add(__genlist, "realized", __genlistRealizedCb, this);
	evas_object_smart_callback_add(__genlist, "unrealized", __genlistUnrealizedCb, this);
	evas_object_smart_callback_add(__genlist, "language,changed", __languageChangedCb, this);
	evas_object_smart_callback_add(__genlist, "longpressed", __longPressedCb, this);
	evas_object_smart_callback_add(__genlist, "scroll", __scrollCb, this);
	evas_object_event_callback_add(__genlist, EVAS_CALLBACK_MOUSE_UP, __mouseUpCb, this);

	return __genlist;
}

void PhLogGenlistHelper::appendFinish()
{
	WHIT();
	WPRET_M( NULL == __genlist, " __genlist is NULL");
	WPRET_M( 0 > __smartAppendIndex, "__smartAppendIndex is Invalid");

	Elm_Object_Item * item = NULL;
	while( NULL != (item = elm_genlist_nth_item_get(__genlist, __smartAppendIndex)) ){
		elm_object_item_del(item);
	}
	elm_genlist_item_bring_in(elm_genlist_first_item_get(__genlist),ELM_GENLIST_ITEM_SCROLLTO_TOP);

	__smartAppendIndex = 0;

	return;
}

Elm_Object_Item *PhLogGenlistHelper::appendItem(
						const Elm_Genlist_Item_Class *itc,
						const void *data,
						Elm_Object_Item *parent,
						Elm_Genlist_Item_Type type,
						Evas_Smart_Cb func,
						const void *funcData)
{
	WASSERT_EX(__genlist != NULL, "genlist is not created.");

	ItemData* itemData = new ItemData;
	itemData->p = funcData;
	itemData->data = data;
	itemData->contents = NULL;
	itemData->thumbnailUpdateTimer = NULL;

	Elm_Genlist_Item_Class *curItemClass;
	Elm_Object_Item *curItem = elm_genlist_nth_item_get(__genlist, __smartAppendIndex);
//	WDEBUG("HERE : curItem : %p",curItem);
	if(NULL == curItem)
	{
//		WDEBUG("insert");
		itemData->it = elm_genlist_item_append(__genlist, itc, itemData, parent, type, func, funcData);
	}
	else
	{
		curItemClass = (Elm_Genlist_Item_Class  *)elm_genlist_item_item_class_get((const Elm_Object_Item *)curItem);
		if(itc == curItemClass)
		{
//			WDEBUG("update");
			ItemData* curItemData = (ItemData*)elm_object_item_data_get(curItem);
			if (curItemData) {
				itemData->contents = curItemData->contents;
				delete curItemData;
			}
			elm_object_item_data_set(curItem, itemData);
			elm_genlist_item_update(curItem);
			itemData->it = curItem;
		}
		else
		{
//			WDEBUG("replace");
			itemData->it = elm_genlist_item_insert_after(__genlist, itc, itemData, parent, curItem, type, func, funcData);
			elm_object_item_del(curItem);
		}
	}

	if (!itemData->it) {
		WERROR("elm_genlist_nth_item_get() is failed");
		delete itemData;
		return NULL;
	}

	__smartAppendIndex++;

//	WDEBUG("HERE : itemData: %p, genlist count : %d, append count : %d", itemData, elm_genlist_items_count (__genlist), __smartAppendIndex);
	return itemData->it;
}

Elm_Object_Item *PhLogGenlistHelper::appendCheckItem(
						const Elm_Genlist_Item_Class *itc,
						const void *data,
						Elm_Object_Item *parent,
						Elm_Genlist_Item_Type type,
						Evas_Smart_Cb func,
						const void *funcData,
						Eina_Bool isChecked)
{
	WASSERT_EX(__genlist != NULL, "genlist is not created.");

	CheckedItemData* itemData = new CheckedItemData;
	itemData->p = funcData;
	itemData->data = data;
	itemData->isChecked = isChecked;
	itemData->checkButton = NULL;

	Elm_Genlist_Item_Class *curItemClass;
	Elm_Object_Item *curItem = elm_genlist_nth_item_get(__genlist, __smartAppendIndex);
//	WDEBUG("HERE : curItem : %p",curItem);
	if(NULL == curItem)
		itemData->it = elm_genlist_item_append(__genlist, itc, itemData, parent, type, func, funcData);
	else
	{
		curItemClass = (Elm_Genlist_Item_Class  *)elm_genlist_item_item_class_get((const Elm_Object_Item *)curItem);
		if(itc == curItemClass)
		{
			CheckedItemData* curItemData = (CheckedItemData*)elm_object_item_data_get(curItem);
			WDEBUG("update from : %p, to : %p", curItemData, itemData);
			if (curItemData) {
				delete curItemData;
			}
			elm_object_item_data_set(curItem, (void *)itemData);
			elm_genlist_item_item_class_update(curItem,itc);
			elm_genlist_item_update(curItem);
			itemData->it = curItem;
		}
		else
		{
			WDEBUG("replace");
			itemData->it = elm_genlist_item_insert_after(__genlist, itc, itemData, parent, curItem, type, func, funcData);
			elm_object_item_del(curItem);
		}
	}

	if (!itemData->it) {
		WERROR("elm_genlist_nth_item_get() is failed");
		delete itemData;
		return NULL;
	}

	__smartAppendIndex++;
//	WDEBUG("HERE : itemData: %p, genlist count : %d, append count : %d", itemData, elm_genlist_items_count (__genlist), __smartAppendIndex);
	return itemData->it;
}
