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

#include <fcntl.h>
#include <vector>
#include <thread>
#include <Elementary.h>
#include <app.h>
#include <contacts.h>
#include <glib.h>
#include <app_preference.h>

#include "WView.h"
#include "WNaviframe.h"
#include "WPopup.h"

#include "ContactsDebug.h"
#include "CtType.h"
#include "CtListUgSinglePickController.h"
#include "CtListDataPerson.h"
#include "CtCommon.h"
#include "CtListView.h"
#include "CtListModelDb.h"
#include "CtProgressBarPopup.h"
#include "CtNumberPopup.h"

#define __ITEM_HEIGHT 96


CtListUgSinglePickController::CtListUgSinglePickController(CtListResultType resultType, CtListView *listView, CtListModelDb* listData)
{
	WHIT();
	__listView = listView;
	__idler = NULL;
	__selectedPersonData = NULL;
	__listModel = listData;
	__resultType = resultType;
}

CtListUgSinglePickController::~CtListUgSinglePickController()
{
	WHIT();

	if(__idler)
		ecore_idler_del(__idler);
	__idler = NULL;

	if(__selectedPersonData)
		delete __selectedPersonData;
}

Eina_Bool CtListUgSinglePickController::__doReplyIdle(void* data)
{
	CtListUgSinglePickController *controller = (CtListUgSinglePickController*)data;
	CtListDataPerson *personData = controller->__selectedPersonData;

	switch(controller->__resultType) {
	case LIST_RESULT_ID:
	case LIST_RESULT_VCARD:
		controller->__doReplySelectedInfo(personData);
		break;
	case LIST_RESULT_NUMBER:
		if(controller->__createDataListPopup(personData, CT_LIST_DATA_NUMBER) == NULL)
			controller->__doReplySelectedInfo(personData);
		break;
	case LIST_RESULT_EMAIL:
		if(controller->__createDataListPopup(personData, CT_LIST_DATA_EMAIL) == NULL)
			controller->__doReplySelectedInfo(personData);
		break;
	case LIST_RESULT_NUMBER_OR_EMAIL:
		if(controller->__createDataListPopup(personData, CT_LIST_DATA_ALL) == NULL)
			controller->__doReplySelectedInfo(personData);
		break;
	default:
		WERROR("wrong control type");
		break;
	}

	controller->__idler = NULL;
	return EINA_FALSE;
}

void CtListUgSinglePickController::onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();

	elm_genlist_item_selected_set(selectedItem, EINA_FALSE);
	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(selectedItem);
	if(__selectedPersonData)
		delete __selectedPersonData;
	__selectedPersonData = itemData->personData->makeClone();

	__idler = ecore_idler_add(__doReplyIdle, this);
}

void CtListUgSinglePickController::onSelectCreate()
{
	__listView->sendSingleResult("0", LIST_RESULT_ID);
	__listView->popOut();
}

void CtListUgSinglePickController::onSelectDone()
{
	WHIT();
}


void CtListUgSinglePickController::onSelectCancel()
{
	WHIT();
}

void CtListUgSinglePickController::onSelectBack()
{
	WHIT();

	__listView->sendSingleResult(nullptr, LIST_RESULT_ID);
}

void CtListUgSinglePickController::onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();
}

void CtListUgSinglePickController::__doReplySelectedInfo(CtListDataPerson* personData)
{
	WENTER();

	switch(__resultType) {
		case LIST_RESULT_ID:
		{
			char buf[CT_TEXT_SHORT_LEN] = {0, };
			snprintf(buf, sizeof(buf), "%d", personData->getPersonId());
			__listView->sendSingleResult(buf, __resultType);
		}
			break;
		case LIST_RESULT_NUMBER_OR_EMAIL:
		case LIST_RESULT_NUMBER:
		case LIST_RESULT_EMAIL:
		{
			char buf[CT_TEXT_SHORT_LEN] = {0, };
			snprintf(buf, sizeof(buf), "%d", personData->getDataId());
			__listView->sendSingleResult(buf, (CtListResultType) personData->getDataType());
		}
			break;
		case LIST_RESULT_VCARD:
		{
			std::string vcardPath = createVcardFile(personData->getPersonRecord());
			__listView->sendSingleResult(vcardPath.c_str(), __resultType);
		}
			break;
		default:
			break;
	}

	__listView->popOut();
}

Evas_Object* CtListUgSinglePickController::__createDataListPopup(CtListDataPerson *personData, int dataType)
{
	WDEBUG("dataType %d", dataType);

	Eina_List *numberList = NULL;
	Eina_List *emailList = NULL;
	Eina_List *checked_numberList = NULL;
	Eina_List *checked_emailList = NULL;
	int dataId = 0;
	int err = CONTACTS_ERROR_NONE;
	if(dataType & CT_LIST_DATA_NUMBER) {
		numberList = personData->getNumberList();
		checked_numberList = ctCommonGetChekedDuplicatedNumberList(numberList);
	}

	if(dataType & CT_LIST_DATA_EMAIL) {
		emailList = personData->getEmailList();
		checked_emailList = ctCommonGetChekedDuplicatedEmailList(emailList);
	}

	if(eina_list_count(checked_numberList) == 1 && eina_list_count(checked_emailList) == 0) {
		char *dataString = NULL;
		err = contacts_record_get_int((contacts_record_h)eina_list_nth(checked_numberList, 0), _contacts_person_number.number_id, &dataId);
		WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_int() fail(%d)",err);
		err = contacts_record_get_str_p((contacts_record_h)eina_list_nth(checked_numberList, 0), _contacts_person_number.number, &dataString);
		WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_str_p() fail(%d)",err);

		personData->setDataId(dataId);
		personData->setDataString(dataString);
		personData->setDataType(LIST_RESULT_NUMBER);
		ctCommonDestroyRecordList(checked_numberList);

		if (__resultType == LIST_RESULT_NUMBER_OR_EMAIL) {
			return __createNumberPopup(personData);
		}

		return NULL;
	} else if(eina_list_count(checked_numberList) == 0 && eina_list_count(checked_emailList) == 1) {
		err = contacts_record_get_int((contacts_record_h)eina_list_nth(checked_emailList, 0), _contacts_person_email.email_id, &dataId);
		WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_int() fail(%d)",err);

		personData->setDataId(dataId);
		personData->setDataType(LIST_RESULT_EMAIL);
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
		CtListUgSinglePickController::CtListPopupItemData *popupItemData = (CtListUgSinglePickController::CtListPopupItemData*)data;
		if(!strcmp(part, "elm.text")){
			char *value = NULL;
			int err = CONTACTS_ERROR_NONE;
			if(popupItemData->dataType == CT_LIST_DATA_NUMBER){
				err = contacts_record_get_str_p(popupItemData->record, _contacts_person_number.number, &value);
				WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_str_p() fail(%d)",err);
			}
			else if(popupItemData->dataType == CT_LIST_DATA_EMAIL){
				err = contacts_record_get_str_p(popupItemData->record, _contacts_person_email.email, &value);
				WPWARN(err != CONTACTS_ERROR_NONE, "contacts_record_get_str_p() fail(%d)",err);
			}
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
	popup->setTitle(personData->getDisplayName());
	popup->setContent([this,itc,checked_numberList,checked_emailList](Evas_Object* parent)->Evas_Object*
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
				if(popupItemData->record)
					contacts_record_destroy(popupItemData->record, true);
				delete popupItemData;
				WERROR("contacts_record_clone() Failed(%d)", err);
				break;
			}
			popupItemData->controller = this;
			popupItemData->dataType = CT_LIST_DATA_NUMBER;

			popupItemData->popupItem = elm_genlist_item_append(genlist, itc, (void*)popupItemData, NULL, ELM_GENLIST_ITEM_NONE, CtListUgSinglePickController::__onSelectDataPopupItem, popupItemData);
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
				if(popupItemData->record)
					contacts_record_destroy(popupItemData->record, true);
				delete popupItemData;
				break;
			}
			popupItemData->controller = this;
			popupItemData->dataType = CT_LIST_DATA_EMAIL;

			popupItemData->popupItem = elm_genlist_item_append(genlist, itc, (void*)popupItemData, NULL, ELM_GENLIST_ITEM_NONE, CtListUgSinglePickController::__onSelectDataPopupItem, popupItemData);
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

Evas_Object* CtListUgSinglePickController::__createNumberPopup(CtListDataPerson *personData)
{
	CtNumberPopup *popup = new CtNumberPopup(personData);
	popup->setOnResult(std::bind(&CtListUgSinglePickController::__doReplySelectedInfo,
			this, std::placeholders::_1));
	__listView->attachPopup(popup);
	return popup->getEvasObj();
}

void CtListUgSinglePickController::__onSelectDataPopupItem(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtListPopupItemData *popupItemData = (CtListPopupItemData*)data;
	int err = CONTACTS_ERROR_NONE;

	popupItemData->controller->__listView->destroyPopup();

	int dataId = 0;
	char *dataString = NULL;
	if(popupItemData->dataType == CT_LIST_DATA_NUMBER) {
		err = contacts_record_get_int(popupItemData->record, _contacts_person_number.number_id, &dataId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		err = contacts_record_get_str_p(popupItemData->record, _contacts_person_number.number, &dataString);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		popupItemData->controller->__selectedPersonData->setDataId(dataId);
		popupItemData->controller->__selectedPersonData->setDataString(dataString);
		popupItemData->controller->__selectedPersonData->setDataType(LIST_RESULT_NUMBER);

		if (popupItemData->controller->__resultType
				== LIST_RESULT_NUMBER_OR_EMAIL) {
			popupItemData->controller->__createNumberPopup(popupItemData->controller->__selectedPersonData);
			return;
		}
	}
	else if(popupItemData->dataType == CT_LIST_DATA_EMAIL) {
		err = contacts_record_get_int(popupItemData->record, _contacts_person_email.email_id, &dataId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		err = contacts_record_get_str_p(popupItemData->record, _contacts_person_email.email, &dataString);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		popupItemData->controller->__selectedPersonData->setDataId(dataId);
		popupItemData->controller->__selectedPersonData->setDataString(dataString);
		popupItemData->controller->__selectedPersonData->setDataType(LIST_RESULT_EMAIL);
	}

	popupItemData->controller->__doReplySelectedInfo(popupItemData->controller->__selectedPersonData);
}
