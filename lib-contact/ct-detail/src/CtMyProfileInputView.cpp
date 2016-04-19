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

#include <account.h>
#include <app_preference.h>
#include <contacts.h>

#include "WPopup.h"

#include "CommonPath.h"
#include "ContactsDebug.h"

#include "CtPath.h"
#include "CtMyProfileDetailView.h"
#include "CtMyProfileInputData.h"
#include "CtMyProfileInputView.h"
#include "CtCommon.h"

#include "CtInputViewGenlistItem.h"
#include "CtInputViewBasicInfoItem.h"
#include "CtInputViewEmailItem.h"
#include "CtInputViewNoteItem.h"
#include "CtInputViewMessengerItem.h"
#include "CtInputViewAddressItem.h"
#include "CtInputViewWebsiteItem.h"
#include "CtInputViewEventItem.h"
#include "CtInputViewNumberItem.h"
#include "CtInputViewMsisdnItem.h"
#include "CtInputViewImportantFieldsItem.h"
#include "CtInputViewImportantSubItem.h"
#include "CtThemeMgr.h"

#define GALLERY_UG "ug-gallery-efl"
#define __ITEM_HEIGHT 96
#define DATE_TIME_FORMAT "%%c"

CtMyProfileInputView::CtMyProfileInputView(contacts_record_h contactRecord)
{
	WHIT();
	__inputData = std::shared_ptr<CtData>( new CtMyProfileInputData(contactRecord) );
	__initialize();
}

CtMyProfileInputView::~CtMyProfileInputView()
{
	WHIT();
	destroyPopup();
	free(__basicInfoFullName);
}

void CtMyProfileInputView::__initialize()
{
	__contactId = 0;
	__appendIndex = 0;
	__saved = false;
	__discard = false;
	__changed = false;

	__basicInfoFullName = NULL;
	__baseInfoExpanded = false;
	__isCreate = __inputData->getContactId() > 0 ? false : true;

	memset( __genlistItems, 0, sizeof(__genlistItems) );

	__birthdayEventExist = std::make_shared<bool> (false);
}

void CtMyProfileInputView::onDbChanged(const char* viewUri)
{
	WENTER();
}

void CtMyProfileInputView::onCompleteGetContacts(int offset)
{
	WENTER();
}

void CtMyProfileInputView::onConditionChagned()
{
	WENTER();
}


Evas_Object* CtMyProfileInputView::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();
	if (__isCreate)
		setTitle(V_("IDS_PB_SK_CREATE_ABB"));
	else
		setTitle(V_("IDS_PB_OPT_EDIT"));

	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	std::string buttonStylesEdjePath = ContactsCommon::getResourcePath(COMMON_BUTTONS_EDJ);
	Evas_Object *base = createBaseLayout(parent);
	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->addExtensionTheme(buttonStylesEdjePath.c_str());
	CtThemeMgr::getInstance()->setObjectTheme(base);

	Evas_Object* genlist = elm_genlist_add(base);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_show(genlist);
	__genlist = genlist;

	elm_object_part_content_set(base, "elm.swallow.content", genlist);
	evas_object_smart_callback_add(genlist, "realized", __genlistRealizedCb, this);
	evas_object_smart_callback_add(genlist, "unrealized", __genlistUnrealizedCb, this);

	return base;
}

void CtMyProfileInputView::onLanguageChanged()
{
	WHIT();

	destroyPopup();

	elm_object_text_set(__saveBtn, V_("IDS_PB_BUTTON_DONE_ABB3"));

	Evas_Object* cancelBtn = elm_object_item_part_content_get(getNaviItem(), "title_left_btn");
	elm_object_text_set(cancelBtn, V_("IDS_PB_BUTTON_CANCEL"));

	if (__isCreate) {
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", V_("IDS_PB_BUTTON_CREATE_ABB2"));
	}
	else {	//TODO: translation needed
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", "EDIT");
	}

	if(__genlist)
		elm_genlist_realized_items_update(__genlist);
}

Evas_Object* CtMyProfileInputView::__getWindowEvasObj(void)
{
	if(getWindow())
		return getWindow()->getEvasObj();
	else
		WASSERT_EX(1, "window null");
	return NULL;
}
Evas_Object* CtMyProfileInputView::__getNaviframeEvasObj(void)
{
	if (getNaviframe()) {
		return getNaviframe()->getEvasObj();
	}
	else {
		return NULL;
	}
}


void CtMyProfileInputView:: __genlistRealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;
	Elm_Object_Item *objectItem = (Elm_Object_Item*) event_info;
	CtInputViewGenlistItem *item = (CtInputViewGenlistItem*) elm_object_item_data_get(objectItem);

	if (item && !item->getElmObjectItem()) {
		item->setElmObjectItem(objectItem);
	}

	view->__updateEntryReturnKeyType(NULL);

	if (item) {
		item->onRealized();
	}

	if (objectItem == view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO]) {
		view->__updateBasicInfoItem();
	}
}


void CtMyProfileInputView::__genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *objectItem = (Elm_Object_Item*) event_info;
	CtInputViewGenlistItem *item = (CtInputViewGenlistItem*) elm_object_item_data_get(objectItem);
	if (item) {
		item->onUnrealized();
	}
}

void CtMyProfileInputView::__save()
{
	int err = CONTACTS_ERROR_NONE;
	int retId = 0;

	if (__isCreate) {
		err = __inputData->insertContact(&retId);

		if (CONTACTS_ERROR_NONE != err) {
			WERROR("insert failed %d", err);
		}
	}
	else {
		err = __inputData->updateContact();
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("update failed %d", err);
		}
	}
	__saved = true;

	if (__isCreate) {
		getNaviframe()->push(new CtMyProfileDetailView());
		destroy();
	} else {
		popOut();
	}

	int ret = preference_set_boolean(CT_KEY_MYPROFILE_CREATE_FIRST, false);
	WDEBUG("preference_set_boolean(%d) Failed", ret);
}

void CtMyProfileInputView::__saveCb(void *data, Evas_Object *obj, void *event_info)
{
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;

	view->__save();
}

void CtMyProfileInputView::__cancelCb(void *data, Evas_Object *obj, void *event_info)
{
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;
	elm_naviframe_item_pop(view->__getNaviframeEvasObj());
}

void CtMyProfileInputView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();
	//add title button
	__saveBtn = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(__saveBtn, "naviframe/title_right");
	elm_object_text_set(__saveBtn, V_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	evas_object_smart_callback_add(__saveBtn, "clicked", __saveCb, this);
	elm_object_item_part_content_set(naviItem, "title_right_btn", __saveBtn);

	if (__isCreate) {
		elm_object_disabled_set(__saveBtn, EINA_TRUE);
	}

	Evas_Object* cancelButton = elm_button_add(getNaviframe()->getEvasObj());
	elm_object_style_set(cancelButton, "naviframe/title_left");
	elm_object_text_set(cancelButton, V_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	evas_object_smart_callback_add(cancelButton, "clicked", __cancelCb, this);
	elm_object_item_part_content_set(naviItem, "title_left_btn", cancelButton);

	__addAllItems();

}

void CtMyProfileInputView::__galleryReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WHIT();
	char **pathArray = NULL;
	char *path = NULL;
	int arrayLength = 0;
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;
	int i=0;

	app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &pathArray, &arrayLength);
	if (arrayLength > 0 && pathArray[0]) {
		path = strdup(pathArray[0]);
	}
	for (i = 0; i < arrayLength; i++) {
		free(pathArray[i]);
	}
	free(pathArray);

	elm_object_tree_focus_allow_set(view->__getNaviframeEvasObj(), EINA_TRUE);

	if (path) {
		WDEBUG("image path %s", path);
		contacts_record_h record = view->__inputData->getImageRecord();
		//view->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_EX_OTHER);
		view->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_OTHER);
		view->__basicInfoUpdateImage();
		free(path);
	}
//	else {
//		elm_object_focus_set(cinput_d->image_focus_btn, EINA_TRUE);
//	}
}

void CtMyProfileInputView::__launchGallery(CtMyProfileInputView *view)
{
	WHIT();

	app_control_h service = NULL;
	int ret = 0;

	elm_object_tree_focus_allow_set(view->__getNaviframeEvasObj(), EINA_FALSE);

	ret = app_control_create(&service);
	WPWARN(APP_CONTROL_ERROR_NONE != ret, "app_control_create() Failed(%d)", ret);

	app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
	app_control_set_app_id(service, GALLERY_UG);
	app_control_add_extra_data(service, "hide-personal", "true");
	app_control_add_extra_data(service, "launch-type", "select-setas");
	app_control_add_extra_data(service, "setas-type", "callerid");
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_add_extra_data(service, "View Mode", "SETAS");
	app_control_add_extra_data(service, "Setas type", "CallerID");
	app_control_add_extra_data(service, "Area Size", "100");
	app_control_add_extra_data(service, "Resolution", "480x480");

	ret = app_control_send_launch_request(service, __galleryReplyCb, view);
	WPWARN(APP_CONTROL_ERROR_NONE != ret, "app_control_send_launch_request() failed(%d)", ret);
	app_control_destroy(service);
}

void CtMyProfileInputView::__cameraReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WHIT();
	char *path = NULL;
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;

	elm_object_tree_focus_allow_set(view->__getNaviframeEvasObj(), EINA_TRUE);

	app_control_get_extra_data(reply, APP_CONTROL_DATA_SELECTED, &path);
	if (path && *path) {
		contacts_record_h record = view->__inputData->getImageRecord();
		view->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_OTHER);
		view->__basicInfoUpdateImage();
	}
	free(path);
}

void CtMyProfileInputView::__launchCamera(CtMyProfileInputView *view)
{
	app_control_h service;
	app_control_create(&service);
	app_control_set_operation(service, APP_CONTROL_OPERATION_CREATE_CONTENT);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	app_control_set_mime(service, "image/jpg");
	app_control_add_extra_data(service, "CALLER", "contacts");

	int err = app_control_send_launch_request(service, __cameraReplyCb, view);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", err);
	app_control_destroy(service);
}


void CtMyProfileInputView::__onGallerySelCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;

	view->__launchGallery(view);

	view->destroyPopup();
}

void CtMyProfileInputView::__onCameraSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;

	view->__launchCamera(view);

	view->destroyPopup();
}

void CtMyProfileInputView::__onRemoveSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;
	view->destroyPopup();

	contacts_record_h record = NULL;
	record = view->__inputData->getImageRecord();
	view->__inputData->removeImage(record);
	view->__basicInfoUpdateImage();
}

void CtMyProfileInputView::__importantSubitemSelectedCb(void* data, Evas_Object* obj, void* event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtMyProfileInputView *view = (CtMyProfileInputView*)data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	CtInputViewImportantSubItem *importantSubItem = (CtInputViewImportantSubItem *)elm_object_item_data_get(item);
	contacts_record_h record = NULL;
	CtMyProfileInputItems checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
	Elm_Object_Item *addedItem = NULL;

	view->destroyPopup();

	elm_genlist_item_selected_set(item, EINA_FALSE);

	switch (importantSubItem->getIndex()) {
	case MYPROFILE_INPUT_IMPORTANT_PHONETIC_INFO:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddPhoneticName(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_PHONETIC_INFO);
		elm_genlist_item_fields_update(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO];

		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_MESSENGER:
	{
		record = view->__inputData->addMessenger("");
		view->__addedIndex = INPUT_ADDED_ITEM_INDEX_IM;

		if (view->__genlistItems[MYPROFILE_INPUT_MESSENGER]) {
			checkExistItem = MYPROFILE_INPUT_MESSENGER;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_URL]) {
			checkExistItem = MYPROFILE_INPUT_URL;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_EVENT]) {
			checkExistItem = MYPROFILE_INPUT_EVENT;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_ADDRESS]) {
			checkExistItem = MYPROFILE_INPUT_ADDRESS;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_NOTE]) {
			checkExistItem = MYPROFILE_INPUT_NOTE;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_EMAIL]) {
			checkExistItem = MYPROFILE_INPUT_EMAIL;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_NUMBER]) {
			checkExistItem = MYPROFILE_INPUT_NUMBER;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_MSISDN]) {
			checkExistItem = MYPROFILE_INPUT_MSISDN;
		}
		else {
			checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_MESSENGER item is after %d item", checkExistItem);

		CtInputViewMessengerItem* item = new CtInputViewMessengerItem( view->__inputData, record, NULL, view->__addedIndex,
			[view](){ view->__itemChanged(); },
			[view](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ view->__messengerItemDeleted( item, isChanged, record ); },
			[view](Evas_Object* obj, Elm_Object_Item* objItem){ view->__activateNextEntry(obj, objItem); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( view->__genlist, item->getItemClassStatic(), item, NULL, view->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		item->focus();
		view->__genlistItems[MYPROFILE_INPUT_MESSENGER] = genItem;
		addedItem = genItem;
		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_URL:
	{
		record = view->__inputData->addWebsite("");
		view->__addedIndex = INPUT_ADDED_ITEM_INDEX_URL;

		if (view->__genlistItems[MYPROFILE_INPUT_URL]) {
			checkExistItem = MYPROFILE_INPUT_URL;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_EVENT]) {
			checkExistItem = MYPROFILE_INPUT_EVENT;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_ADDRESS]) {
			checkExistItem = MYPROFILE_INPUT_ADDRESS;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_NOTE]) {
			checkExistItem = MYPROFILE_INPUT_NOTE;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_EMAIL]) {
			checkExistItem = MYPROFILE_INPUT_EMAIL;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_NUMBER]) {
			checkExistItem = MYPROFILE_INPUT_NUMBER;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_MSISDN]) {
			checkExistItem = MYPROFILE_INPUT_MSISDN;
		}
		else {
			checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_URL item is after %d item", checkExistItem);

		CtInputViewWebsiteItem* item = new CtInputViewWebsiteItem( view->__inputData, record, NULL, view->__addedIndex,
			[view](){ view->__itemChanged(); },
			[view](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ view->__websiteItemDeleted( item, isChanged, record );},
			[view](Evas_Object* obj, Elm_Object_Item* objItem){ view->__activateNextEntry(obj, objItem); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( view->__genlist, item->getItemClassStatic(), item, NULL, view->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		item->focus();
		view->__genlistItems[MYPROFILE_INPUT_URL] = genItem;
		addedItem = genItem;
		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_ORGANIZATION:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddOrganization(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_ORGANIZATION);
		elm_genlist_item_fields_update(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO];

		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_NICKNAME:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddNickname(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_NICKNAME);
		elm_genlist_item_fields_update(view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = view->__genlistItems[MYPROFILE_INPUT_BASIC_INFO];

		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_EVENT:
	{
		view->__createEventDataPopup();
		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_ADDRESS:
	{
		record = view->__inputData->addAddress("");
		view->__addedIndex = INPUT_ADDED_ITEM_INDEX_ADDRESS;

		if (view->__genlistItems[MYPROFILE_INPUT_ADDRESS]) {
			checkExistItem = MYPROFILE_INPUT_ADDRESS;
		}
		if (view->__genlistItems[MYPROFILE_INPUT_NOTE]) {
			checkExistItem = MYPROFILE_INPUT_NOTE;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_EMAIL]) {
			checkExistItem = MYPROFILE_INPUT_EMAIL;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_NUMBER]) {
			checkExistItem = MYPROFILE_INPUT_NUMBER;
		}
		else if (view->__genlistItems[MYPROFILE_INPUT_MSISDN]) {
			checkExistItem = MYPROFILE_INPUT_MSISDN;
		}
		else {
			checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_ADDRESS item is after %d item", checkExistItem);

		CtInputViewAddressItem* item = new CtInputViewAddressItem( view->__inputData, record, NULL, view->__addedIndex,
			[view](){ view->__itemChanged(); },
			[view](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ view->__addressItemDeleted( item, isChanged, record ); },
			[view](Evas_Object* obj, Elm_Object_Item* objItem){ view->__activateNextEntry(obj, objItem); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( view->__genlist, item->getItemClassStatic(), item, NULL, view->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		item->focus();
		view->__genlistItems[MYPROFILE_INPUT_ADDRESS] = genItem;
		addedItem = genItem;
		break;
	}
	default:
		break;
	}
	if (addedItem) {
		elm_genlist_item_select_mode_set(addedItem, ELM_OBJECT_SELECT_MODE_NONE);
		elm_genlist_item_show(addedItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

void CtMyProfileInputView::__addNumberClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;

	record = __inputData->addNumber("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NUMBER;

    	CtInputViewNumberItem* item = new CtInputViewNumberItem( NULL, __inputData, record, NULL, __addedIndex,
			[this](){ __itemChanged(); },
			[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
			[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

	Elm_Object_Item* genItem = NULL;
	if (__genlistItems[MYPROFILE_INPUT_NUMBER]) {
		genItem = elm_genlist_item_insert_after(__genlist, item->getItemClassStatic(), item, NULL, __genlistItems[MYPROFILE_INPUT_NUMBER], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	else if (__genlistItems[MYPROFILE_INPUT_MSISDN]) {
		genItem = elm_genlist_item_insert_after(__genlist, item->getItemClassStatic(), item, NULL, __genlistItems[MYPROFILE_INPUT_MSISDN], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	else {
		genItem = elm_genlist_item_insert_after(__genlist, item->getItemClassStatic(), item, NULL, __genlistItems[MYPROFILE_INPUT_BASIC_INFO], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	item->setElmObjectItem( genItem );
	item->focus();
	__genlistItems[MYPROFILE_INPUT_NUMBER] = genItem;
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void CtMyProfileInputView::__addEmailClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;
	CtMyProfileInputItems checkExistItem = MYPROFILE_INPUT_BASIC_INFO;

	record = __inputData->addEmail("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_EMAIL;

	if (__genlistItems[MYPROFILE_INPUT_EMAIL]) {
		checkExistItem = MYPROFILE_INPUT_EMAIL;
	}
	else if (__genlistItems[MYPROFILE_INPUT_NUMBER]) {
		checkExistItem = MYPROFILE_INPUT_NUMBER;
	}
	else if (__genlistItems[MYPROFILE_INPUT_MSISDN]) {
		checkExistItem = MYPROFILE_INPUT_MSISDN;
	}
	else {
		checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
	}
	WDEBUG("INPUT_EMAIL item is after %d item", checkExistItem);

	CtInputViewEmailItem *item = new CtInputViewEmailItem( __inputData, record, NULL, __addedIndex,
			[this](){ __itemChanged(); },
			[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __emailItemDeleted( item, isChanged, record ); },
			[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
	Elm_Object_Item* genItem = elm_genlist_item_insert_after( __genlist, item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	item->focus();
	__genlistItems[MYPROFILE_INPUT_EMAIL] = genItem;
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void CtMyProfileInputView::__addNoteClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;
	CtMyProfileInputItems checkExistItem = MYPROFILE_INPUT_BASIC_INFO;

	WPRET_M(__genlistItems[MYPROFILE_INPUT_NOTE], "note already is added");

	record = __inputData->addNote("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NOTE;

	if (__genlistItems[MYPROFILE_INPUT_EMAIL]) {
		checkExistItem = MYPROFILE_INPUT_EMAIL;
	}
	else if (__genlistItems[MYPROFILE_INPUT_NUMBER]) {
		checkExistItem = MYPROFILE_INPUT_NUMBER;
	}
	else if (__genlistItems[MYPROFILE_INPUT_MSISDN]) {
		checkExistItem = MYPROFILE_INPUT_MSISDN;
	}
	else {
		checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
	}
	WDEBUG("INPUT_NOTE item is after %d item", checkExistItem);

	CtInputViewNoteItem *item = new CtInputViewNoteItem( __inputData, record, NULL, __addedIndex,
			[this](){ __itemChanged(); },
			[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __noteItemDeleted( item, isChanged, record ); },
			[this](Evas_Object* obj, Elm_Object_Item* objItem){this->__activateNextEntry(obj, objItem);});
	Elm_Object_Item* genItem = elm_genlist_item_insert_after( __genlist, item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	item->focus();
	__genlistItems[MYPROFILE_INPUT_NOTE] = genItem;
	__updateNoteBtn(true);
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void CtMyProfileInputView::__addMoreClickedCb()
{
	WHIT();

	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle( "IDS_PB_BUTTON_MORE");
	popup->setContent([this](Evas_Object* parent)->Evas_Object*
	{
		Elm_Object_Item *genItem = NULL;

		Evas_Object* genlist = elm_genlist_add(parent);
		elm_object_style_set( genlist, "popup" );
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated
		elm_object_content_set(parent, genlist);

		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
		WPRET_VM(NULL == basicInfoItem, NULL, "basicInfoItem is NULL");

		for (int i = MYPROFILE_INPUT_IMPORTANT_ADDRESS; i < MYPROFILE_INPUT_IMPORTANT_MAX; i++) {
			WDEBUG("item %d", i);
			if (i == MYPROFILE_INPUT_IMPORTANT_PHONETIC_INFO) {
				if (basicInfoItem->getAddPhoneticName()) {
					continue;
				}
			} else if (i == MYPROFILE_INPUT_IMPORTANT_NICKNAME) {
				if (basicInfoItem->getAddNickname()) {
					continue;
				}
			} else if (i == MYPROFILE_INPUT_IMPORTANT_ORGANIZATION) {
				if (basicInfoItem->getAddOrganization()) {
					continue;
				}
			}
			CtInputViewImportantSubItem *item = new CtInputViewImportantSubItem(i, true);
			genItem = elm_genlist_item_append( genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, __importantSubitemSelectedCb, this);
			item->setElmObjectItem( genItem );
		}

		evas_object_show(genlist);

		return genlist;

	});

	attachPopup( popup );
}


bool CtMyProfileInputView::__checkIsEmpty()
{
	Elm_Object_Item *item = __genlistItems[MYPROFILE_INPUT_BASIC_INFO];
	bool isEmpty = true;

	while (item) {
		if (item == __genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(item);
		if (viewItem) {
			WDEBUG("viewItem->getIsEmpty() = %d", viewItem->getIsEmpty());
			if (false == viewItem->getIsEmpty()) {
				isEmpty = false;
				break;
			}
		}
		item = elm_genlist_item_next_get(item);
	}
	return isEmpty;
}

bool CtMyProfileInputView::__checkIsChanged()
{
	Elm_Object_Item *item = __genlistItems[MYPROFILE_INPUT_BASIC_INFO];
	bool changed = false;

	while (item) {
		if (item == __genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(item);
		if (viewItem) {
			WDEBUG("viewItem->getIsChanged() = %d", viewItem->getIsChanged());
			if (true == viewItem->getIsChanged()) {
				changed = true;
				break;
			}
		}
		item = elm_genlist_item_next_get(item);
	}
	return changed;
}

void CtMyProfileInputView::__updateNoteBtn(bool isNoteInput)
{
	WHIT();
	if (__genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]) {
		CtInputViewImportantFieldsItem *item = (CtInputViewImportantFieldsItem*)elm_object_item_data_get(__genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]);
		if(item) {
			item->updateNoteBtn(isNoteInput);
		}
	}
}

void CtMyProfileInputView::__updateSaveBtn()
{
	Eina_Bool empty = __checkIsEmpty();

	elm_object_disabled_set(__saveBtn, empty);
	WPRET_M(true == empty, "all fields are empty, can't save");

	WPRET_M(false == __isCreate, "don't need to update save button");

	Eina_Bool changed = __checkIsChanged();

	elm_object_disabled_set(__saveBtn, !changed);
}

void CtMyProfileInputView::__addItem(int itemType)
{
	WDEBUG("itemType %d", itemType);
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;
	void *data = NULL;
	contacts_record_h record = NULL;

	switch(itemType) {
	case MYPROFILE_INPUT_BASIC_INFO:
	{
		CtInputViewBasicInfoItem* item = new CtInputViewBasicInfoItem(__inputData, &__nameEntry,
				[this](){ __itemChanged(); },
				[this](CtInputViewBasicInfoItem::ITEM_DELETED deletedInfo){ __basicInfoItemDeleted( deletedInfo); },
				[this](Evas_Object* obj){ __basicInfoItemImageClicked(obj); },
				[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); },
				[this](Evas_Object* obj){ __basicInfoItemShowCreateKeypad(obj); },
				[this](Elm_Object_Item* objItem){ __updateEntryReturnKeyType(objItem); });
		Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
		item->setElmObjectItem( genItem );
		__genlistItems[MYPROFILE_INPUT_BASIC_INFO] = genItem;
		__updateBasicInfoItem();
		break;
	}
	case MYPROFILE_INPUT_MSISDN:
	{
		contacts_record_h defaultRecord = __inputData->getDefaultNumber();
		if (defaultRecord) {
			CtInputViewMsisdnItem *item = new CtInputViewMsisdnItem( __inputData, defaultRecord);

			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_MSISDN] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_NUMBER:
	{
		list = __inputData->getNumberList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			WDEBUG("added");

			const char *number = __inputData->getNumberNumber(record);

			CtInputViewNumberItem *item = new CtInputViewNumberItem( NULL, __inputData, record, number, INPUT_ADDED_ITEM_INDEX_NONE,
				[this](){ __itemChanged(); },
				[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
				[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_NUMBER] = genItem;
		}

		if (__isCreate && list == NULL) {
			record = __inputData->addNumber("");

			CtInputViewNumberItem *item = new CtInputViewNumberItem( NULL, __inputData, record, NULL, INPUT_ADDED_ITEM_INDEX_NONE,
				[this](){ __itemChanged(); },
				[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
				[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

			Elm_Object_Item* genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_NUMBER] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_EMAIL:
	{
		list = __inputData->getEmailList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			const char *email = __inputData->getEmailEmail(record);

			CtInputViewEmailItem* item = new CtInputViewEmailItem( __inputData, record, email, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __emailItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_EMAIL] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_ADDRESS:
	{
		list = __inputData->getAddressList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			char address[CT_TEXT_MAX_LEN] = {0,};
			__inputData->getAddressAddress( record, address, sizeof(address));

			CtInputViewAddressItem* item = new CtInputViewAddressItem( __inputData, record, address, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __addressItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_ADDRESS] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_MESSENGER:
	{
		list = __inputData->getMessengerList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			const char *messenger = __inputData->getMessengerMessenger(record);

			CtInputViewMessengerItem* item = new CtInputViewMessengerItem( __inputData, record, messenger, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __messengerItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_MESSENGER] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_URL:
	{
		list = __inputData->getWebsiteList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			const char *website = __inputData->getWebsiteWebsite(record);

			CtInputViewWebsiteItem* item = new CtInputViewWebsiteItem( __inputData, record, website, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __websiteItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_URL] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_EVENT:
	{
		list = __inputData->getEventList();

		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}

			char event[256] = {0};
			snprintf(event, sizeof(event), "%d", __inputData->getEventEvent(record));

			if( __inputData->getEventType(record) == CONTACTS_EVENT_TYPE_BIRTH) {
				*__birthdayEventExist = true;
			}

			CtInputViewEventItem *item = new CtInputViewEventItem( this, __inputData, record, event, __birthdayEventExist,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __eventItemDeleted( objItem, isChanged, record ); });
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_EVENT] = genItem;
		}
		break;
	}
	case MYPROFILE_INPUT_NOTE:
	{
		list = __inputData->getNoteList();
		EINA_LIST_FOREACH(list, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			if(record == NULL) {
				continue;
			}
			const char *note = __inputData->getNote(record);

			CtInputViewNoteItem *item = new CtInputViewNoteItem( __inputData, record, note, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __noteItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){this->__activateNextEntry(obj, objItem);});
			Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[MYPROFILE_INPUT_NOTE] = genItem;
			__updateNoteBtn(true);
		}
		break;
	}
	case MYPROFILE_INPUT_IMPORTANT_FIELDS:
	{
		bool isNoteInput = (__genlistItems[MYPROFILE_INPUT_NOTE] == NULL) ? false : true;
		CtInputViewImportantFieldsItem *item = new CtInputViewImportantFieldsItem([this](){ __addNumberClickedCb(); },
				[this](){ __addEmailClickedCb(); },
				[this](){ __addNoteClickedCb(); },
				[this](){ __addMoreClickedCb(); },
				isNoteInput);
		Elm_Object_Item *genItem = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
		item->setElmObjectItem( genItem );
		__genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS] = genItem;
		break;
	}
	}
}

void CtMyProfileInputView::__addAllItems()
{
	WHIT();
	int i = 0;

	for (; i < MYPROFILE_INPUT_ITEMS_MAX; i++) {
		__addItem(i);
	}
}

void CtMyProfileInputView::onDestroy()
{
	WHIT();
	evas_object_smart_callback_del(__genlist, "unrealized", __genlistUnrealizedCb);
	evas_object_smart_callback_del(__genlist, "realized", __genlistRealizedCb);
}

void CtMyProfileInputView::onEvent(int eventType)
{
	WHIT();
}

bool CtMyProfileInputView::onPop()
{
	if(!__saved) {
		WDEBUG("canceled");
		if (__discard) {
			return true;
		}

		Eina_Bool changed = __checkIsChanged();

		if (!__isCreate) { // edit view
			if (__changed) {
				changed = EINA_TRUE;
			}
		}

		if (changed) {
			WDEBUG("changed");

			WPopup *popup = new WPopup();
			popup->setTextTranslatable(PACKAGE);
			popup->setTitle("IDS_PB_HEADER_DISCARD_CHANGES_ABB");
			popup->setContent("IDS_PB_POP_ALL_CHANGES_WILL_BE_DISCARDED");

			popup->addButton("IDS_PB_BUTTON_CANCEL", NULL );
			popup->addButton("IDS_PB_BUTTON_DISCARD_ABB", [this](bool* destroyPopup){
				WDEBUG("Discard");
				this->__discard = true;
				popOut();
				*destroyPopup = false;
			});

			attachPopup(popup);
			return false;
		} else {
			WDEBUG("not changed");
			return true;
		}
	}

	return true;
}

void CtMyProfileInputView::__itemChanged()
{
	__updateSaveBtn();
}

void CtMyProfileInputView::__basicInfoItemDeleted( int deletedInfo)
{
	CtInputViewBasicInfoItem::ITEM_DELETED info = (CtInputViewBasicInfoItem::ITEM_DELETED)deletedInfo;

	CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
	WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

	switch( info )
	{
	case CtInputViewBasicInfoItem::PHONETIC_NAME_DELETED:
		if( basicInfoItem->getPhoneticNameIsChanged()
			|| __inputData->getPhoneticFirstName()
			|| __inputData->getPhoneticMiddleName()
			|| __inputData->getPhoneticLastName()) {
			__changed = true;
		}

		basicInfoItem->setPhoneticNameIsChanged(false);
		basicInfoItem->setAddPhoneticName(false);
		__inputData->setPhoneticFullName(NULL);
		break;
	case CtInputViewBasicInfoItem::NICKNAME_DELETED:
		if( basicInfoItem->getNicknameIsChanged() || __inputData->getNickname())
			__changed = true;

		basicInfoItem->setNicknameIsChanged(false);
		basicInfoItem->setAddNickname(false);
		__inputData->setNickname(NULL);
		break;
	case CtInputViewBasicInfoItem::ORGANIZATION_DELETED:
		if( (basicInfoItem->getCompanyIsChanged() || __inputData->getCompany())
			|| (basicInfoItem->getJobTitleIsChanged() || __inputData->getJobTitle()))
		{
			__changed = true;
		}

		basicInfoItem->setCompanyIsChanged(false);
		basicInfoItem->setJobTitleIsChanged(false);
		basicInfoItem->setAddOrganization(false);
		__inputData->setCompany(NULL);
		__inputData->setJobTitle(NULL);
		break;
	default:
		WERROR("Not handled case =%d", deletedInfo);
		break;
	}

	__updateSaveBtn();
	__updateEntryReturnKeyType(__genlistItems[MYPROFILE_INPUT_BASIC_INFO]);
}

void CtMyProfileInputView::__basicInfoItemImageClicked( Evas_Object* obj )
{
	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_PB_HEADER_SET_CONTACT_PICTURE_ABB");
	popup->setContent([this](Evas_Object* parent)->Evas_Object*
	{
		Elm_Genlist_Item_Class* itc1 = elm_genlist_item_class_new();
		Elm_Genlist_Item_Class* itc2 = elm_genlist_item_class_new();
		Elm_Genlist_Item_Class* itc3 = elm_genlist_item_class_new();

		if (itc1) {
			itc1->item_style = "type1";
			itc1->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
			{
				if(!strcmp(part, "elm.text")) {
					return SAFE_STRDUP(V_("IDS_PB_OPT_FROM_GALLERY_ABB2"));
				}
				return NULL;
			};
		}

		if (itc2) {
			itc2->item_style = "type1";
			itc2->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
			{
				if(!strcmp(part, "elm.text")) {
					return SAFE_STRDUP(V_("IDS_PB_OPT_TAKE_PICTURE_ABB"));
				}
				return NULL;
			};
		}

		if (itc3) {
			itc3->item_style = "type1";
			itc3->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
			{
				if(!strcmp(part, "elm.text")) {
					CtMyProfileInputView* view = (CtMyProfileInputView*)data;
					if(view->__inputData->getImagePath())
						return SAFE_STRDUP(V_("IDS_PB_OPT_REMOVE"));
					else
						return NULL;

				}
				return NULL;
			};
		}

		int count = 2;

		Evas_Object* genlist = elm_genlist_add(parent);
		elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);

		elm_genlist_item_append(genlist, itc1, this, NULL, ELM_GENLIST_ITEM_NONE, __onGallerySelCb, this);
		elm_genlist_item_append(genlist, itc2, this, NULL, ELM_GENLIST_ITEM_NONE, __onCameraSelCb, this);

		if(__inputData->getImagePath()) {
			elm_genlist_item_append(genlist, itc3, this, NULL, ELM_GENLIST_ITEM_NONE, __onRemoveSelCb, this);
			count++;
		}

		elm_genlist_item_class_free(itc1);
		elm_genlist_item_class_free(itc2);
		elm_genlist_item_class_free(itc3);

		return genlist;
	});

	attachPopup( popup );
}

void CtMyProfileInputView::__basicInfoItemShowCreateKeypad( Evas_Object* obj)
{
	WHIT();
	if (__inputData->getContactId() == 0) {
		elm_object_focus_set(__nameEntry, EINA_TRUE);
		elm_entry_cursor_end_set(__nameEntry);
	}
}

void CtMyProfileInputView::__basicInfoUpdateImage()
{
	WHIT();
	elm_genlist_item_fields_update(__genlistItems[MYPROFILE_INPUT_BASIC_INFO], "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
}

Elm_Object_Item* CtMyProfileInputView::__getLastItem(Elm_Object_Item* deleteItem)
{
	Elm_Object_Item* item = NULL;
	Elm_Object_Item* lastItem = NULL;
	Elm_Object_Item* retItem = NULL;
	int count = 0;

	item = elm_genlist_first_item_get( __genlist );
	while (item) {
		if( elm_genlist_item_item_class_get( deleteItem ) == elm_genlist_item_item_class_get(item)) {
			lastItem = item;
			count++;
		}
		item = elm_genlist_item_next_get(item);
	}

	if( lastItem == deleteItem ) {
		if (1 == count) {
			retItem = NULL;
		}
		else { // must > 1
			retItem = elm_genlist_item_prev_get(lastItem);
		}
	}
	else {
		retItem = lastItem;
	}

	return retItem;
}

void CtMyProfileInputView::__numberItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_NUMBER] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getNumberNumber(record)) {
		__changed = true;
	}

	__inputData->removeNumber(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__emailItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_EMAIL] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getEmailEmail( record ) ) {
		__changed = true;
	}
	__inputData->removeEmail( record );
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__addressItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_ADDRESS] = __getLastItem(deleteItem);

	char address[CT_TEXT_MAX_LEN] = {0,};
	__inputData->getAddressAddress( record, address, sizeof(address));
	if( isChanged || address[0] != '\0') {
		__changed = true;
	}

	__inputData->removeAddress(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__messengerItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_MESSENGER] = __getLastItem(deleteItem);

	if ( isChanged || __inputData->getMessengerMessenger( record)) {
		__changed = true;
	}

	__inputData->removeMessenger(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__websiteItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_URL] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getWebsiteWebsite( record )) {
		__changed = true;
	}

	__inputData->removeWebsite(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__eventItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[MYPROFILE_INPUT_EVENT] = __getLastItem(deleteItem);

	if (isChanged || __inputData->getEventEvent(record)) {
		__changed = true;
	}

	__inputData->removeEvent(record);
	__updateSaveBtn();
}

void CtMyProfileInputView::__noteItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	Evas_Object *noteBtn = elm_object_item_part_content_get(__genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS], "elm.icon.3");
	elm_object_disabled_set(noteBtn, EINA_FALSE);

	if( isChanged || __inputData->getNote(record)) {
		__changed = true;
	}

	__inputData->removeNote( record );
	__genlistItems[MYPROFILE_INPUT_NOTE] = NULL;
	__updateNoteBtn(false);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtMyProfileInputView::__activateNextEntry( Evas_Object* obj, Elm_Object_Item* item )
{
	WHIT();
	WPRET_M(NULL == obj, "obj is NULL");
	WPRET_M(NULL == item, "item is NULL");
	Elm_Object_Item* nextItem = elm_genlist_item_next_get(item);
	bool focused = false;
	Evas_Object* entry = obj;
	while (nextItem) {
		if (nextItem == __genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(nextItem);
		if (viewItem) {
			entry = viewItem->getEntry();
			if (entry) {
				elm_object_focus_set(entry, EINA_TRUE);
				focused = true;
				break;
			}
		}
		nextItem = elm_genlist_item_next_get(nextItem);
	}
	// don't have next entry, close IME
	elm_object_focus_set(entry, focused);
}

void CtMyProfileInputView::__updateEntryReturnKeyType(Elm_Object_Item* deleteItem)
{
	WHIT();
	Elm_Object_Item *item = __genlistItems[MYPROFILE_INPUT_BASIC_INFO];
	Evas_Object* entry = NULL;
	Evas_Object* box = NULL;
	Eina_List* list = NULL;
	Eina_List* cursor = NULL;
	void *tmp = NULL;
	Evas_Object* layout = NULL;

	while (item) {
		if (item == __genlistItems[MYPROFILE_INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		if (item == deleteItem) {
			item = elm_genlist_item_next_get(item);
			continue;
		}

		if (item == __genlistItems[MYPROFILE_INPUT_BASIC_INFO]) {
			box = elm_object_item_part_content_get(item, "elm.icon.1");
			if (box) {
				list = elm_box_children_get(box);
				EINA_LIST_FOREACH(list, cursor, tmp) {
					layout = (Evas_Object*)tmp;
					if (layout) {
						entry = elm_object_part_content_get(layout, "elm.icon.1");
						if (entry && elm_entry_single_line_get(entry)) {
							elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
						}
					}
				}
			}
		}
		else {
			CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(item);
			if (viewItem) {
				entry = viewItem->getEntry();

				if (entry && elm_entry_single_line_get(entry)) {
					elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
				}
			}
		}
		item = elm_genlist_item_next_get(item);
	}

	if (entry && elm_entry_single_line_get(entry)) {
		elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	}
}

void CtMyProfileInputView::__updateBasicInfoItem()
{
	WHIT();
	if(__genlistItems[MYPROFILE_INPUT_BASIC_INFO]) {
		elm_object_item_signal_emit(__genlistItems[MYPROFILE_INPUT_BASIC_INFO], "image,enable", "");
	}
}

void CtMyProfileInputView::__createEventDataPopup()
{
	WHIT();
	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_ST_BODY_SET_DATE");
	popup->addButton("IDS_PB_BUTTON_CANCEL", NULL);
	popup->addButton( "IDS_PB_SK_SET", [popup, this](bool* destroyPopup) {
		struct tm time1 = {};
		time_t t;
		t = time(NULL);
		localtime_r(&t, &time1);

		int year = 0;
		int month = 0;
		int day = 0;
		Evas_Object *datetimePickerBg = elm_object_content_get(popup->getEvasObj());
		Evas_Object *dateTime = elm_object_part_content_get(datetimePickerBg, "elm.swallow.content");
		elm_datetime_value_get(dateTime, &time1);
		year = time1.tm_year + 1900;
		month = time1.tm_mon + 1;
		day = time1.tm_mday;

		__appendEventItem(year, month, day);

		*destroyPopup = true;
	});
	popup->setOnMoreProperties([this](Evas_Object* popupObj ) {
		struct tm time1 = {};
		time_t t;
		t = time(NULL);
		localtime_r(&t, &time1);

		Evas_Object* dateTime = elm_datetime_add(popupObj);
		elm_datetime_format_set(dateTime, DATE_TIME_FORMAT);
		elm_datetime_value_set(dateTime, &time1);

		std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
		Evas_Object* datetimePickerBg = elm_layout_add(popupObj);
		evas_object_size_hint_weight_set(datetimePickerBg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		CtThemeMgr::getInstance()->setObjectTheme(datetimePickerBg);
		elm_layout_file_set(datetimePickerBg, contactsEdjePath.c_str(), "datetimePickerBg");
		elm_object_part_content_set(datetimePickerBg, "elm.swallow.content", dateTime);
		evas_object_show(datetimePickerBg);

		elm_object_content_set(popupObj, datetimePickerBg);
	});

	attachPopup( popup );
}

void CtMyProfileInputView::__appendEventItem(int year, int month, int day)
{
	WHIT();
	contacts_record_h record = NULL;
	CtMyProfileInputItems checkExistItem = MYPROFILE_INPUT_BASIC_INFO;

	record = __inputData->addEvent(year * 10000 + month * 100 + day);

	if (__genlistItems[MYPROFILE_INPUT_EVENT]) {
		checkExistItem = MYPROFILE_INPUT_EVENT;
	}
	else if (__genlistItems[MYPROFILE_INPUT_ADDRESS]) {
		checkExistItem = MYPROFILE_INPUT_ADDRESS;
	}
	else if (__genlistItems[MYPROFILE_INPUT_NOTE]) {
		checkExistItem = MYPROFILE_INPUT_NOTE;
	}
	else if (__genlistItems[MYPROFILE_INPUT_EMAIL]) {
		checkExistItem = MYPROFILE_INPUT_EMAIL;
	}
	else if (__genlistItems[MYPROFILE_INPUT_NUMBER]) {
		checkExistItem = MYPROFILE_INPUT_NUMBER;
	}
	else if (__genlistItems[MYPROFILE_INPUT_MSISDN]) {
		checkExistItem = MYPROFILE_INPUT_MSISDN;
	}
	else {
		checkExistItem = MYPROFILE_INPUT_BASIC_INFO;
	}
	WDEBUG("INPUT_EVENT item is after %d item", checkExistItem);

	if(*__birthdayEventExist ) {
		__inputData->setEventType(record, CONTACTS_EVENT_TYPE_ANNIVERSARY, NULL);
	}
	else {
		if(__inputData->getEventType(record) == CONTACTS_EVENT_TYPE_BIRTH) {
			*__birthdayEventExist = true;
		}
	}

	CtInputViewEventItem *item = new CtInputViewEventItem(this, __inputData, record, NULL, __birthdayEventExist,
		[this](){ __itemChanged(); },
		[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __eventItemDeleted( item, isChanged, record ); } );
	Elm_Object_Item *genItem = elm_genlist_item_insert_after( __genlist, item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	__genlistItems[MYPROFILE_INPUT_EVENT] = genItem;
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

extern "C" {
	WAPP_ASSIST_EXPORT WView* createMyProfileInputView(contacts_record_h contactRecord) {
		CtMyProfileInputView* view = new CtMyProfileInputView(contactRecord);
		return view;
	}
}
