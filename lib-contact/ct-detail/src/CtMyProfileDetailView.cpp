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

#include <app.h>
#include <contacts.h>
#include <app_preference.h>

#include "WNaviframe.h"
#include "WWindow.h"
#include "WMenuPopup.h"

#include "ContactsDebug.h"
#include "CtPath.h"
#include "CtCommon.h"
#include "ContactsCommon.h"

#include "CtMyProfileDetailView.h"
#include "CtDetailViewBasicInfoControl.h"
#include "CtDetailViewNumberItem.h"
#include "CtDetailViewEmailItem.h"
#include "CtDetailViewNoteItem.h"
#include "CtDetailViewAddressItem.h"
#include "CtDetailViewEventItem.h"
#include "CtDetailViewMessengerItem.h"
#include "CtDetailViewRelationItem.h"
#include "CtDetailViewWebsiteItem.h"
#include "CtMyProfileDetailData.h"
#include "CtMyProfileInputView.h"
#include "CtThemeMgr.h"

#define __ITEM_HEIGHT 96


CtMyProfileDetailView::CtMyProfileDetailView()
{
	__detailData = new CtMyProfileDetailData();
	__initialize();
}

CtMyProfileDetailView::~CtMyProfileDetailView()
{
	WHIT();
	//ctCommonTerminateAppservice(getNaviframe()->getEvasObj());
	destroyPopup();
	delete __detailData;
}

void CtMyProfileDetailView::__initialize()
{
	for (int i = MYPROFILE_DETAIL_MSISDN; i < MYPROFILE_DETAIL_ITEMS_MAX; i++) {
		__genlistItems[i] = {};
	}
}

const char* CtMyProfileDetailView::getClassName()
{
	return "CtMyProfileDetailView";
}

void CtMyProfileDetailView::onDbChanged(const char* viewUri)
{
	WENTER();
	__createBaseInfo();
	__updateGenlist();
}

void CtMyProfileDetailView::onCompleteGetContacts(int offset)
{
	WENTER();
}

void CtMyProfileDetailView::onConditionChagned()
{
	WENTER();
}

Evas_Object* CtMyProfileDetailView::__getWindow(void)
{
	if(getWindow())
		return getWindow()->getEvasObj();
	else
		WASSERT_EX(0, "window null");

	return NULL;
}

void CtMyProfileDetailView::onDestroy()
{
	WENTER();

}

void CtMyProfileDetailView::onEvent(int eventType)
{
	WENTER();

}

void CtMyProfileDetailView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();
	elm_naviframe_item_title_enabled_set(naviItem, EINA_FALSE, EINA_FALSE);

	enableMoreButton(naviItem, [](void *data, Evas_Object *obj, void *event_info){
		CtMyProfileDetailView* view = static_cast<CtMyProfileDetailView*>(data);
		WMenuPopup* popup = new WMenuPopup();
		popup->prepare(view->__getWindow(), view->getNaviframe()->getEvasObj() );

		popup->appendItem(V_("IDS_PB_OPT_EDIT"), [view]()
				{
					view->getNaviframe()->push(new CtMyProfileInputView(NULL), NULL, NULL);
				});
		popup->appendItem(V_("IDS_PB_BUTTON_DELETE_ABB4"), [view]()
				{
					view->__onDeleteCb(view, NULL, NULL);
				});
		popup->appendItem(V_("IDS_PB_HEADER_SHARE_VIA"), [view]()
				{
					view->__onShareCb(view, NULL, NULL);
				});
		view->attachPopup( popup );

	}, this);
	__detailData->attachListener(this);
}

void CtMyProfileDetailView::__basicInfoBackBtnCb(void *data, Evas_Object *obj,
		const char *emission, const char *source)
{
	WHIT();
	WPRET_M(!data, "data is NULL");
	CtMyProfileDetailView* view = (CtMyProfileDetailView*)data;

	view->popOut();
}

void CtMyProfileDetailView::__createBaseInfo()
{
	WHIT();
	CtDetailViewBasicInfoControl *basicInfo = new CtDetailViewBasicInfoControl(__detailData, false);
	WPRET_M(!basicInfo, "basicInfo is NULL");
	basicInfo->create(__mainLayout, NULL);
	edje_object_signal_callback_add(_EDJ(basicInfo->getEvasObj()), "mouse,clicked,*", "back_btn", __basicInfoBackBtnCb, this);

	elm_object_part_content_set(__mainLayout, "swallow.base_info", basicInfo->getEvasObj());
}

Evas_Object* CtMyProfileDetailView::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	std::string detailLayoutEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);
	CtThemeMgr::getInstance()->addExtensionTheme(detailLayoutEdjePath.c_str());

	__mainLayout = elm_layout_add(parent);
	elm_layout_file_set(__mainLayout, detailLayoutEdjePath.c_str(), "main_layout");

	CtThemeMgr::getInstance()->setObjectTheme(__mainLayout);

	__genlist = elm_genlist_add(__mainLayout);
	evas_object_size_hint_weight_set(__genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_genlist_mode_set(__genlist, ELM_LIST_COMPRESS);
	elm_object_part_content_set(__mainLayout, "swallow.content", __genlist);

	__createBaseInfo();
	__addAllItems();

	return __mainLayout;

}

void CtMyProfileDetailView::onLanguageChanged()
{
	WHIT();

	if(__genlist)
		elm_genlist_realized_items_update(__genlist);
}

int CtMyProfileDetailView::getPersonId(void)
{
	WASSERT_EX(NULL != __detailData, "__detailData is null");
	return __detailData->getPersonId();
}


void CtMyProfileDetailView::__addAllItems()
{
	WHIT();
	for (int i = 0; i < MYPROFILE_DETAIL_ITEMS_MAX; i++) {
		__addItem(i);
	}
}

void CtMyProfileDetailView::__addItem(int itemType)
{
	WHIT();

	WDEBUG("itemType %d", itemType);
	switch(itemType) {
	case MYPROFILE_DETAIL_MSISDN:
		__addMsisdnItem();
		break;
	case MYPROFILE_DETAIL_NUMBER:
		__addNumberItems();
		break;
	case MYPROFILE_DETAIL_EMAIL:
		__addEmailItems();
		break;
	case MYPROFILE_DETAIL_NOTE:
		__addNoteItems();
		break;
	case MYPROFILE_DETAIL_ADDRESS:
		__addAddressItems();
		break;
	case MYPROFILE_DETAIL_EVENT:
		__addEventItems();
		break;
	case MYPROFILE_DETAIL_RELATIONSHIP:
		__addRelationItems();
		break;
	case MYPROFILE_DETAIL_WEBSITE:
		__addWebsiteItems();
		break;
	case MYPROFILE_DETAIL_MESSENGER:
		__addMessengerItems();
		break;
        default:
            WDEBUG("Unsupported type: %d", itemType);
            break;
	}
}

void CtMyProfileDetailView::__addMsisdnItem()
{
	WHIT();

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	contacts_record_h defaultRecord = __detailData->getDefNumberRecord();
	if (defaultRecord) {
		CtDetailViewNumberItem *item = new CtDetailViewNumberItem(__detailData, defaultRecord, true);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addNumberItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getNumberList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewNumberItem *item = new CtDetailViewNumberItem(__detailData, record, true);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addEmailItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getEmailList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;
		CtDetailViewEmailItem* item = new CtDetailViewEmailItem(__detailData, record, true);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addNoteItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getNoteList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewNoteItem* item = new CtDetailViewNoteItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addAddressItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getAddressList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewAddressItem* item = new CtDetailViewAddressItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addEventItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getEventList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewEventItem *item = new CtDetailViewEventItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addRelationItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getRelationshipList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewRelationItem *item = new CtDetailViewRelationItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addWebsiteItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getUrlList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewWebsiteItem *item = new CtDetailViewWebsiteItem(__detailData, record, true);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL) );
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

void CtMyProfileDetailView::__addMessengerItems()
{
	WHIT();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getMessengerList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewMessengerItem *item = new CtDetailViewMessengerItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL) );
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

//MENU
void CtMyProfileDetailView::__onShareCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtMyProfileDetailView* view = static_cast<CtMyProfileDetailView*>(data);
	WPRET_M(!(view && view->__detailData), "invalid data");

	sharePerson(view->__detailData->getPersonId(), true);
}

//MENU
void CtMyProfileDetailView::__onDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtMyProfileDetailView* view = static_cast<CtMyProfileDetailView*>(data);
	Eina_List* contactList = view->__detailData->getContactsList();
	unsigned int count = eina_list_count(contactList);

	WPopup *popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_PB_BUTTON_DELETE_ABB4");

	char *text = NULL;

	if(view->__isContactSyncToAnyAccount(view->__detailData->getPersonRecord()))// contact synced with any account
	{
		if( count > 1 ) {
			text = strdup(V_("IDS_PB_POP_WHEN_SYNC_IS_ENABLED_THIS_CONTACT_AND_ALL_ITS_LINKED_CONTACTS_IN_OTHER_SYNCED_ACCOUNTS_WILL_BE_DELETED"));
		}
		else {
			text = strdup(V_("IDS_PB_POP_WHEN_SYNC_IS_ENABLED_THIS_CONTACT_WILL_BE_DELETED_FROM_ANY_SYNCED_ACCOUNTS_IT_IS_SAVED_IN"));
		}
	}
	else // contact not synced with any account
	{
		if( count > 1 ) {
			text = strdup(V_("IDS_PB_POP_THIS_CONTACT_AND_ALL_ITS_LINKED_CONTACTS_WILL_BE_DELETED"));
		}
		else {
			text = strdup(V_("IDS_PB_POP_THIS_CONTACT_WILL_BE_DELETED"));
		}
	}

	popup->setContent(text);
	popup->addButton(V_("IDS_PB_BUTTON_CANCEL"), [](bool* destroyPopup){
		WDEBUG("Cancel");
		*destroyPopup = true;
	});
	popup->addButton(V_("IDS_PB_BUTTON_DELETE_ABB4"), [view](bool* destroyPopup){
		WDEBUG("Delete");
		if (CONTACTS_ERROR_NONE == view->__deleteContacts(view)) {
			evas_object_data_set(view->getNaviframe()->getEvasObj(), "ct_list_update_myprofile", (void *)1);

			view->popOut();
			*destroyPopup = false;
		}
		else {
			*destroyPopup = true;
		}
	});

	view->attachPopup( popup );
	SAFE_FREE(text);
}

int CtMyProfileDetailView::__deleteContacts(void *data)
{
	WHIT();
	CtMyProfileDetailView* view = static_cast<CtMyProfileDetailView*>(data);
	int err = CONTACTS_ERROR_NONE;
	WDEBUG("view->getPersonId() = %d", view->getPersonId());

	err = contacts_db_delete_record(_contacts_my_profile._uri, view->getPersonId());
	WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_delete_record() fail(%d)",err);
	if (err == CONTACTS_ERROR_NONE) {
		view->__detailData->detachListener();
		int ret = preference_set_boolean(CT_KEY_MYPROFILE_CREATE_FIRST, true);
		WDEBUG("preference_set_boolean(%d) Failed", ret);
	}
	return err;
}

bool CtMyProfileDetailView::__isContactSyncToAnyAccount(contacts_record_h personRecord)
{
	WHIT();
	contacts_record_h addressbookRecord = NULL;
	char* accountName = NULL;
	int addressbookId = -1;

	int ret = contacts_record_get_int(personRecord, _contacts_my_profile.address_book_id, &addressbookId);
	WPWARN(CONTACTS_ERROR_NONE != ret, "contacts_record_get_str_p() Failed(%d)", ret);
	WDEBUG("addressbookId = %d", addressbookId);

	ret = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &addressbookRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != ret, false, "contacts_db_get_record() Failed(%d)", ret);

	ret = contacts_record_get_str_p(addressbookRecord, _contacts_address_book.name, &accountName);
	WPWARN(CONTACTS_ERROR_NONE != ret, "contacts_record_get_str_p() Failed(%d)", ret);
	WDEBUG("accountName = %s", accountName);

	if(strstr(accountName, "addressbook/phone")==NULL && strstr(accountName, "addressbook/sim")==NULL) {
		contacts_record_destroy(addressbookRecord, true);
		return true;
	}

	contacts_record_destroy(addressbookRecord, true);
	return false;
}

void CtMyProfileDetailView::__updateGenlist()
{
	WHIT();
	elm_genlist_clear(__genlist);
	for (int i = MYPROFILE_DETAIL_MSISDN; i < MYPROFILE_DETAIL_ITEMS_MAX; i++) {
		__genlistItems[i] = {};
	}

	__addAllItems();
}

extern "C" {
	WAPP_ASSIST_EXPORT WView* createMyProfileDetailView() {
		CtMyProfileDetailView* view = new CtMyProfileDetailView();
		return view;
	}
}

