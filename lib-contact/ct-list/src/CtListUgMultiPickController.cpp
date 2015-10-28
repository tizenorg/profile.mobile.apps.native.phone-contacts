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

#include <vector>
#include <thread>
#include <Elementary.h>
#include <app.h>
#include <contacts.h>
#include <app_preference.h>

#include "WView.h"
#include "WNaviframe.h"
#include "WPopup.h"

#include "ContactsDebug.h"
#include "CtType.h"
#include "CtListUgMultiPickController.h"
#include "CtCommon.h"
#include "CtContactsToVcard.h"
#include "CtListView.h"
#include "CtListModelDb.h"
#include "CtProgressBarPopup.h"

#define __ITEM_HEIGHT 96
#define CHECKED_INFO_PAUSE_INTERVAL	200*1000


CtListUgMultiPickController::CtListUgMultiPickController(CtListResultType resultType, CtListView *listView, CtListModelDb* listData)
{
	WHIT();
	__listView = listView;
	__linkedCount = 1;

	__listModel = listData;
	__ctrlType = LIST_CTRL_DEFAULT;
	__selectedPersonData = NULL;

	switch (resultType) {
		case LIST_RESULT_ID:
			__ctrlType = LIST_CTRL_REQUEST_CHECK_FOR_PERSON;
			break;
		case LIST_RESULT_NUMBER:
		case LIST_RESULT_NUMBER_FOR_CALL:
		case LIST_RESULT_NUMBER_FOR_MESSAGE:
			__ctrlType = LIST_CTRL_REQUEST_CHECK_FOR_NUMBER;
			break;
		case LIST_RESULT_EMAIL:
			__ctrlType = LIST_CTRL_REQUEST_CHECK_FOR_EMAIL;
			break;
		case LIST_RESULT_VCARD:
			__ctrlType = LIST_CTRL_REQUEST_CHECK_FOR_VCARD;
			break;
		case LIST_RESULT_NUMBER_OR_EMAIL:
			__ctrlType = LIST_CTRL_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL;
			break;
	}
}

CtListUgMultiPickController::~CtListUgMultiPickController()
{
	WHIT();

	if(__selectedPersonData)
		delete __selectedPersonData;

}

void CtListUgMultiPickController::__onSelectDataPopupItem(void *data, Evas_Object *obj, void *event_info)
{
	//WHIT();
	CtListPopupItemData *popupItemData = (CtListPopupItemData*)data;
	int personId = popupItemData->controller->__selectedPersonData->getPersonId();
	Elm_Object_Item *seletedItem = popupItemData->controller->__listView->getGenlistItemByPersonId(personId);

	popupItemData->controller->__listView->destroyPopup();
	CtCheckState checkState = CHECK_OFF;
	if(popupItemData->controller->__getCheckState(seletedItem) == CHECK_OFF)
		checkState = CHECK_ON;
	else
		checkState = CHECK_OFF;

	popupItemData->controller->__changeCheckState(seletedItem, checkState, true);

	if(popupItemData->dataType == CT_LIST_DATA_NUMBER) {
		popupItemData->controller->__setCheckedInfo(seletedItem, popupItemData->record, CT_LIST_DATA_NUMBER);

	}
	else if(popupItemData->dataType == CT_LIST_DATA_EMAIL) {
		popupItemData->controller->__setCheckedInfo(seletedItem, popupItemData->record, CT_LIST_DATA_EMAIL);
	}

	elm_genlist_realized_items_update(popupItemData->controller->__listView->__genlist);
}

Evas_Object* CtListUgMultiPickController::__createDataListPopup(Elm_Object_Item *item, int dataType)
{
	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_VM(itemData == NULL, NULL, "itemData is null");
	Eina_List *numberList = NULL;
	Eina_List *emailList = NULL;
	if(dataType == CT_LIST_DATA_NUMBER)
		numberList = __selectedPersonData->getNumberList();
	else if(dataType == CT_LIST_DATA_EMAIL)
		emailList = itemData->personData->getEmailList();
	else if(dataType == CT_LIST_DATA_ALL) {
		numberList = __selectedPersonData->getNumberList();
		emailList = __selectedPersonData->getEmailList();
	}
	else {
		WERROR("wrong dataType %d", dataType);
		return NULL;
	}

	Eina_List *checked_numberList = NULL;
	Eina_List *checked_emailList = NULL;
	checked_numberList = ctCommonGetChekedDuplicatedNumberList(numberList);
	checked_emailList = ctCommonGetChekedDuplicatedEmailList(emailList);

	int count = eina_list_count(checked_numberList) + eina_list_count(checked_emailList);
	WDEBUG("data count %d", count);
	if(count == 1) {
		ctCommonDestroyRecordList(checked_numberList);
		ctCommonDestroyRecordList(checked_emailList);
		return NULL;
	}

	Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
	if (!itc) {
		WERROR("elm_genlist_item_class_new() failed");
		ctCommonDestroyRecordList(checked_numberList);
		ctCommonDestroyRecordList(checked_emailList);
		return NULL;
	}

	itc->item_style = "type1";
	itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CtListUgMultiPickController::CtListPopupItemData *popupItemData = (CtListUgMultiPickController::CtListPopupItemData*)data;
		if(!strcmp(part, "elm.text")){
			char *value = NULL;
			if(popupItemData->dataType == CT_LIST_DATA_NUMBER)
				contacts_record_get_str_p(popupItemData->record, _contacts_person_number.number, &value);
			else if(popupItemData->dataType == CT_LIST_DATA_EMAIL)
				contacts_record_get_str_p(popupItemData->record, _contacts_person_email.email, &value);
			else
				WPRET_VM(1, NULL, "wrong dataType %d", popupItemData->dataType);

			if (value)
				return SAFE_STRDUP(value);
			else
				return NULL;
		}
		return NULL;
	};
	itc->func.del = [](void* data, Evas_Object* obj)->void
	{
		CtListPopupItemData *popupItemData = (CtListPopupItemData*)data;
		contacts_record_destroy(popupItemData->record, true);
		delete popupItemData;
	};

	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle(itemData->personData->getDisplayName());
	popup->setContent([this,itc,checked_numberList,checked_emailList,item](Evas_Object* parent)->Evas_Object*
	{
		Evas_Object* genlist = elm_genlist_add(parent);
		elm_object_style_set( genlist, "popup" );
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated
		elm_object_content_set(parent, genlist);

		void *data = NULL;
		int count = 0;
		int err = CONTACTS_ERROR_NONE;
		Eina_List *cursor;
		EINA_LIST_FOREACH(checked_numberList, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;

			CtListPopupItemData *popupItemData = new CtListPopupItemData;
			err = contacts_record_clone(record, &popupItemData->record);
			if(err != CONTACTS_ERROR_NONE)
			{
				WERROR("contacts_record_clone() Failed(%d)", err);
				if(popupItemData->record)
					contacts_record_destroy(popupItemData->record, true);
				delete popupItemData;
				break;
			}
			popupItemData->controller = this;
			popupItemData->dataType = CT_LIST_DATA_NUMBER;

			popupItemData->popupItem = elm_genlist_item_append(genlist, itc, (void*)popupItemData, NULL, ELM_GENLIST_ITEM_NONE, CtListUgMultiPickController::__onSelectDataPopupItem, popupItemData);
			count++;
		}
		ctCommonDestroyRecordList(checked_numberList);

		EINA_LIST_FOREACH(checked_emailList, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;

			CtListPopupItemData *popupItemData = new CtListPopupItemData;
			err = contacts_record_clone(record, &popupItemData->record);
			if(err != CONTACTS_ERROR_NONE)
			{
				WERROR("contacts_record_clone() Failed(%d)", err);
				delete popupItemData;
				break;
			}
			popupItemData->controller = this;
			popupItemData->dataType = CT_LIST_DATA_EMAIL;

			popupItemData->popupItem = elm_genlist_item_append(genlist, itc, (void*)popupItemData, NULL, ELM_GENLIST_ITEM_NONE, CtListUgMultiPickController::__onSelectDataPopupItem, popupItemData);
			count++;
		}
		ctCommonDestroyRecordList(checked_emailList);

		evas_object_show(genlist);

		return genlist;

	});

	__listView->attachPopup(popup);

	elm_genlist_item_class_free(itc);

	return popup->getEvasObj();
}

void CtListUgMultiPickController::__updateTitleAndDone()
{
	Elm_Object_Item *naviItem = elm_naviframe_top_item_get(__listView->getViewNaviframe());
	Evas_Object *btn = elm_object_item_part_content_get(naviItem, "title_right_btn");

	int checkedCount = __listModel->getCheckedDataCount();

	if(btn) {
		if(checkedCount > 0)
			elm_object_disabled_set(btn, EINA_FALSE);
		else
			elm_object_disabled_set(btn, EINA_TRUE);
	}

	WDEBUG("%d, %d", checkedCount, __listModel->getPersonListCount(LIST_QUERY_DEFAULT));
	//To avoid watchdog
	if(checkedCount>500) {
		if( checkedCount%500  == 1)
			usleep(CHECKED_INFO_PAUSE_INTERVAL);
	}

	char buf[CT_TEXT_SHORT_LEN] = {0,};
	if(__listModel->getLimitCount() == 0)
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), checkedCount);
	else
		snprintf(buf, sizeof(buf), "%d/%d", checkedCount, __listModel->getLimitCount());

	elm_object_item_part_text_set(naviItem, "elm.text.title", buf);
}

bool CtListUgMultiPickController::__changeCheckState(Elm_Object_Item *item, CtCheckState checkState, bool updateTitleAndDone)
{
	WHIT();

	Elm_Genlist_Item_Type item_type = elm_genlist_item_type_get(item);
	if(item_type == ELM_GENLIST_ITEM_GROUP) {
		return false;
	}

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_VM(itemData == NULL, false, "itemData is null");

	Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
	Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
	elm_check_state_set(check, (Eina_Bool)checkState);

	itemData->personData->setAsChecked((bool)checkState);
	if(itemData->personData->isChecked() == true) {
		__listModel->pushCheckedData(itemData->personData);
		__linkedCount += itemData->personData->getLinkedCount();
	}
	else {
		__listModel->removeCheckedData(itemData->personData);
		__linkedCount -= itemData->personData->getLinkedCount();
	}


/*
	Elm_Object_Item *naviItem = elm_naviframe_top_item_get(__listView->getViewNaviframe());
	Evas_Object *btn = elm_object_item_part_content_get(naviItem, "title_right_btn");

	if(btn) {
		if(checkedCount > 0)
			elm_object_disabled_set(btn, EINA_FALSE);
		else
			elm_object_disabled_set(btn, EINA_TRUE);
	}

	WDEBUG("%d, %d", checkedCount, __listModel->getPersonListCount(LIST_QUERY_DEFAULT));
	//To avoid watchdog
	if(checkedCount>500) {
		if( checkedCount%500  == 1)
			usleep(CHECKED_INFO_PAUSE_INTERVAL);
	}
*/

	if(__listView->__selectAllObj) {
		if (__listModel->getSearchString()) {
			if (__listModel->checkIsSelectAllOnSearching()) {
				__listView->__selectAllState = CHECK_ON;
			}
			else {
				__listView->__selectAllState = CHECK_OFF;
			}
		}
		else {
			int checkedCount = __listModel->getCheckedDataCount();
			int maxCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
			WDEBUG("%d, %d", checkedCount, maxCount);

			if(checkedCount == maxCount) {
				WDEBUG("check select all");
				elm_check_state_set(__listView->__selectAllObj, EINA_TRUE);
				__listView->__selectAllState = CHECK_ON;
			}
			else {
				elm_check_state_set(__listView->__selectAllObj, EINA_FALSE);
				__listView->__selectAllState = CHECK_OFF;
			}
		}
	}
/*
	char buf[CT_TEXT_SHORT_LEN] = {0,};
	if(__listModel->getLimitCount() == 0)
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), checkedCount);
	else
		snprintf(buf, sizeof(buf), "%d/%d", checkedCount, __listModel->getLimitCount());

	elm_object_item_part_text_set(naviItem, "elm.text.title", buf);
*/

	if (updateTitleAndDone) {
		__updateTitleAndDone();
	}


	return itemData->personData->isChecked();
}

CtCheckState CtListUgMultiPickController::__getCheckState(Elm_Object_Item *item)
{
	WHIT();

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_VM(itemData == NULL, CHECK_OFF, "itemData is null");

	return (CtCheckState)itemData->personData->isChecked();
}

void CtListUgMultiPickController::__setCheckedInfo(Elm_Object_Item *item, contacts_record_h record, CtListItemDataType dataType)
{
	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_M(itemData == NULL, "itemData is null");
	int dataId = 0;
	char *dataString = NULL;
	int err = CONTACTS_ERROR_NONE;

	switch(dataType) {
	case CT_LIST_DATA_PERSON:
		itemData->personData->setDataId(itemData->personData->getPersonId());
		itemData->personData->setDataString(itemData->personData->getDisplayName());
		itemData->personData->setDataType((int)dataType);
		break;
	case CT_LIST_DATA_NUMBER:
		err = contacts_record_get_int(record, _contacts_person_number.number_id, &dataId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		err = contacts_record_get_str_p(record, _contacts_person_number.number, &dataString);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		itemData->personData->setDataId(dataId);
		itemData->personData->setDataString(dataString);
		itemData->personData->setDataType((int)dataType);
		break;
	case CT_LIST_DATA_EMAIL:
		err = contacts_record_get_int(record, _contacts_person_email.email_id, &dataId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		err = contacts_record_get_str_p(record, _contacts_person_email.email, &dataString);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		itemData->personData->setDataId(dataId);
		itemData->personData->setDataString(dataString);
		itemData->personData->setDataType((int)dataType);
		break;
	case CT_LIST_DATA_VCARD:
		break;
	default:
		WERROR("wring data type %d", dataType);
		break;
	}

}

void CtListUgMultiPickController::onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	//WHIT();
	Elm_Object_Item *item = selectedItem;
	elm_genlist_item_selected_set(item, EINA_FALSE);

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);

	if(__selectedPersonData)
		delete __selectedPersonData;
	__selectedPersonData = itemData->personData->makeClone();

	WDEBUG("__ctrlType %d", __ctrlType);

	if(checkState == __getCheckState(item))
		return;

	if(checkState == CHECK_ON) {
		int checkedNum = 0;

		checkedNum = __listModel->getCheckedDataCount()+1;

		if(__checkLimitCount(checkedNum, __listModel->getLimitCount()) == true)
				return;
	}

	switch(__ctrlType){
	case LIST_CTRL_REQUEST_CHECK_FOR_VCARD:
	{
		__changeCheckState(item, checkState, true);
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_LINK_CONTACTS:
	{
		if(checkState == CHECK_ON) {
			const int currentContactLinkedCount = 0;
			const int checkedContactLinkedCount = itemData->personData->getLinkedCount();
			if(__checkMaxLinkCount(currentContactLinkedCount, checkedContactLinkedCount) == true)
				return;
		}

		if(__changeCheckState(item, checkState, true) == true) {
			__setCheckedInfo(item, NULL, CT_LIST_DATA_PERSON);
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_PERSON:
	{
		if(__changeCheckState(item, checkState, true) == true) {
			__setCheckedInfo(item, NULL, CT_LIST_DATA_PERSON);
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, true);
			break;
		}

		if(__createDataListPopup(item, CT_LIST_DATA_NUMBER) == NULL) {
			if(__changeCheckState(item, checkState, true) == true) {
				Eina_List *cursor = NULL;
				void *data = NULL;
				Eina_List *dataList = __selectedPersonData->getNumberList();
				EINA_LIST_FOREACH(dataList, cursor, data) {
					contacts_record_h record = (contacts_record_h)data;
					__setCheckedInfo(item, record, CT_LIST_DATA_NUMBER);
					break;
				}
			}
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_EMAIL:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, true);
			break;
		} else if(checkState == CHECK_ON){
			Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
			Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
			elm_check_state_set(check, EINA_FALSE);
		}

		if(__createDataListPopup(item, CT_LIST_DATA_EMAIL) == NULL) {
			if(__changeCheckState(item, checkState, true) == true) {
				Eina_List *cursor = NULL;
				void *data = NULL;
				Eina_List *dataList = __selectedPersonData->getEmailList();
				EINA_LIST_FOREACH(dataList, cursor, data) {
					contacts_record_h record = (contacts_record_h)data;
					__setCheckedInfo(item, record, CT_LIST_DATA_EMAIL);
					break;
				}
			}
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, true);
			break;
		} else if(checkState == CHECK_ON){
			Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
			Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
			elm_check_state_set(check, EINA_FALSE);
		}

		if(__createDataListPopup(item, CT_LIST_DATA_ALL) == NULL) {
			if(__changeCheckState(item, checkState, true) == true) {

				Eina_List *cursor = NULL;
				void *data = NULL;
				Eina_List *dataList = __selectedPersonData->getNumberList();
				if(dataList == NULL) {
					dataList = __selectedPersonData->getEmailList();
					EINA_LIST_FOREACH(dataList, cursor, data) {
						contacts_record_h record = (contacts_record_h)data;
						__setCheckedInfo(item, record, CT_LIST_DATA_EMAIL);
						break;
					}
				}
				else {
					EINA_LIST_FOREACH(dataList, cursor, data) {
						contacts_record_h record = (contacts_record_h)data;
						__setCheckedInfo(item, record, CT_LIST_DATA_NUMBER);
						break;
					}
				}
			}
		}
	}
		break;
	default:
		WWARNING("Not handled type=%d", __ctrlType);
		break;
	}

	elm_genlist_realized_items_update(view->__genlist);
}

void CtListUgMultiPickController::onSelectDone()
{
	WHIT();

	switch(__ctrlType){
	case LIST_CTRL_REQUEST_CHECK_FOR_VCARD:
	case LIST_CTRL_REQUEST_CHECK_FOR_PERSON:
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER:
	case LIST_CTRL_REQUEST_CHECK_FOR_EMAIL:
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL:
	case LIST_CTRL_REQUEST_CHECK_FOR_LINK_CONTACTS:
		__doReplyCheckedInfo();
		break;
	default:
		break;
	}
}


void CtListUgMultiPickController::onSelectCancel()
{
	WHIT();
	__listView->sendMultiResult(NULL, 0);
	__listView->popOut();
}

void CtListUgMultiPickController::onSelectBack()
{
	WHIT();
	__listView->sendMultiResult(NULL, 0);
}

void CtListUgMultiPickController::onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();

	Elm_Object_Item *item = selectedItem;
	if (item)
		elm_genlist_item_selected_set(item, EINA_FALSE);

	Evas_Object *genlist = view->__genlist;
	int checkedCount = __listModel->getCheckedDataCount();
	int limitCount = __listModel->getLimitCount();
	int maxCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);

	if (limitCount > 0 && limitCount < maxCount && checkState == CHECK_ON) {
		__checkLimitCount(maxCount, limitCount);
		return;
	}

	WDEBUG("select all %d", checkState);
	if (!item) {
		item = elm_genlist_first_item_get(genlist);
		if(view->__searchbarItem) {
			item = elm_genlist_item_next_get(view->__searchbarItem);
		}
	}

	while(item) {
		if(item == view->__selectAllItem) {
			item = elm_genlist_item_next_get(item);
			continue;
		}
		Elm_Genlist_Item_Type item_type = elm_genlist_item_type_get(item);
		if(item_type == ELM_GENLIST_ITEM_GROUP) {
			item = elm_genlist_item_next_get(item);
			continue;
		}

		if(__checkItem(item, view, checkState) == false) {
			WDEBUG("already checked");
			item = elm_genlist_item_next_get(item);
			continue;
		}
		checkedCount++;

		item = elm_genlist_item_next_get(item);
	}

	__updateTitleAndDone();

	WHIT();
	elm_genlist_realized_items_update(genlist);
}

bool CtListUgMultiPickController::__checkItem(Elm_Object_Item *item, CtListView *view, CtCheckState checkState)
{
	//WHIT();
	elm_genlist_item_selected_set(item, EINA_FALSE);

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_VM(itemData == NULL, false, "itemData is null");
	WDEBUG("__ctrlType %d", __ctrlType);

	if(checkState == __getCheckState(item))
		return false;

	if(checkState == CHECK_ON) {
		int checkedNum = 0;

		checkedNum = __listModel->getCheckedDataCount()+1;

		if(__checkLimitCount(checkedNum, __listModel->getLimitCount()) == true)
				return false;
	}

	switch(__ctrlType){
	case LIST_CTRL_REQUEST_CHECK_FOR_VCARD:
	{
		__changeCheckState(item, checkState, false);
	}
	break;
	case LIST_CTRL_REQUEST_CHECK_FOR_LINK_CONTACTS:
	case LIST_CTRL_REQUEST_CHECK_FOR_PERSON:
	{
		if(__changeCheckState(item, checkState, false) == true) {
			__setCheckedInfo(item, NULL, CT_LIST_DATA_PERSON);
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, false);
			break;
		}

		if(__changeCheckState(item, checkState, false) == true) {
			Eina_List *cursor = NULL;
			void *data = NULL;
			Eina_List *dataList = itemData->personData->getNumberList();
			EINA_LIST_FOREACH(dataList, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				__setCheckedInfo(item, record, CT_LIST_DATA_NUMBER);
				break;
			}
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_EMAIL:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, false);
			break;
		} else if(checkState == CHECK_ON){
			Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
			Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
			elm_check_state_set(check, EINA_FALSE);
		}

		if(__changeCheckState(item, checkState, false) == true) {
			Eina_List *cursor = NULL;
			void *data = NULL;
			Eina_List *dataList = itemData->personData->getEmailList();
			EINA_LIST_FOREACH(dataList, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				__setCheckedInfo(item, record, CT_LIST_DATA_EMAIL);
				break;
			}
		}
	}
		break;
	case LIST_CTRL_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL:
	{
		if(checkState == CHECK_OFF) {
			__changeCheckState(item, checkState, false);
			break;
		} else if(checkState == CHECK_ON){
			Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
			Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
			elm_check_state_set(check, EINA_FALSE);
		}

		if(__changeCheckState(item, checkState, false) == true) {
			Eina_List *cursor = NULL;
			void *data = NULL;
			Eina_List *dataList = itemData->personData->getNumberList();
			if(dataList == NULL) {
				dataList = itemData->personData->getEmailList();
				EINA_LIST_FOREACH(dataList, cursor, data) {
					contacts_record_h record = (contacts_record_h)data;
					__setCheckedInfo(item, record, CT_LIST_DATA_EMAIL);
					break;
				}
			}
			else {
				EINA_LIST_FOREACH(dataList, cursor, data) {
					contacts_record_h record = (contacts_record_h)data;
					__setCheckedInfo(item, record, CT_LIST_DATA_NUMBER);
					break;
				}
			}
		}
	}
		break;
	default:
		WWARNING("Not handled type=%d", __ctrlType);
		break;
	}

	elm_genlist_realized_items_update(view->__genlist);

	return true;
}


bool CtListUgMultiPickController::__checkLimitCount(int checkCount, int limitCount)
{
	WHIT();

	if(limitCount == 0)
		return false;

	if(limitCount < checkCount) {
		WPopup* popup = new WPopup();
		popup->setTextTranslatable(PACKAGE);
		popup->setTitle("IDS_WMGR_HEADER_UNABLE_TO_SELECT_MORE_ITEMS_ABB");
		char buf[CT_TEXT_MAX_LEN] = {0,};
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_POP_THE_MAXIMUM_NUMBER_OF_ITEMS_THAT_CAN_BE_SELECTED_HPD_HAS_BEEN_EXCEEDED"), __listModel->getLimitCount());
		popup->setContent(buf);
		popup->addButton("IDS_PB_BUTTON_OK_ABB2", NULL );

		WDEBUG("limitcount %d, checkcount %d", limitCount, checkCount);
		__listView->attachPopup(popup);
		return true;
	}

	return false;
}

bool CtListUgMultiPickController::__checkMaxLinkCount(const int currentContactLinkedCount, const int checkedContactLinkedCount)
{
	WHIT();
	int sumOfCheckedContactLinkCount = 0;

	std::unique_ptr<std::vector<std::shared_ptr<CtListDataPerson>>> checkedDatas(__listModel->getCheckedDataList());
	int checkedDataCount = checkedDatas->size();
	if (checkedDataCount) {
		for(auto iter = checkedDatas->begin();iter != checkedDatas->end();++iter) {
			sumOfCheckedContactLinkCount += (*iter)->getLinkedCount();
		}		
	}

	WDEBUG("current %d checked %d total%d", currentContactLinkedCount, checkedContactLinkedCount, sumOfCheckedContactLinkCount);
	if(currentContactLinkedCount+checkedContactLinkedCount+sumOfCheckedContactLinkCount <= 10)
		return false;

	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_WMGR_HEADER_UNABLE_TO_SELECT_MORE_ITEMS_ABB");
	char buf[CT_TEXT_MAX_LEN] = {0,};
	snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_POP_UNABLE_TO_ADD_CONTACTS_MAXIMUM_NUMBER_OF_LINKABLE_CONTACTS_HPD_REACHED"), 10);
	popup->setContent(buf);
	popup->addButton("IDS_PB_BUTTON_OK_ABB2", NULL );

	__listView->attachPopup(popup);
	return true;
}

void CtListUgMultiPickController::__doReplyCheckedInfo()
{
	WENTER();
	if(__ctrlType == LIST_CTRL_REQUEST_CHECK_FOR_VCARD) {
		__vcardReplyCheckedInfo();
		return;
	}

	std::unique_ptr<std::vector<std::shared_ptr<CtListDataPerson>>> checkedData(__listModel->getCheckedDataList());
	int checkedDataCount = checkedData->size();
	if (checkedDataCount == 0) {
		WERROR("no checked item");
		return;
	}

	char **resultArray = new char*[checkedDataCount];
	char **result = resultArray;

	for (auto &&data : *checkedData) {
		int dataId = 0;
		if (data->getDataType() == CT_LIST_DATA_PERSON) {
			dataId = data->getPersonId();
		} else {
			dataId = data->getDataId();
		}

		char buf[CT_TEXT_SHORT_LEN] = { 0, };
		snprintf(buf, sizeof(buf), "%d", dataId);
		*result++ = strdup(buf);
	}

	__listView->sendMultiResult((const char**) resultArray, checkedDataCount);
	for (int i = 0; i < checkedDataCount; ++i) {
		free(resultArray[i]);
	}

	delete [] resultArray;
	__listView->popOut();
}

void CtListUgMultiPickController::__vcardReplyCheckedInfo()
{
	std::unique_ptr<std::vector<std::shared_ptr<CtListDataPerson>>> checkedData(__listModel->getCheckedDataList());
	WPRET_M(!checkedData, "checkedData is null");
	int checkedDataCount = checkedData->size();
	WPRET_M(!checkedDataCount, "checkedDataCount is null");

	std::vector<int> personIdList;
	CtContactsToVcard *vcard = NULL;

	personIdList.reserve(checkedDataCount);
	for (auto &item : *checkedData) {
		personIdList.push_back(item->getPersonId());
	}

	vcard = new CtContactsToVcard("Pick contacts", TargetStorage::INTERNAL_OTHER, std::move(personIdList));
	vcard->setOnFinish(std::bind(&CtListUgMultiPickController::__sendVcardResult, this, std::placeholders::_1));

	__listView->attachPopup(vcard->getProgressPopup());

	vcard->run();
}

void CtListUgMultiPickController::__sendVcardResult(CtProgressController& controller)
{
	CtContactsToVcard &vcard = static_cast<CtContactsToVcard&>(controller);
	const char *vcardPath = vcard.getVcardPath().c_str();

	__listView->sendMultiResult(&vcardPath, 1);
	__listView->popOut();
}
