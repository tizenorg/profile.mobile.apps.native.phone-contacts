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
#include <notification.h>
#include <app_preference.h>

#include "WNaviframe.h"
#include "WWindow.h"
#include "WMenuPopup.h"
#include "WToastPopup.h"

#include "ContactsDebug.h"
#include "ContactsAppControl.h"
#include "CtPath.h"
#include "CtCommon.h"
#include "CtThemeMgr.h"
#include "CtRegistryMgr.h"
#include "ContactsCommon.h"

#include "CtDetailView.h"
#include "CtDetailViewBasicInfoControl.h"
#include "CtDetailViewNumberItem.h"
#include "CtDetailViewEmailItem.h"
#include "CtDetailViewNoteItem.h"
#include "CtDetailViewAddressItem.h"
#include "CtDetailViewEventItem.h"
#include "CtDetailViewMessengerItem.h"
#include "CtDetailViewRelationItem.h"
#include "CtDetailViewWebsiteItem.h"
#include "CtDetailData.h"
#include "CtInputView.h"

#define __ITEM_HEIGHT 98
#define __MAX_HEIGHT 680
#define __MAX_HEIGHT_LANDSCAPE 360
#define CT_DETAIL_UPDATE_TIMER 3.0

CtDetailView::CtDetailView(int personId)
{
	__initialize();
	__detailData = new CtDetailData(personId);
	__needHomogeneous = __checkConditionForGenlistHomogeneous(__detailData);
}

CtDetailView::~CtDetailView()
{
	WHIT();

	destroyPopup();

	delete __detailData;
	// TODO Auto-generated destructor stub
}

bool CtDetailView::__checkConditionForGenlistHomogeneous(CtDetailData *detailData)
{
	//addressbook item is multiple line
	Eina_List *addressList = detailData->getAddressList();
	Eina_List *noteList = detailData->getNoteList();
	if(addressList) {
		WDEBUG("address count %d", eina_list_count(addressList));
		if(eina_list_count(addressList) > 0)
			return false;
	}

	if(noteList) {
		WDEBUG("note count %d", eina_list_count(noteList));
		if(eina_list_count(noteList) > 0)
			return false;
	}

	return true;
}

void CtDetailView::__updateGenlist()
{
	WHIT();

	int personId = getPersoinId();

	delete __detailData;
	__detailData = new CtDetailData(personId);
	__detailData->attachListener(this);

	elm_genlist_clear(__genlist);

	__needHomogeneous = __checkConditionForGenlistHomogeneous(__detailData);

	if(__needHomogeneous)
		elm_genlist_homogeneous_set(__genlist, EINA_TRUE);
	else
		elm_genlist_homogeneous_set(__genlist, EINA_FALSE);

	__emailItem = NULL;
	__appendIndex = 0;
	__genlistItemNumberFirst = NULL;
	__genlistItemNumberLast = NULL;
	memset( __genlistItems, 0, sizeof(__genlistItems) );

	__addAllItems();
}

void CtDetailView::__initialize()
{
	WENTER();
	__basicInfo = NULL;
	__appendIndex = 0;
	__genlistItemNumberFirst = NULL;
	__genlistItemNumberLast = NULL;
	__emailItem = NULL;
	__needHomogeneous = false;
	memset( __genlistItems, 0, sizeof(__genlistItems) );

	WLEAVE();
}
const char* CtDetailView::getClassName()
{
	return "CtDetailView";
}

void CtDetailView::onDbChanged(const char* viewUri)
{
	WENTER();

	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	std::string detailLayoutEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);
	// if launch ct-list UG(like select relationship),  when destroy ct-list UG, CtThemeMgr instance will be deleted, so it need to recreate
	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->addExtensionTheme(detailLayoutEdjePath.c_str());
	CtThemeMgr::getInstance()->setObjectTheme(getEvasObj());

	bool isPersonValid = false;
	isPersonValid = __detailData->isCurrentPersonValid();
	if(false == isPersonValid) {
		popOut();
		return;
	}

	__updateGenlist();
	__createBaseInfo();
}

void CtDetailView::onCompleteGetContacts(int offset)
{
	WENTER();
}

void CtDetailView::onConditionChagned()
{
	WENTER();
}

Evas_Object* CtDetailView::__getWindow(void)
{
	if(getWindow())
		return getWindow()->getEvasObj();
	else
		WASSERT_EX(0, "window null");

	return NULL;
}

void CtDetailView::onDestroy()
{
	WHIT();
}

void CtDetailView::onEvent(int eventType)
{
	WENTER();

}

void CtDetailView::__detailBackBtnCb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	WHIT();
	WPRET_M(!data, "data is NULL");
	CtDetailView* view = (CtDetailView*)data;

	view->popOut();
}

void CtDetailView::__createBaseInfo()
{
	WHIT();

	__basicInfo = new CtDetailViewBasicInfoControl(__detailData, true);
	__basicInfo->create(__mainLayout, NULL);
	edje_object_signal_callback_add(_EDJ(__basicInfo->getEvasObj()), "mouse,clicked,*", "back_btn",  __detailBackBtnCb, this);

	elm_object_part_content_set(__mainLayout, "swallow.base_info", __basicInfo->getEvasObj());
}

void CtDetailView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();

	__addAllItems();
	elm_naviframe_item_title_enabled_set(naviItem, EINA_FALSE, EINA_FALSE);

	__detailData->attachListener(this);
	enableMoreButton(naviItem, [](void *data, Evas_Object *obj, void *event_info){
		CtDetailView* view = static_cast<CtDetailView*>(data);
		Eina_List* list = view->__detailData->getContactsList();
		// gal search contact does not have contact list
		WPRET_M(0 == eina_list_count(list), "contacts list is empty");

		WMenuPopup* popup = new WMenuPopup();
		popup->prepare(view->__getWindow(), view->getNaviframe()->getEvasObj() );

		if (!view->__detailData->getReadOnlyMode()) {
			popup->appendItem(V_("IDS_PB_OPT_EDIT"), [view]()
					{
						view->__onEditCb(view, NULL, NULL);
					});
		}
		popup->appendItem(V_("IDS_PB_BUTTON_DELETE_ABB4"), [view]()
				{
					view->__onDeleteCb(view, NULL, NULL);
				});
		if (!view->__detailData->getReadOnlyMode()) {
			popup->appendItem(V_("IDS_PB_HEADER_SHARE_VIA"), [view]()
					{
						view->__onShareCb(view, NULL, NULL);
					});
		}

		view->attachPopup( popup );

	}, this);

	__createBaseInfo();

	WLEAVE();
}

Evas_Object* CtDetailView::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();
	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	std::string detailLayoutEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);
	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->addExtensionTheme(detailLayoutEdjePath.c_str());

	__mainLayout = elm_layout_add(parent);
	elm_layout_file_set(__mainLayout, detailLayoutEdjePath.c_str(), "main_layout");

	CtThemeMgr::getInstance()->setObjectTheme(__mainLayout);

	Evas_Object* genlist = elm_genlist_add(__mainLayout);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	if(__needHomogeneous)
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);

	evas_object_show(genlist);
	__genlist = genlist;
	elm_object_part_content_set(__mainLayout, "swallow.content", genlist);

	WLEAVE();
	return __mainLayout;
}

void CtDetailView::onLanguageChanged()
{
	WHIT();

	if(__genlist)
		elm_genlist_realized_items_update(__genlist);
}

int CtDetailView::getPersoinId(void)
{
	WASSERT_EX(NULL != __detailData, "__detailData is null");
	return __detailData->getPersonId();
}

void CtDetailView::__addAllItems()
{
	WENTER();
	for (int i = __appendIndex; i < DETAIL_ITEMS_MAX; i++) {
		__addItem((CtDetailItems) i);
	}

	WLEAVE();
}

void CtDetailView::__addItem(CtDetailItems itemType)
{
	WENTER();
	WDEBUG("itemType %d", (int)itemType);
	switch(itemType) {
	case DETAIL_NUMBER:
		__addNumberItems();
		break;
	case DETAIL_EMAIL:
		__addEmailItems();
		break;
	case DETAIL_NOTE:
		__addNoteItems();
		break;
	case DETAIL_ADDRESS:
		__addAddressItems();
		break;
	case DETAIL_EVENT:
		__addEventItems();
		break;
	case DETAIL_RELATIONSHIP:
		__addRelationItems();
		break;
	case DETAIL_WEBSITE:
		__addWebsiteItems();
		break;
	case DETAIL_MESSENGER:
		__addMessengerItems();
		break;
	case DETAIL_ADDRBOOK:
		__addAddressbookItems();
		break;

        default:
            WDEBUG("Unsupported type: %d", itemType);
            break;
	}
	WLEAVE();
}

void CtDetailView::__addNumberItems()
{
	WENTER();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;
	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getNumberList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewNumberItem *item = new CtDetailViewNumberItem(__detailData, record);
		Elm_Object_Item *obj = elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, __onPhonenumberItemSelectedCb, item);
		item->setElmObjectItem(obj);
		if (NULL == __genlistItemNumberFirst)
			__genlistItemNumberFirst = (Elm_Object_Item*)item->getElmObjectItem();
		__genlistItemNumberLast= (Elm_Object_Item*)item->getElmObjectItem();
	}

	WLEAVE();
}

void CtDetailView::__addEmailItems()
{
	WENTER();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getEmailList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewEmailItem *item = new CtDetailViewEmailItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, __onEmailItemSelectedCb, item));
		__emailItem = (Elm_Object_Item*)item->getElmObjectItem();
	}
	WLEAVE();
}

void CtDetailView::__addNoteItems()
{
	WENTER();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getNoteList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewNoteItem *item = new CtDetailViewNoteItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
	WLEAVE();
}

void CtDetailView::__addAddressItems()
{
	WENTER();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getAddressList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewAddressItem *item = new CtDetailViewAddressItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL));
		elm_genlist_item_select_mode_set((Elm_Object_Item*)item->getElmObjectItem(), ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
	WLEAVE();
}

void CtDetailView::__addEventItems()
{
	WENTER();
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
	WLEAVE();
}

void CtDetailView::__addRelationItems()
{
	WENTER();
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
	WLEAVE();
}

void CtDetailView::__addWebsiteItems()
{
	WENTER();
	void *data = NULL;
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;

	WASSERT_EX(NULL != __detailData, "__detailData is null");
	list = __detailData->getUrlList();
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h record = (contacts_record_h)data;
		if(record == NULL)
			continue;

		CtDetailViewWebsiteItem *item = new CtDetailViewWebsiteItem(__detailData, record);
		item->setElmObjectItem(elm_genlist_item_append(__genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, __onWebsiteItemSelectedCb, item) );
	}
	WLEAVE();
}

void CtDetailView::__addMessengerItems()
{
	WENTER();
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
	WLEAVE();
}

void CtDetailView::__addAddressbookItems()
{
	WENTER();
	WLEAVE();
}

void CtDetailView::__onPhonenumberItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtDetailViewNumberItem* item = (CtDetailViewNumberItem*)data;
	Elm_Object_Item * elmItem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(elmItem, EINA_FALSE);

	item->onSelect(data, obj, event_info);
}

void CtDetailView::__onEmailItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtDetailViewEmailItem* item = (CtDetailViewEmailItem*)data;
	Elm_Object_Item *elmItem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(elmItem, EINA_FALSE);

	item->onSelect(data, obj, event_info);
}

void CtDetailView::__onWebsiteItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtDetailViewWebsiteItem* item = (CtDetailViewWebsiteItem*)data;
	Elm_Object_Item *elmItem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(elmItem, EINA_FALSE);

	item->onSelect(data, obj, event_info);
}

void CtDetailView::__onEditCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtDetailView* view = static_cast<CtDetailView*>(data);
	int err = CONTACTS_ERROR_NONE;
	Eina_List* contactList = view->__detailData->getContactsWithoutReadOnly();

	contacts_record_h record_contact = static_cast<contacts_record_h>(contactList->data);
	contacts_record_h record_contact_cloned = NULL;
	err = contacts_record_clone(record_contact, &record_contact_cloned);
	WPRDT_M(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
	view->getNaviframe()->push(new CtInputView(record_contact_cloned), NULL, NULL);
}

void CtDetailView::__onDeleteCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtDetailView* view = static_cast<CtDetailView*>(data);
	Eina_List* contactList = view->__detailData->getContactsList();
	unsigned int count = eina_list_count(contactList);

	WPopup *popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_PB_BUTTON_DELETE_ABB4");

	if (count > 1) {
		popup->setContent("IDS_PB_POP_THIS_CONTACT_AND_ALL_ITS_LINKED_CONTACTS_WILL_BE_DELETED");
	}
	else {
		popup->setContent("IDS_PB_POP_THIS_CONTACT_WILL_BE_DELETED");
	}

	popup->addButton("IDS_PB_BUTTON_CANCEL", [](bool* destroyPopup){
		WDEBUG("Cancel");
		*destroyPopup = true;
	});
	popup->addButton("IDS_PB_BUTTON_DELETE_ABB4", [view](bool* destroyPopup){
		WDEBUG("Delete");
		if (CONTACTS_ERROR_NONE == __deleteContacts(view)) {
//			view->popOut(); // popOut is called twice
			*destroyPopup = false;
		}
		else {
			*destroyPopup = true;
		}
	});

	view->attachPopup( popup );
}

int CtDetailView::__deleteContacts(void *data)
{
	WHIT();
	CtDetailView* view = static_cast<CtDetailView*>(data);
	int err = CONTACTS_ERROR_NONE;

	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_DELETED_PERSON, 1);
	CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_DELETED_PERSON_ID, view->getPersoinId());

	err = contacts_db_delete_record(_contacts_person._uri, view->getPersoinId());
	WPWARN(err != CONTACTS_ERROR_NONE, "contacts_db_delete_record() fail(%d)",err);
	return err;
}

void CtDetailView::__onShareCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtDetailView* view = static_cast<CtDetailView*>(data);
	WPRET_M(!(view && view->__detailData), "invalid data");

	sharePerson(view->__detailData->getPersonId(), false);
}

void CtDetailView::__updateNumberFields()
{
	WHIT();
	Elm_Object_Item* item = __genlistItemNumberFirst;

	while (item) {
		elm_genlist_item_fields_update(item, "elm.icon.left.bottom", ELM_GENLIST_ITEM_FIELD_CONTENT);
		if (item == __genlistItemNumberLast) {
			break;
		}
		item = elm_genlist_item_next_get(item);
	}
}

int CtDetailView::__getHeightByItemCount(Evas_Object* win, int count)
{
	int height = ELM_SCALE_SIZE(__ITEM_HEIGHT * count);
	int maxHeight = ELM_SCALE_SIZE(__MAX_HEIGHT);
	int rotation = elm_win_rotation_get(win);
	if (90 == rotation || 270 == rotation) {
		maxHeight = ELM_SCALE_SIZE(__MAX_HEIGHT_LANDSCAPE);
	}
	if (height > maxHeight) {
		return maxHeight;
	}
	else {
		return height;
	}
}
