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

#include <dlfcn.h>
#include <vector>
#include <Elementary.h>
#include <app_preference.h>
#include <app.h>
#include <contacts.h>
#include <Ecore_File.h>
#include <app_preference.h>

#include "WView.h"
#include "WNaviframe.h"
#include "WPopup.h"
#include "WMenuPopup.h"
#include "WToastPopup.h"

#include "ContactsDebug.h"
#include "ContactsAppControl.h"
#include "CtDetailView.h"
#include "CtType.h"
#include "CtListBasicController.h"
#include "CtListDataPerson.h"
#include "CtCommon.h"
#include "CtContactsToVcard.h"
#include "CtPath.h"
#include "CtListView.h"
#include "CtListModelDb.h"
#include "CtRegistryMgr.h"

#define __ITEM_HEIGHT 96

bool CtListBasicController::__stopDoAsyncAction = false;

CtListBasicController::CtListBasicController(CtListCtrlType ctrlType, CtListView *listView, CtListModelDb* listModel)
{
	WHIT();

	__listView = listView;
	__listModel = listModel;
	__ctrlType = ctrlType;
	__progressPopup = NULL;
	__updateDataType = 0;
	__canceled = false;
	__stopDoAsyncAction = false;
	__doActionThread = NULL;
	__settingDllHandle = NULL;
	__detailDllHandle = NULL;
	__cmDllHandle = NULL;

	preference_set_changed_cb(CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP, __onPreferenceChangedCb, this);
}

CtListBasicController::~CtListBasicController()
{
	WHIT();

	preference_unset_changed_cb(CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP);

	__stopDoAsyncAction = true;

	__canceled = true;

	if (auto popup = __popupMonitor.lock()) {
		popup->destroy();
	}

	if(__doActionThread){
		__doActionThread->destroy();
		__doActionThread = NULL;
	}

}

void CtListBasicController::__onPreferenceChangedCb(const char *key, void *data)
{
	CtListBasicController * listCtrl = static_cast<CtListBasicController*>(data);

	if (strcmp(key, CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP) == 0) {
		bool showPopup = false;

		int error = preference_get_boolean(CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP, &showPopup);
		WPRET_M(error != PREFERENCE_ERROR_NONE, "preference_get_boolean failed");

		WDEBUG("showPopup %d", showPopup);

		if (showPopup) {
			preference_set_boolean(CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP, false);
			WToastPopup* popup = new WToastPopup("Some of your contacts appear to be duplicates. Manage this in Link contacts.");
			listCtrl->__listView->attachPopup(popup);

		}
	}
}

bool CtListBasicController::__changeCheckState(Elm_Object_Item *item, bool checkState)
{
	WHIT();

	Elm_Genlist_Item_Type item_type = elm_genlist_item_type_get(item);
	if(item_type == ELM_GENLIST_ITEM_GROUP) {
		return false;
	}

	Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
	Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	WPRET_VM(itemData == NULL, false, "itemData is null");

	itemData->personData->setAsChecked(checkState);
	if(itemData->personData->isChecked() == true) {
		__listModel->pushCheckedData(itemData->personData);
	}
	else {
		__listModel->removeCheckedData(itemData->personData);
	}
	elm_check_state_set(check, (Eina_Bool)checkState);

	int checkedCount = __listModel->getCheckedDataCount();
	Elm_Object_Item *naviItem = elm_naviframe_top_item_get(__listView->getViewNaviframe());
	Evas_Object *btn = elm_object_item_part_content_get(naviItem, "title_right_btn");
	if(btn) {
		if(checkedCount > 0)
			elm_object_disabled_set(btn, EINA_FALSE);
		else
			elm_object_disabled_set(btn, EINA_TRUE);
	}

	WDEBUG("%d, %d", checkedCount, __listModel->getPersonListCount(LIST_QUERY_DEFAULT));

	if (__listView->__onSearching) {
		if(__listView->__selectAllObj) {
			if (__listModel->checkIsSelectAllOnSearching()) {
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
	else {
		if(__listView->__selectAllObj) {
			if( checkedCount == __listModel->getPersonListCount(LIST_QUERY_DEFAULT)) {
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

	char buf[CT_TEXT_SHORT_LEN] = {0,};
	if(__listModel->getLimitCount() == 0)
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), checkedCount);
	else
		snprintf(buf, sizeof(buf), "%d/%d", checkedCount, __listModel->getLimitCount());
	elm_object_item_part_text_set(naviItem, "elm.text.title", buf);

	return (bool)elm_check_state_get(check);
}

bool CtListBasicController::__getCheckState(Elm_Object_Item *item)
{
	WHIT();
	Evas_Object* layout = elm_object_item_part_content_get(item,"elm.icon.2");
	Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");


	return (bool)elm_check_state_get(check);
}



void CtListBasicController::onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();
	WPRET_M(!view, "invalid view");
	Elm_Object_Item *item = selectedItem;
	elm_genlist_item_selected_set(selectedItem, EINA_FALSE);

	CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);

	WDEBUG("__ctrlType %d", __ctrlType);

	switch(__ctrlType){
	case LIST_CTRL_DEFAULT:
	{
		WView *detailView = new CtDetailView(itemData->personData->getPersonId());
		view->getNaviframe()->push(detailView);
	}
		break;
	case LIST_CTRL_DELETE_CONTACTS:
	case LIST_CTRL_SHARE_CONTACTS:
	case LIST_CTRL_ADD_FAVORITES:
	case LIST_CTRL_REMOVE_FAVORITES:
		__changeCheckState(item, checkState);
		break;
	default:
		WWARNING("Not handled type=%d", __ctrlType);
		break;
	}

	elm_genlist_realized_items_update(view->__genlist);
}

void CtListBasicController::onSelectCreate()
{
	launchContactCreate(NULL);
}

void CtListBasicController::onSelectDone()
{
	switch(__ctrlType){
	case LIST_CTRL_DEFAULT:
		break;
	case LIST_CTRL_DELETE_CONTACTS:
		__canceled = false;
		__doDeleteContacts();
	break;
	case LIST_CTRL_SHARE_CONTACTS:
		__doShareContacts();
	break;
	case LIST_CTRL_ADD_FAVORITES:
	case LIST_CTRL_REMOVE_FAVORITES:
		__doFavoritesContacts(true);
		break;
	case LIST_CTRL_REORDER_FAVORITES:
		__doSetFavoritesOrder();
		break;
	default:
		break;
	}
}

void CtListBasicController::onSelectCancel()
{
	WHIT();
	switch(__ctrlType)
	{
	case LIST_CTRL_DELETE_CONTACTS :
	case LIST_CTRL_SHARE_CONTACTS :
	case LIST_CTRL_ADD_FAVORITES :
	case LIST_CTRL_REMOVE_FAVORITES :
	case LIST_CTRL_REORDER_FAVORITES :
		__listView->popOut();
		break;
	default:
		WWARNING("Not handled type=%d", __ctrlType);
		break;
	}
}

void CtListBasicController::onSelectBack()
{
	WHIT();
}

void CtListBasicController::onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();
	view->__selectAllState = checkState;

	Elm_Object_Item *item = selectedItem;
	if (item)
		elm_genlist_item_selected_set(item, EINA_FALSE);

	Evas_Object *genlist = view->__genlist;

	WDEBUG("select all %d", checkState);

	int index = 0;
	std::shared_ptr<CtListDataPerson> personData = view->__listModel->getAtPersonData(index, LIST_QUERY_DEFAULT);
	while(personData) {
		if(checkState == true && !personData->isChecked()) {
			__listModel->doCheckedDataNoRelation(personData, true);
		}
		else if (checkState == false && personData->isChecked()) {
			__listModel->doCheckedDataNoRelation(personData, false);
		}
		personData->setAsChecked(checkState);
		index++;

		personData = view->__listModel->getAtPersonData(index, LIST_QUERY_DEFAULT);
	}

	if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
		index = 0;
		personData = view->__listModel->getAtPersonData(index, LIST_QUERY_MFC);
		while (personData) {
			if(checkState == true && !personData->isChecked()) {
				__listModel->doCheckedDataNoRelation(personData, true);
			}
			else if (checkState == false && personData->isChecked()) {
				__listModel->doCheckedDataNoRelation(personData, false);
			}
			personData->setAsChecked(checkState);
			index++;
			personData = view->__listModel->getAtPersonData(index, LIST_QUERY_MFC);
		}
	}

	elm_genlist_realized_items_update(genlist);
}

void CtListBasicController::__moreManageFavoritesItemOperation()
{
	WENTER();

	Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
	WPRET_M(!itc, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")){
			if (1 == (long) data)
				return SAFE_STRDUP(V_("IDS_PB_OPT_ADD"));
			else if (2 == (long) data)
				return SAFE_STRDUP(V_("IDS_PB_OPT_REMOVE"));
			else if (3 == (long) data)
				return SAFE_STRDUP(V_("IDS_PB_HEADER_REORDER"));
			else
				return NULL;
		}
		return NULL;
	};

	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle( "IDS_PB_HEADER_MANAGE_FAVOURITES_ABB");
	popup->setContent([this,itc](Evas_Object* parent)->Evas_Object*
	{
		Evas_Object* genlist = elm_genlist_add(parent);
		elm_object_style_set( genlist, "popup" );
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated
		elm_object_content_set(parent, genlist);

		elm_genlist_item_append(genlist, itc, (void*)1, NULL, ELM_GENLIST_ITEM_NONE,this->__onAddFavoritesSelCb,this);

		Elm_Object_Item *item2 = elm_genlist_item_append(genlist, itc, (void*)2, NULL, ELM_GENLIST_ITEM_NONE,this->__onRemoveFavoritesSelCb,this);
		Elm_Object_Item *item3 = elm_genlist_item_append(genlist, itc, (void*)3, NULL, ELM_GENLIST_ITEM_NONE,this->__onReorderFavoritesSelCb,this);

		int count = 0;
		count = __listModel->getContactsCount(LIST_QUERY_FAVORITE);
		if(count == 0) {
			if(0 == __listModel->getContactsCount(LIST_QUERY_MFC))
				elm_object_item_disabled_set(item2, EINA_TRUE);
			elm_object_item_disabled_set(item3, EINA_TRUE);
		}
		else {
			if(count == 1)
				elm_object_item_disabled_set(item3, EINA_TRUE);
		}

		evas_object_show(genlist);

		return genlist;

	});

	__listView->attachPopup(popup);

	elm_genlist_item_class_free(itc);

	WLEAVE();

}


void CtListBasicController::onSelectMenu()
{
	WENTER();

	WMenuPopup* popup = new WMenuPopup();
	popup->prepare(__listView->getViewWindow(), __listView->getViewNaviframe());

	int listCount = __listView->__listModel->getPersonListCount(LIST_QUERY_DEFAULT);
	if(listCount == 0) // make sure the count
		listCount = __listView->__listModel->getContactsCount(LIST_QUERY_DEFAULT);

	if(listCount > 0) {
		popup->appendItem(V_("IDS_PB_BUTTON_DELETE_ABB4"), [this]()
				{
					int personId = __listView->getListTopItemPersonId();
					CtListView *deleteView = new CtListView(LIST_VIEW_CHECK, LIST_DATA_ALL_PERSON, LIST_CTRL_DELETE_CONTACTS);
					deleteView->setTopItemOnList(personId);
					__listView->getWNaviframe()->push(deleteView, NULL, NULL);
				});
		popup->appendItem(V_("IDS_PB_HEADER_SHARE_VIA"), [this]()
				{
					__listView->getWNaviframe()->push(new CtListView(LIST_VIEW_CHECK, LIST_DATA_PERSON_EXCEPT_FACEBOOK, LIST_CTRL_SHARE_CONTACTS), NULL, NULL);
				});
		popup->appendItem(V_("IDS_PB_HEADER_MANAGE_FAVOURITES_ABB"), [this]()
				{
					__moreManageFavoritesItemOperation();
				});
	}

	popup->appendItem( V_("IDS_PB_OPT_SETTINGS"), [this]()
			{
				WHIT();
				if (__settingDllHandle == NULL) {
					__settingDllHandle = ctGetDllHandle("libct-setting.so");
				}

				typedef WView* (*get_setting_view_t)();
				get_setting_view_t createSettingView = (get_setting_view_t)dlsym(__settingDllHandle, "createSettingView");
				if (createSettingView) {
					WView* settingView = createSettingView();
					__listView->getWNaviframe()->push(settingView, NULL, NULL);
				}
			});

	__listView->attachPopup(popup);
}

void CtListBasicController::__onAddFavoritesSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	//CONTACTS_LOGGING_LOG(LOGGING_LOG_ADD_TO_FAVOR); //DO NOT REMOVE!!!
	CtListBasicController * listCtrl = static_cast<CtListBasicController*>(data);
	listCtrl->__listView->destroyPopup();
	listCtrl->__listView->getWNaviframe()->push(new CtListView(LIST_VIEW_CHECK, LIST_DATA_PERSON_EXCEPT_SIM, LIST_CTRL_ADD_FAVORITES), NULL, NULL);

}

void CtListBasicController::__onRemoveFavoritesSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	//CONTACTS_LOGGING_LOG(LOGGING_LOG_REMOVE_FROM_FAVOR); //DO NOT REMOVE!!!
	CtListBasicController * listCtrl = static_cast<CtListBasicController*>(data);
	listCtrl->__listView->destroyPopup();
	listCtrl->__listView->getWNaviframe()->push(new CtListView(LIST_VIEW_CHECK, LIST_DATA_ALL_FAVORITES, LIST_CTRL_REMOVE_FAVORITES), NULL, NULL);
}

void CtListBasicController::__onReorderFavoritesSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtListBasicController * listCtrl = static_cast<CtListBasicController*>(data);
	listCtrl->__listView->destroyPopup();
	listCtrl->__listView->getWNaviframe()->push(new CtListView(LIST_VIEW_REORDER, LIST_DATA_ALL_FAVORITES, LIST_CTRL_REORDER_FAVORITES), NULL, NULL);
}

int CtListBasicController::__setImage(contacts_record_h record, char *image)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	if(image && *image) {
		err = contacts_record_set_str(record, _contacts_image.path, image);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	//	err = contacts_record_set_int(record, _contacts_image.type, CONTACTS_IMAGE_TYPE_EX_CUSTOM);
	//	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
	}
	return err;
}

int CtListBasicController::__removeImage(contacts_record_h contactRecord, contacts_record_h imageRecord)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(contactRecord, _contacts_contact.image, imageRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	err = contacts_record_destroy(imageRecord, true);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_destroy() Failed(%d)", err);

	return err;
}

bool CtListBasicController::__setFavoriteImage(int personId, bool set, contacts_record_h contactRecord)
{
	return true;
}

//typedef struct {
//	contacts_list_h personRecordList;
//	contacts_list_h contactRecordList;
//} CtListContactList;

bool CtListBasicController::__getHasReadOnlyMode(char* addressbookIDs)
{
	WHIT();
	int addressbookId = -1;
	char* tok = NULL;
	contacts_record_h addressbookRecord = NULL;
	int err = CONTACTS_ERROR_NONE;
	tok = strtok (addressbookIDs," ");
	int addressbookMode = 0;
	while (tok != NULL)
	{
		WDEBUG ("%s",tok);
		addressbookId = atoi(tok);

		err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &addressbookRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_db_get_record() Failed(%d)", err);

		err = contacts_record_get_int(addressbookRecord, _contacts_address_book.mode, &addressbookMode);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_destroy(addressbookRecord, true);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_destroy() Failed(%d)", err);

//		if (CONTACTS_ADDRESS_BOOK_MODE_EX_READONLY & addressbookMode) {
		if (CONTACTS_ADDRESS_BOOK_MODE_READONLY & addressbookMode) {
			return true;
		}
		tok = strtok (NULL, " ");
	}
	return false;
}

void CtListBasicController::__doFavoritesContacts(bool addFirstOne)
{
	WHIT();

	bool updateMfc = false;
	bool updateFavorite = false;
	bool isfavorite = false;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	contacts_list_h contactRecordList = NULL;
	int displayContactId =  0;
	const int LIST_BULK_LENGTH = 100;
	int count =0;

	if(__progressPopup == NULL) {
		__checkedData.reset(__listModel->getCheckedDataList());
		if ((int)__checkedData->size() == 0)
			return;

		__listView->setOnProgressOperation(true);
		if(__ctrlType == LIST_CTRL_ADD_FAVORITES)
			__progressPopup = new CtProgressBarPopup(__checkedData->size(), V_("IDS_PB_BODY_ADD_TO_FAVOURITES"),true);
		else if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES)
			__progressPopup = new CtProgressBarPopup(__checkedData->size(), V_("IDS_PB_BODY_REMOVE_FROM_FAVOURITES"),true);
		if (__progressPopup) {
			__progressPopup->addButton(V_("IDS_PB_BUTTON_CANCEL"), [this](bool* destroyPopup)->bool{
				WDEBUG("Cancel");
				__canceled = true;
				*destroyPopup = true;
				return true;
			});
			__progressPopup->create(__listView->getViewNaviframe(), NULL);
			__popupMonitor = __progressPopup->getWeakPtr();
		}
	}
	else if(__popupMonitor.lock()){
		__progressPopup->setProgressRemainCount(__checkedData->size());
	}
	else{
		__listView->setOnProgressOperation(false);
		__progressPopup = NULL;
		__listView->popOut();
		return;
	}

	if(__ctrlType == LIST_CTRL_ADD_FAVORITES){
		isfavorite = true;
	}
	else if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
		isfavorite = false;
	}
	else {
		WERROR("__ctrlType() Failed(%d)", __ctrlType);
	}
	WDEBUG("isfavorite %d", isfavorite);

	err = contacts_list_create(&contactRecordList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_create() Failed(%d) - contactRecordList", err);

	if((int)__checkedData->size() > LIST_BULK_LENGTH) {
		for(auto iter = __checkedData->begin() ;count < LIST_BULK_LENGTH ;++iter) {
			int personId = (*iter)->getPersonId();
			displayContactId = 0;
			// remove mfc
			if (__listModel->checkMostFrequentContact((personId))) {
				err = contacts_person_reset_usage(personId, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_person_reset_usage() Failed(%d)", err);
				count++;
				updateMfc = true;
				continue;
			}
			//get person
			err = contacts_db_get_record(_contacts_person._uri, personId, &record);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_db_get_record() Failed(%d) - _contacts_person", err);
				continue;
			}
			char *thumbnailPath = NULL;
			err = contacts_record_get_str_p(record, _contacts_person.image_thumbnail_path, &thumbnailPath);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

			char *addressbookIDs =NULL;
			err = contacts_record_get_str_p(record, _contacts_person.addressbook_ids, &addressbookIDs);
			if(CONTACTS_ERROR_NONE != err)
			{
				WERROR("contacts_record_get_str_p() Failed(%d)", err);
				continue;
			}
			if(__getHasReadOnlyMode(addressbookIDs) == true)
			{
				err = contacts_record_set_bool(record, _contacts_person.is_favorite, isfavorite);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_bool() Failed(%d)", err);
				err = contacts_db_update_record(record);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);
				continue;
			}

			err = contacts_record_get_int(record, _contacts_person_contact.display_contact_id, &displayContactId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
			contacts_record_destroy(record, true);

			Eina_List *contactList = NULL;
			void *data = NULL;
			Eina_List *cursor = NULL;
			contactList = ctCommonGetContactsList(personId);

			EINA_LIST_FOREACH(contactList, cursor, data) {
				contacts_record_h contactRecord = (contacts_record_h)data;
				int contactId = -1;
				err = contacts_record_set_bool(contactRecord, _contacts_contact.is_favorite, isfavorite);
				if(CONTACTS_ERROR_NONE != err) {
					WERROR("contacts_record_set_bool() Failed(%d)", err);
					contacts_record_destroy(contactRecord, true);
					continue;
				}

				err = contacts_record_get_int(contactRecord, _contacts_contact.id, &contactId);
				WWARNING ("contactId : %d, displayContactId id :%d",contactId, displayContactId);
				//if(contactId == displayContactId) {
					contacts_record_h imageRecord = NULL;
					err = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.image, 0, &imageRecord);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_child_record_at_p() Failed(%d) - contact_list", err);

					if(imageRecord) {
						int imageType = 0;
						err = contacts_record_get_int(imageRecord, _contacts_image.type, &imageType);
						if(CONTACTS_ERROR_NONE != err) {
							WERROR("contacts_record_get_int() Failed(%d)", err);
							contacts_record_destroy(contactRecord, true);
							continue;
						}
//						if(imageType != CONTACTS_IMAGE_TYPE_EX_CUSTOM) {
						if(imageType != CONTACTS_IMAGE_TYPE_CUSTOM) {
							WERROR("imageType is Not Custom(%d)", imageType);
							err = contacts_list_add(contactRecordList, contactRecord);
							WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_add() Failed(%d) - contact_list", err);
							continue;
						}
						else {
							//remove default image.
							if(!isfavorite) {
								__removeImage(contactRecord,imageRecord);
							}
						}
					}
					else {
						if(thumbnailPath == NULL) {
							WDEBUG("thumbnailPath is NULL");
						}
					}
				//}

				err = contacts_list_add(contactRecordList, contactRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_add() Failed(%d) - contact_list", err);
			}

			count++;
			updateFavorite = true;
			if(addFirstOne)
				break;
		}
		if(addFirstOne)
			__checkedData->erase(__checkedData->begin(), __checkedData->begin()+(1));
		else
			__checkedData->erase(__checkedData->begin(), __checkedData->begin()+(LIST_BULK_LENGTH-1));
	}
	else if((int)__checkedData->size() > 0) {
		for(auto iter = __checkedData->begin() ;iter != __checkedData->end();++iter) {
			int personId = (*iter)->getPersonId();
			displayContactId = 0;
			// remove mfc
			if (__listModel->checkMostFrequentContact(personId)) {
				err = contacts_person_reset_usage(personId, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_person_reset_usage() Failed(%d)", err);
				updateMfc = true;
				continue;
			}
			WDEBUG("personId %d", personId);
			err = contacts_db_get_record(_contacts_person._uri, personId, &record);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_db_get_record() Failed(%d) - _contacts_person", err);
				break;
			}

			char *thumbnailPath = NULL;
			err = contacts_record_get_str_p(record, _contacts_person.image_thumbnail_path, &thumbnailPath);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

			char *addressbookIDs =NULL;
			err = contacts_record_get_str_p(record, _contacts_person.addressbook_ids, &addressbookIDs);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

			if(__getHasReadOnlyMode(addressbookIDs) == true)
			{
				err = contacts_record_set_bool(record, _contacts_person.is_favorite, isfavorite);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_bool() Failed(%d)", err);
				err = contacts_db_update_record(record);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);
				continue;
			}
			err = contacts_record_get_int(record, _contacts_person_contact.display_contact_id, &displayContactId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
			contacts_record_destroy(record, true);
			WDEBUG("displayContactId %d", displayContactId);
			Eina_List *contactList = NULL;
			void *data = NULL;
			Eina_List *cursor = NULL;
			contactList = ctCommonGetContactsList(personId);
			EINA_LIST_FOREACH(contactList, cursor, data) {
				contacts_record_h contactRecord = (contacts_record_h)data;

				err = contacts_record_set_bool(contactRecord, _contacts_contact.is_favorite, isfavorite);
				if(CONTACTS_ERROR_NONE != err) {
					WERROR("contacts_record_set_bool() Failed(%d)", err);
					contacts_record_destroy(contactRecord, true);
					continue;
				}

				int contactId = -1;
				err = contacts_record_get_int(contactRecord, _contacts_contact.id, &contactId);
				if(CONTACTS_ERROR_NONE != err) {
					WERROR("contacts_record_get_int() Failed(%d)", err);
					contacts_record_destroy(contactRecord, true);
					continue;
				}
				WWARNING ("contactId : %d, displayContactId id :%d",contactId, displayContactId);
				//if(contactId == displayContactId) {
					contacts_record_h imageRecord = NULL;
					err = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.image, 0, &imageRecord);
					WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_child_record_at_p() Failed(%d) - contact_list", err);
					if(imageRecord) {
						int imageType = 0;
						err = contacts_record_get_int(imageRecord, _contacts_image.type, &imageType);
						if(CONTACTS_ERROR_NONE != err) {
							WERROR("contacts_record_get_int() Failed(%d)", err);
							contacts_record_destroy(contactRecord, true);
							continue;
						}
//						if(imageType != CONTACTS_IMAGE_TYPE_EX_CUSTOM) {
							if(imageType != CONTACTS_IMAGE_TYPE_CUSTOM) {
							WERROR("imageType is Not Custom(%d)", imageType);
							err = contacts_list_add(contactRecordList, contactRecord);
							WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_add() Failed(%d) - contact_list", err);
							continue;
						}
						else {
							//remove default image.
							if(!isfavorite) {
								__removeImage(contactRecord,imageRecord);
							}
						}
					}
					else {
						if(thumbnailPath == NULL) {
							WDEBUG("thumbnailPath is NULL");
						}
					}
				//}
				err = contacts_list_add(contactRecordList, contactRecord);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_add() Failed(%d) - contact_list", err);
			}
			count++;
			updateFavorite = true;
		}
		__checkedData->clear();
	}

	if(__doActionThread){
		__doActionThread->destroy();
		__doActionThread = NULL;
	}
	__doActionThread = new CtThread(__onDoFavouritesThreadRunCb, contactRecordList, __onDoFavouritesThreadEndCb, this);
	__doActionThread->start();

	WDEBUG("updateFavorite %d updateMfc %d", updateFavorite, updateMfc);
	if(updateMfc && false == updateFavorite) {
		CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_MFC_UPDATED, 1);
	}
}

void CtListBasicController:: __onDoFavouritesThreadRunCb(void *data)
{
	WHIT();
	contacts_list_h contactRecordList = (contacts_list_h)data;

	int err = CONTACTS_ERROR_NONE;

	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_FAVORITE_UPDATED, 1);

	contacts_connect_on_thread();
	err = contacts_db_update_records(contactRecordList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_records() Failed(%d)", err);

	contacts_list_destroy(contactRecordList, true);
	contacts_disconnect_on_thread();

	WLEAVE();
}

void CtListBasicController::__onDoFavouritesThreadEndCb(void *data)
{
	WHIT();
	CtListBasicController * listCtrl = static_cast<CtListBasicController*>(data);

	if (__stopDoAsyncAction) return;
	if (listCtrl->__checkedData->empty()){
		if (auto popup = listCtrl->__popupMonitor.lock()) {
			popup->destroy();
		}
		listCtrl->__listView->setOnProgressOperation(false);
		listCtrl->__progressPopup = NULL;
		listCtrl->__listView->popOut();
	}
	else{
		listCtrl->__doFavoritesContacts(false);
	}
}

void CtListBasicController::__doSetFavoritesOrder()
{
	WENTER();

	int err = CONTACTS_ERROR_NONE;

	for (auto iter = __listView->__reorderData.begin(); iter != __listView->__reorderData.end(); iter++) {
		err = contacts_person_set_favorite_order(iter->personId, iter->prevPersonId, iter->nextPersonId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_person_set_favorite_order() Failed(%d) %d %d %d", err, iter->personId, iter->prevPersonId, iter->nextPersonId);
	}

	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_FAVORITE_UPDATED, 1);

	__listView->popOut();
	WLEAVE();
}

std::string CtListBasicController::__makeCheckedDataPath(int personId)
{
	WENTER();
	contacts_record_h personRecord = NULL;

	std::string vcardPath;
	do {
		int err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		if (err != CONTACTS_ERROR_NONE) {
			WDEBUG("contacts_db_get_record FAILED");
			break;
		}

		vcardPath = createVcardFile(personRecord);

		if(vcardPath.empty()) {
			WDEBUG("createVcardPath FAILED");
			break;
		}

	} while(false);

	if (personRecord) {
		contacts_record_destroy(personRecord, true);
	}

	WLEAVE();
	return vcardPath;
}

void CtListBasicController::__doShareContacts()
{
	WENTER();
	std::unique_ptr<std::vector<std::shared_ptr<CtListDataPerson>>> checkedData(__listModel->getCheckedDataList());
	int checkedIdCount = checkedData->size();
	WPRET_M(!checkedIdCount, "checked id count is null");

	std::vector<char *> result;
	result.reserve(checkedIdCount);

	for (auto &data : *checkedData) {
		char buf[CT_TEXT_SHORT_LEN] = { 0, };
		snprintf(buf, sizeof(buf), "%d", data->getPersonId());
		result.push_back(strdup(buf));
	}

	if (launchMultiShareContacts((const char**) result.data(), checkedIdCount) != TIZEN_ERROR_NONE) {
		WERROR("launchMultiShareContacts failed");
	}

	for (auto &id : result) {
		free(id);
	}

	__listView->popOut();
}

void CtListBasicController::__doDeleteContacts(void)
{
	WENTER();

	int checkedIdCount = 0;

	if(__progressPopup == NULL) {
		__checkedData.reset(__listModel->getCheckedDataList());
		checkedIdCount = __checkedData->size();
		if ( 0 == checkedIdCount)
			return;
		std::shared_ptr<CtListDataPerson> personData;

		//delete just only 1 contact  for performance
		if ( 1 == checkedIdCount) {
			int idx = 0;

			__listView->popOut();

			while(true) {
				personData = __listModel->getAtPersonData(idx, LIST_QUERY_DEFAULT);
				if(personData == NULL)
					break;
				if(personData->isChecked()) {
					int err = CONTACTS_ERROR_NONE;
					int  personId = personData->getPersonId();
					CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_DELETED_PERSON, 1);
					CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_DELETED_PERSON_ID, personId);
					err = contacts_db_delete_record(_contacts_person._uri, personId);
					WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_delete_record() fail(%d)",err);
				}
				idx++;
			}
			return;
		}

		__listView->setOnProgressOperation(true);
		__progressPopup = new CtProgressBarPopup(checkedIdCount, V_("IDS_PB_POP_DELETING_CONTACTS_ING"),true);
		__progressPopup->addButton(V_("IDS_PB_BUTTON_CANCEL"), [this](bool* destroyPopup)->bool{
			WDEBUG("Cancel");
			__listView->setOnProgressOperation(false);
			__canceled = true;
			*destroyPopup = true;
			return true;
		});
		__progressPopup->create(__listView->getViewNaviframe(), NULL);
		__popupMonitor = __progressPopup->getWeakPtr();
	}

	if (__canceled) {
		WDEBUG("delete is Canceled");
		return;
	}

	if(__checkedData->size() > 0){
		__doActionThread = new CtThread(__onDoDeleteThreadRunCb, this, __onDoDeleteThreadEndCb, this);
		__doActionThread->start();
	}
}

void CtListBasicController:: __onDoDeleteThreadRunCb(void *data)
{
	WHIT();
	CtListBasicController * listCtrl = (CtListBasicController*)data;
	int err = CONTACTS_ERROR_NONE;

	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_DELETED_PERSON, 1);

	for(size_t i = 0; i < listCtrl->__checkedData->size(); ++i) {
		err = contacts_db_delete_record(_contacts_person._uri, listCtrl->__checkedData->at(i)->getPersonId());
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_delete_record() Failed(%d)", err);
		listCtrl->__progressPopup->setProgressRemainCount(listCtrl->__checkedData->size() - i);
	}

	listCtrl->__checkedData->clear();
	WLEAVE();
}

void CtListBasicController::__onDoDeleteThreadEndCb(void *data)
{
	WENTER();

	CtListBasicController * listCtrl = (CtListBasicController*)data;
	if (__stopDoAsyncAction) return;

	listCtrl->__listView->setOnProgressOperation(false);
	listCtrl->__progressPopup = NULL;
	listCtrl->__listView->popOut();
}

