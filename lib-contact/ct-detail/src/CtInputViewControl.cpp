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
#include <app_preference.h>
#include <contacts.h>

#include "WPopup.h"

#include "ContactsDebug.h"
#include "ContactsAppControl.h"

#include "CtString.h"
#include "CtPath.h"
#include "CtInputData.h"
#include "CtInputViewControl.h"
#include "CtCommon.h"
#include "CtDetailView.h"
#include "CtInputView.h"

#include "CtInputViewGenlistItem.h"
#include "CtInputViewBasicInfoItem.h"
#include "CtInputViewEmailItem.h"
#include "CtInputViewNoteItem.h"
#include "CtInputViewMessengerItem.h"
#include "CtInputViewAddressItem.h"
#include "CtInputViewWebsiteItem.h"
#include "CtInputViewRelationItem.h"
#include "CtInputViewEventItem.h"
#include "CtInputViewNumberItem.h"
#include "CtInputViewImportantFieldsItem.h"
#include "CtInputViewImportantSubItem.h"
#include "CtThemeMgr.h"
#include "SystemSettingsMgr.h"

#define GALLERY_UG "ug-gallery-efl"
#define INPUT_CREATE_KEYPAD_TIMER 0.2
#define DATE_TIME_FORMAT "%%c"


CtInputViewControl::CtInputViewControl( std::shared_ptr<CtData>& inputData, CtInputView* view )
{
	__inputData = inputData;
	__view = view;
	__initialize();
}

CtInputViewControl::~CtInputViewControl()
{
	WHIT();

	if (__conformant) {
		evas_object_smart_callback_del(__conformant, "virtualkeypad,state,on", __onSipUpCb);
		evas_object_smart_callback_del(__conformant, "virtualkeypad,state,off", __onSipDownCb);
	}

	if (__keypadTimer) {
		ecore_timer_del(__keypadTimer);
		__keypadTimer = NULL;
	}
}

bool CtInputViewControl::getIsCreate()
{
	return __view->getIsCreate();
}

bool CtInputViewControl::getIsEmpty()
{
	return __checkIsEmpty();
}

bool CtInputViewControl::getIsChanged()
{
	return __checkIsChanged();
}

void CtInputViewControl::setIsDestroy(bool isDestroy)
{
	__isDestroy = isDestroy;
}

Evas_Object* CtInputViewControl::getViewEvasObj()
{
	return __view->getEvasObj();
}

Evas_Object* CtInputViewControl::getNaviframeEvasObj()
{
	return __view->getNaviframeEvasObj();
}

Evas_Object* CtInputViewControl::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();
	Evas_Object* genlist = elm_genlist_add(parent);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_show(genlist);
	__genlist = genlist;

	evas_object_smart_callback_add(genlist, "realized", __genlistRealizedCb, this);
	evas_object_smart_callback_add(genlist, "unrealized", __genlistUnrealizedCb, this);
	__addAllItems(genlist);

	// set virtual keypad
	__conformant = __view->getConformantEvasObject();
	if (__conformant) {
		evas_object_smart_callback_add(__conformant, "virtualkeypad,state,on", __onSipUpCb, this);
		evas_object_smart_callback_add(__conformant, "virtualkeypad,state,off", __onSipDownCb, this);
	}

	WLEAVE();
	return genlist;
}

void CtInputViewControl::onDestroy()
{
	evas_object_smart_callback_del(__genlist, "unrealized", __genlistUnrealizedCb);
	evas_object_smart_callback_del(__genlist, "realized", __genlistRealizedCb);
	SystemSettingsMgr::getInstance().unregisterChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, __regionFormatChangedCb);
}

void CtInputViewControl::__initialize()
{
	__changed = false;
	__isDestroy = false;
	__needResetTheme = false;
	__needShowMorePopup = false;
	__sipIsShown = false;
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NONE;
	__keypadTimer = NULL;

	memset( __genlistItems, 0, sizeof(__genlistItems) );

	__genlist = NULL;
	__conformant = NULL;
	__birthdayEventExist = std::make_shared<bool> (false);


	SystemSettingsMgr::getInstance().registerChangedCallback(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, __regionFormatChangedCb, this);
}

void CtInputViewControl::__regionFormatChangedCb(system_settings_key_e key, void *data)
{
	CtInputViewControl *control = (CtInputViewControl*)data;
	char *regionFormat = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, &regionFormat);

	WDEBUG("regionFormat %s", regionFormat);

	if(control->__genlistItems[INPUT_EVENT])
		elm_genlist_realized_items_update(control->__genlist);
}

void CtInputViewControl::__addAllItems(Evas_Object* genlist)
{
	WENTER();
	Eina_List *cursor = NULL;
	Eina_List *list = NULL;
	void *data = NULL;
	contacts_record_h record = NULL;
	int itemType = 0;

	for (; itemType < INPUT_ITEMS_MAX; itemType++) {
		switch(itemType) {
		case INPUT_BASIC_INFO:
		{
			CtInputViewBasicInfoItem *item = new CtInputViewBasicInfoItem(__inputData,&__nameEntry,
					[this](){ __itemChanged(); },
					[this](CtInputViewBasicInfoItem::ITEM_DELETED deletedInfo){ __basicInfoItemDeleted( deletedInfo); },
					[this](Evas_Object* obj){ __basicInfoItemImageClicked(obj); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); },
					[this](Evas_Object* obj){ basicInfoItemShowCreateKeypad(obj); },
					[this](Elm_Object_Item* objItem){ __updateEntryReturnKeyType(objItem); });
			Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[INPUT_BASIC_INFO] = genItem;
			break;
		}
		case INPUT_NUMBER:
		{
			list = __inputData->getNumberList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}

				WDEBUG("added");

				const char *number = __inputData->getNumberNumber(record);

				CtInputViewNumberItem *item = new CtInputViewNumberItem( this, __inputData, record, number, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_NUMBER] = genItem;
			}

			if (__view->getIsCreate() && list == NULL) {
				WDEBUG("set base number");
				record = __inputData->addNumber("");

				CtInputViewNumberItem *item = new CtInputViewNumberItem( this, __inputData, record, NULL, INPUT_ADDED_ITEM_INDEX_NONE,
					[this](){ __itemChanged(); },
					[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
					[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_NUMBER] = genItem;
			}
			break;
		}
		case INPUT_EMAIL:
		{
			list = __inputData->getEmailList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) continue;

				const char *email = __inputData->getEmailEmail(record);

				CtInputViewEmailItem *item = new CtInputViewEmailItem( __inputData, record, email, INPUT_ADDED_ITEM_INDEX_NONE,
						[this](){ __itemChanged( ); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __emailItemDeleted( objItem, isChanged, record ); },
						[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_EMAIL] = genItem;
			}
			break;
		}
		case INPUT_ADDRESS:
		{
			list = __inputData->getAddressList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}

				char address[CT_TEXT_MAX_LEN] = {0,};
				__inputData->getAddressAddress( record, address, sizeof(address));

				CtInputViewAddressItem *item = new CtInputViewAddressItem( __inputData, record, address, INPUT_ADDED_ITEM_INDEX_NONE,
						[this](){ __itemChanged(); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __addressItemDeleted( objItem, isChanged, record ); },
						[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_ADDRESS] = genItem;
			}

			break;
		}
		case INPUT_MESSENGER:
		{
			list = __inputData->getMessengerList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}

				const char *messenger = __inputData->getMessengerMessenger(record);

				CtInputViewMessengerItem *item = new CtInputViewMessengerItem( __inputData, record, messenger, INPUT_ADDED_ITEM_INDEX_NONE,
						[this](){ __itemChanged(); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __messengerItemDeleted( objItem, isChanged, record ); },
						[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_MESSENGER] = genItem;
			}
			break;
		}
		case INPUT_URL:
		{
			list = __inputData->getWebsiteList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}

				const char *website = __inputData->getWebsiteWebsite(record);

				CtInputViewWebsiteItem *item = new CtInputViewWebsiteItem( __inputData, record, website, INPUT_ADDED_ITEM_INDEX_NONE,
						[this](){ __itemChanged(); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __websiteItemDeleted( objItem, isChanged, record ); },
						[this](Evas_Object* obj, Elm_Object_Item* objItem){__activateNextEntry(obj, objItem); } );
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_URL] = genItem;
			}
			break;
		}
		case INPUT_RELATIONSHIP:
		{
			list = __inputData->getRelationshipList();

			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}

				const char *relationship = __inputData->getRelationshipRelationship(record);

				CtInputViewRelationItem *item = new CtInputViewRelationItem( this,__inputData, record, relationship,
						[this](){ __itemChanged(); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __relationItemDeleted( objItem, isChanged, record ); },
						[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); });
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_RELATIONSHIP] = genItem;
			}
			break;
		}
		case INPUT_EVENT:
		{
			list = __inputData->getEventList();

			*__birthdayEventExist = false;
			EINA_LIST_FOREACH(list, cursor, data) {
				contacts_record_h record = (contacts_record_h)data;
				if(record == NULL) {
					continue;
				}
				char event[256];
				snprintf( event, sizeof(event), "%d", __inputData->getEventEvent(record) );

				if( __inputData->getEventType(record) == CONTACTS_EVENT_TYPE_BIRTH)
					*__birthdayEventExist = true;

				CtInputViewEventItem *item = new CtInputViewEventItem( __view, __inputData, record, event, __birthdayEventExist,
						[this](){ __itemChanged(); },
						[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __eventItemDeleted( objItem, isChanged, record ); });
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_EVENT] = genItem;
			}
			break;
		}
		case INPUT_NOTE:
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
						[this](Evas_Object* obj, Elm_Object_Item* objItem){this->__activateNextEntry(obj, objItem);} );
				Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
				item->setElmObjectItem( genItem );
				__genlistItems[INPUT_NOTE] = genItem;
				__updateNoteBtn(true);
			}
			break;
		}
		case INPUT_IMPORTANT_FIELDS:
		{
			bool isNoteInput = (__genlistItems[INPUT_NOTE] == NULL) ? false : true;
			CtInputViewImportantFieldsItem *item = new CtInputViewImportantFieldsItem([this](){ __addNumberClickedCb(); },
					[this](){ __addEmailClickedCb(); },
					[this](){ __addNoteClickedCb(); },
					[this](){ __addMoreClickedCb(); },
					isNoteInput);
			Elm_Object_Item *genItem = elm_genlist_item_append(genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
			item->setElmObjectItem( genItem );
			__genlistItems[INPUT_IMPORTANT_FIELDS] = genItem;
			break;
		}
		}
	}
	WLEAVE();
}

void CtInputViewControl:: __genlistRealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtInputViewControl *control = (CtInputViewControl*)data;

	control->__updateEntryReturnKeyType(NULL);

	Elm_Object_Item *objectItem = (Elm_Object_Item*) event_info;
	CtInputViewGenlistItem *item = (CtInputViewGenlistItem*) elm_object_item_data_get(objectItem);
	if (item) {
		item->onRealized();
	}

	if (objectItem == control->__genlistItems[INPUT_BASIC_INFO]) {
		control->__updateBasicInfoItem();
	}
}

void CtInputViewControl::__genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *objectItem = (Elm_Object_Item*) event_info;
	CtInputViewGenlistItem *item = (CtInputViewGenlistItem*) elm_object_item_data_get(objectItem);
	if (item) {
		item->onUnrealized();
	}
}

void CtInputViewControl::__updateNoteBtn(bool isNoteInput)
{
	WHIT();

	if(__genlistItems[INPUT_IMPORTANT_FIELDS]) {
		CtInputViewImportantFieldsItem *item = (CtInputViewImportantFieldsItem*)elm_object_item_data_get(__genlistItems[INPUT_IMPORTANT_FIELDS]);
		if(item) {
			item->updateNoteBtn(isNoteInput);
		}
	}

}

void CtInputViewControl::showTopItem()
{
	WHIT();

	elm_genlist_item_show(__genlistItems[INPUT_BASIC_INFO], ELM_GENLIST_ITEM_SCROLLTO_TOP);

	bool first = false;
	int error = preference_get_boolean(CT_KEY_CONTACT_CREATE_FIRST, &first);
	WPRET_M(error != PREFERENCE_ERROR_NONE, "preference_get_boolean failed");

	if (!first && __inputData->getContactId() == 0 && __nameEntry) {
		elm_object_focus_set(__nameEntry, EINA_TRUE);
		elm_entry_cursor_end_set(__nameEntry);
	}
}

void CtInputViewControl::__addNumberClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;

	record = __inputData->addNumber("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NUMBER;

	CtInputViewNumberItem* item = new CtInputViewNumberItem( this, __inputData, record, NULL, __addedIndex,
		[this](){ __itemChanged(); },
		[this](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ __numberItemDeleted( objItem, isChanged, record ); },
		[this](Evas_Object* obj, Elm_Object_Item* objItem){ __activateNextEntry(obj, objItem); } );

	Elm_Object_Item* genItem = NULL;
	if (__genlistItems[INPUT_NUMBER])
	{
		genItem = elm_genlist_item_insert_after(getEvasObj(), item->getItemClassStatic(), item, NULL, __genlistItems[INPUT_NUMBER], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	else
	{
		genItem = elm_genlist_item_insert_after(getEvasObj(), item->getItemClassStatic(), item, NULL, __genlistItems[INPUT_BASIC_INFO], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	__genlistItems[INPUT_NUMBER] = genItem;
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	item->focus();
}

void CtInputViewControl::__addEmailClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;
	CtInputItems checkExistItem = INPUT_BASIC_INFO;

	record = __inputData->addEmail("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_EMAIL;

	if (__genlistItems[INPUT_EMAIL]) {
		checkExistItem = INPUT_EMAIL;
	}
	else if (__genlistItems[INPUT_NUMBER]) {
		checkExistItem = INPUT_NUMBER;
	}
	else {
		checkExistItem = INPUT_BASIC_INFO;
	}
	WDEBUG("INPUT_EMAIL item is after %d item", checkExistItem);

	CtInputViewEmailItem *item = new CtInputViewEmailItem( __inputData, record, NULL, __addedIndex,
			[this](){ __itemChanged(); },
			[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __emailItemDeleted( item, isChanged, record ); },
			[this](Evas_Object* obj, Elm_Object_Item* item){ __activateNextEntry(obj, item); } );
	Elm_Object_Item* genItem = elm_genlist_item_insert_after( getEvasObj(), item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	__genlistItems[INPUT_EMAIL] = genItem;
	elm_genlist_item_show( genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	item->focus();
}

void CtInputViewControl::__addNoteClickedCb()
{
	WHIT();
	contacts_record_h record = NULL;
	CtInputItems checkExistItem = INPUT_BASIC_INFO;

	WPRET_M(__genlistItems[INPUT_NOTE], "note already is added");

	record = __inputData->addNote("");
	__addedIndex = INPUT_ADDED_ITEM_INDEX_NOTE;

	if (__genlistItems[INPUT_EMAIL]) {
		checkExistItem = INPUT_EMAIL;
	}
	else if (__genlistItems[INPUT_NUMBER]) {
		checkExistItem = INPUT_NUMBER;
	}
	else {
		checkExistItem = INPUT_BASIC_INFO;
	}

	CtInputViewNoteItem *item = new CtInputViewNoteItem( __inputData, record, NULL, __addedIndex,
			[this](){ __itemChanged(); },
			[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __noteItemDeleted( item, isChanged, record ); },
			[this](Evas_Object* obj, Elm_Object_Item* objItem){this->__activateNextEntry(obj, objItem);} );
	Elm_Object_Item* genItem = elm_genlist_item_insert_after( getEvasObj(), item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	__genlistItems[INPUT_NOTE] = genItem;
	__updateNoteBtn(true);
	elm_genlist_item_show( genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	item->focus();
}

void CtInputViewControl::__addMoreClickedCb()
{
	WHIT();

	if (__sipIsShown) {
		// show popup in __onSipDownCb
		__needShowMorePopup = true;
	}
	else {
		__showMorePopup();
	}
}

void CtInputViewControl::__importantSubitemSelectedCb(void* data, Evas_Object* obj, void* event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtInputSubItems selectedSubitem = INPUT_IMPORTANT_NONE;
	CtInputViewControl *control = (CtInputViewControl*)data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	CtInputViewImportantSubItem *importantSubItem = (CtInputViewImportantSubItem *)elm_object_item_data_get(item);
	contacts_record_h record = NULL;
	CtInputItems checkExistItem = INPUT_BASIC_INFO;
	Elm_Object_Item *addedItem = NULL;

	selectedSubitem = (CtInputSubItems)importantSubItem->getIndex();

	elm_genlist_item_selected_set(item, EINA_FALSE);

	switch (selectedSubitem) {
	case INPUT_IMPORTANT_PHONETIC_INFO:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(control->__genlistItems[INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddPhoneticName(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_PHONETIC_INFO);
		elm_genlist_item_fields_update(control->__genlistItems[INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = control->__genlistItems[INPUT_BASIC_INFO];
		control->destroyPopup();
		break;
	}
	case INPUT_IMPORTANT_MESSENGER:
	{
		record = control->__inputData->addMessenger("");
		control->__addedIndex = INPUT_ADDED_ITEM_INDEX_IM;

		if (control->__genlistItems[INPUT_MESSENGER]) {
			checkExistItem = INPUT_MESSENGER;
		}
		else if (control->__genlistItems[INPUT_URL]) {
			checkExistItem = INPUT_URL;
		}
		else if (control->__genlistItems[INPUT_RELATIONSHIP]) {
			checkExistItem = INPUT_RELATIONSHIP;
		}
		else if (control->__genlistItems[INPUT_EVENT]) {
			checkExistItem = INPUT_EVENT;
		}
		else if (control->__genlistItems[INPUT_ADDRESS]) {
			checkExistItem = INPUT_ADDRESS;
		}
		else if (control->__genlistItems[INPUT_NOTE]) {
			checkExistItem = INPUT_NOTE;
		}
		else if (control->__genlistItems[INPUT_EMAIL]) {
			checkExistItem = INPUT_EMAIL;
		}
		else if (control->__genlistItems[INPUT_NUMBER]) {
			checkExistItem = INPUT_NUMBER;
		}
		else {
			checkExistItem = INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_MESSENGER item is after %d item", checkExistItem);

		CtInputViewMessengerItem *item = new CtInputViewMessengerItem( control->__inputData, record, NULL, control->__addedIndex,
			[control](){ control->__itemChanged(); },
			[control](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ control->__messengerItemDeleted( item, isChanged, record ); },
			[control](Evas_Object* obj, Elm_Object_Item* item){ control->__activateNextEntry(obj, item); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( control->getEvasObj(), item->getItemClassStatic(), item, NULL, control->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		control->__genlistItems[INPUT_MESSENGER] = genItem;
		addedItem = genItem;
		control->destroyPopup();
		item->focus();
		break;
	}
	case INPUT_IMPORTANT_URL:
	{
		record = control->__inputData->addWebsite("");
		control->__addedIndex = INPUT_ADDED_ITEM_INDEX_URL;

		if (control->__genlistItems[INPUT_URL]) {
			checkExistItem = INPUT_URL;
		}
		else if (control->__genlistItems[INPUT_RELATIONSHIP]) {
			checkExistItem = INPUT_RELATIONSHIP;
		}
		else if (control->__genlistItems[INPUT_EVENT]) {
			checkExistItem = INPUT_EVENT;
		}
		else if (control->__genlistItems[INPUT_ADDRESS]) {
			checkExistItem = INPUT_ADDRESS;
		}
		else if (control->__genlistItems[INPUT_NOTE]) {
			checkExistItem = INPUT_NOTE;
		}
		else if (control->__genlistItems[INPUT_EMAIL]) {
			checkExistItem = INPUT_EMAIL;
		}
		else if (control->__genlistItems[INPUT_NUMBER]) {
			checkExistItem = INPUT_NUMBER;
		}
		else {
			checkExistItem = INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_URL item is after %d item", checkExistItem);

		CtInputViewWebsiteItem *item = new CtInputViewWebsiteItem( control->__inputData, record, NULL, control->__addedIndex,
			[control](){ control->__itemChanged(); },
			[control](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ control->__websiteItemDeleted( item, isChanged, record );},
			[control](Evas_Object* obj, Elm_Object_Item* item){ control->__activateNextEntry(obj, item); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( control->getEvasObj(), item->getItemClassStatic(), item, NULL, control->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		control->__genlistItems[INPUT_URL] = genItem;
		addedItem = genItem;
		control->destroyPopup();
		item->focus();
		break;
	}
	case INPUT_IMPORTANT_ORGANIZATION:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(control->__genlistItems[INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddOrganization(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_ORGANIZATION);
		elm_genlist_item_fields_update(control->__genlistItems[INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = control->__genlistItems[INPUT_BASIC_INFO];
		control->destroyPopup();
		break;
	}
	case INPUT_IMPORTANT_RELATIONSHIP:
	{
		record = control->__inputData->addRelationship("");
		control->__addedIndex = INPUT_ADDED_ITEM_INDEX_RELATIONSHIP;

		if (control->__genlistItems[INPUT_RELATIONSHIP]) {
			checkExistItem = INPUT_RELATIONSHIP;
		}
		else if (control->__genlistItems[INPUT_EVENT]) {
			checkExistItem = INPUT_EVENT;
		}
		else if (control->__genlistItems[INPUT_ADDRESS]) {
			checkExistItem = INPUT_ADDRESS;
		}
		else if (control->__genlistItems[INPUT_NOTE]) {
			checkExistItem = INPUT_NOTE;
		}
		else if (control->__genlistItems[INPUT_EMAIL]) {
			checkExistItem = INPUT_EMAIL;
		}
		else if (control->__genlistItems[INPUT_NUMBER]) {
			checkExistItem = INPUT_NUMBER;
		}
		else {
			checkExistItem = INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_RELATIONSHIP item is after %d item", checkExistItem);

		CtInputViewRelationItem *item = new CtInputViewRelationItem( control,control->__inputData, record, NULL,
			[control](){ control->__itemChanged(); },
			[control](Elm_Object_Item* objItem, bool isChanged, contacts_record_h record){ control->__relationItemDeleted( objItem, isChanged, record );},
			[control](Evas_Object* obj, Elm_Object_Item* objItem){ control->__activateNextEntry(obj, objItem); });
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( control->getEvasObj(), item->getItemClassStatic(), item, NULL, control->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		control->__genlistItems[INPUT_RELATIONSHIP] = genItem;
		addedItem = genItem;
		control->destroyPopup();
		item->focus();
		break;
	}
	case INPUT_IMPORTANT_NICKNAME:
	{
		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(control->__genlistItems[INPUT_BASIC_INFO]);
		WPRET_M(NULL == basicInfoItem, "basicInfoItem is NULL");

		elm_object_item_disabled_set(item, EINA_TRUE);
		basicInfoItem->setAddNickname(true);
		basicInfoItem->setAddedIndex(INPUT_ADDED_ITEM_INDEX_NICKNAME);
		elm_genlist_item_fields_update(control->__genlistItems[INPUT_BASIC_INFO], "elm.icon.1", ELM_GENLIST_ITEM_FIELD_CONTENT);
		addedItem = control->__genlistItems[INPUT_BASIC_INFO];
		control->destroyPopup();
		break;
	}
	case INPUT_IMPORTANT_EVENT:
	{
		control->__createEventDataPopup();
		break;
	}
	case INPUT_IMPORTANT_ADDRESS:
	{
		record = control->__inputData->addAddress("");
		control->__addedIndex = INPUT_ADDED_ITEM_INDEX_ADDRESS;

		if (control->__genlistItems[INPUT_ADDRESS]) {
			checkExistItem = INPUT_ADDRESS;
		}
		else if (control->__genlistItems[INPUT_NOTE]) {
			checkExistItem = INPUT_NOTE;
		}
		else if (control->__genlistItems[INPUT_EMAIL]) {
			checkExistItem = INPUT_EMAIL;
		}
		else if (control->__genlistItems[INPUT_NUMBER]) {
			checkExistItem = INPUT_NUMBER;
		}
		else {
			checkExistItem = INPUT_BASIC_INFO;
		}
		WDEBUG("INPUT_ADDRESS item is after %d item", checkExistItem);

		CtInputViewAddressItem *item = new CtInputViewAddressItem( control->__inputData, record, NULL, control->__addedIndex,
			[control](){ control->__itemChanged(); },
			[control](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ control->__addressItemDeleted( item, isChanged, record ); },
			[control](Evas_Object* obj, Elm_Object_Item* item){ control->__activateNextEntry(obj, item); } );
		Elm_Object_Item* genItem = elm_genlist_item_insert_after( control->getEvasObj(), item->getItemClassStatic(), item, NULL, control->__genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
		item->setElmObjectItem( genItem );
		control->__genlistItems[INPUT_ADDRESS] = genItem;
		addedItem = genItem;
		control->destroyPopup();
		item->focus();
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

bool CtInputViewControl::__checkIsEmpty()
{
	bool isEmpty = true;

	CtInputViewGenlistItem *nameItem = (CtInputViewGenlistItem*)
			elm_object_item_data_get(__genlistItems[INPUT_BASIC_INFO]);
	CtInputViewGenlistItem *numberItem = (CtInputViewGenlistItem*)
			elm_object_item_data_get(__genlistItems[INPUT_NUMBER]);

	if (nameItem) {
		isEmpty = nameItem->getIsEmpty();
	}

	if (numberItem) {
		isEmpty = isEmpty && numberItem->getIsEmpty();
	}

	return isEmpty;
}

bool CtInputViewControl::__checkIsChanged()
{
	Elm_Object_Item *item = __genlistItems[INPUT_BASIC_INFO];
	bool changed = false;

	if (__changed) return __changed;
	while (item) {
		if (item == __genlistItems[INPUT_IMPORTANT_FIELDS]) {
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

void CtInputViewControl::__updateSaveBtn()
{
	WHIT();
	Eina_Bool empty = __checkIsEmpty();
	__view->updateSaveBtn(empty);
}

void CtInputViewControl::__itemChanged()
{
	__updateSaveBtn();
}

void CtInputViewControl::__basicInfoItemDeleted( int deletedInfo)
{
	CtInputViewBasicInfoItem::ITEM_DELETED info = (CtInputViewBasicInfoItem::ITEM_DELETED)deletedInfo;

	CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(__genlistItems[INPUT_BASIC_INFO]);
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
	__updateEntryReturnKeyType(__genlistItems[INPUT_BASIC_INFO]);
}

void CtInputViewControl::__basicInfoItemImageClicked( Evas_Object* obj )
{
	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle( "IDS_PB_HEADER_SET_CONTACT_PICTURE_ABB");
	popup->setContent([this](Evas_Object* parent)->Evas_Object*
	{
		int count = 2;
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
					CtInputViewControl *control = (CtInputViewControl*)data;
					if(control->__inputData->getImagePath())
						return SAFE_STRDUP(V_("IDS_PB_OPT_REMOVE"));
					else
						return NULL;

				}
				return NULL;
			};
		}

		Evas_Object* genlist = elm_genlist_add(parent);
		elm_object_style_set( genlist, "popup" );
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated
		elm_object_content_set(parent, genlist);

		elm_genlist_item_append(genlist, itc1, this, NULL, ELM_GENLIST_ITEM_NONE, __onGallerySelCb, this);
		elm_genlist_item_append(genlist, itc2, this, NULL, ELM_GENLIST_ITEM_NONE, __onCameraSelCb, this);

		if(__inputData->getImagePath()) {
			elm_genlist_item_append(genlist, itc3, this, NULL, ELM_GENLIST_ITEM_NONE, __onRemoveSelCb, this);
			count++;
		}

		evas_object_show(genlist);
		elm_genlist_item_class_free(itc1);
		elm_genlist_item_class_free(itc2);
		elm_genlist_item_class_free(itc3);

		return genlist;

	});
	// Make a parent as window. Issue: when IME is displayed, the pop-up is displayed at the moment upper side of IME keyboard, if its parent is not window.2014.11.05
	popup->create(__view->getWindow()->getEvasObj(), NULL );

	attachPopup( popup );
}

void CtInputViewControl::__activateNextEntry( Evas_Object* obj, Elm_Object_Item* item ) // activatedCb
{
	WHIT();
	WPRET_M(NULL == obj, "obj is NULL");
	WPRET_M(NULL == item, "item is NULL");
	Elm_Object_Item* nextItem = elm_genlist_item_next_get(item);
	bool focused = false;
	Evas_Object* entry = obj;
	while (nextItem) {
		if (nextItem == __genlistItems[INPUT_IMPORTANT_FIELDS]) {
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

void CtInputViewControl::basicInfoItemShowCreateKeypad( Evas_Object* obj)
{
	WHIT();
	if (__inputData->getContactId() == 0 && __nameEntry) {
		elm_object_focus_set(__nameEntry, EINA_TRUE);
		elm_entry_cursor_end_set(__nameEntry);
	}
}

void CtInputViewControl::__basicInfoUpdateImage()
{
	WHIT();

	elm_genlist_item_fields_update(__genlistItems[INPUT_BASIC_INFO], "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
	return;
}

Elm_Object_Item* CtInputViewControl::__getLastItem(Elm_Object_Item* deleteItem)
{
	Elm_Object_Item* item = NULL;
	Elm_Object_Item* lastItem = NULL;
	Elm_Object_Item* retItem = NULL;
	int count = 0;

	item = elm_genlist_first_item_get( getEvasObj());
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


void CtInputViewControl::__numberItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_NUMBER] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getNumberNumber(record)) {
		CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(deleteItem);
		if (viewItem) {
			__changed = true;
		}
	}

	__inputData->removeNumber(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__emailItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_EMAIL] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getEmailEmail( record ) ) {
		__changed = true;
	}
	__inputData->removeEmail( record );
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__addressItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_ADDRESS] = __getLastItem(deleteItem);

	char address[CT_TEXT_MAX_LEN] = {0,};
	__inputData->getAddressAddress( record, address, sizeof(address));
	if( isChanged || address[0] != '\0') {
		__changed = true;
	}

	__inputData->removeAddress(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__websiteItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_URL] = __getLastItem(deleteItem);

	if( isChanged || __inputData->getWebsiteWebsite( record )) {
		__changed = true;
	}

	__inputData->removeWebsite(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__messengerItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_MESSENGER] = __getLastItem(deleteItem);

	if ( isChanged || __inputData->getMessengerMessenger( record)) {
		__changed = true;
	}

	__inputData->removeMessenger(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__relationItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_RELATIONSHIP] = __getLastItem(deleteItem);

	if (isChanged || __inputData->getRelationshipRelationship(record)) {
		__changed = true;
	}

	__inputData->removeRelationship(record);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__eventItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	__genlistItems[INPUT_EVENT] = __getLastItem(deleteItem);

	if (isChanged || __inputData->getEventEvent(record)) {
		__changed = true;
	}

	__inputData->removeEvent(record);
	__updateSaveBtn();
}

void CtInputViewControl::__noteItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record)
{
	WHIT();
	Evas_Object *noteBtn = elm_object_item_part_content_get(__genlistItems[INPUT_IMPORTANT_FIELDS], "elm.icon.3");
	elm_object_disabled_set(noteBtn, EINA_FALSE);

	if( isChanged || __inputData->getNote(record)) {
		__changed = true;
	}

	__inputData->removeNote( record );
	__genlistItems[INPUT_NOTE] = NULL;
	__updateNoteBtn(false);
	__updateSaveBtn();
	__updateEntryReturnKeyType(deleteItem);
}

void CtInputViewControl::__onGallerySelCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtInputViewControl *control = (CtInputViewControl*)data;

	control->__launchGallery(control);

	control->destroyPopup();
}

void CtInputViewControl::__onCameraSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WPRET_M(NULL == data, "data is NULL");
	CtInputViewControl *control = (CtInputViewControl*)data;

	control->__launchCamera(control);

	control->destroyPopup();
}

void CtInputViewControl::__onRemoveSelCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	CtInputViewControl *control = (CtInputViewControl*)data;
	control->destroyPopup();

	control->__inputData->removeImage(control->__inputData->getImageRecord());
	control->__basicInfoUpdateImage();
}

void CtInputViewControl::__galleryReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WHIT();
	char **pathArray = NULL;
	char *path = NULL;
	int arrayLength = 0;
	CtInputViewControl *control = (CtInputViewControl*)data;
	int i=0;

	app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &pathArray, &arrayLength);
	if (arrayLength > 0 && pathArray[0]) {
		path = strdup(pathArray[0]);
	}
	for(i = 0; i < arrayLength; i++) {
		free(pathArray[i]);
	}
	free(pathArray);


	elm_object_tree_focus_allow_set(control->__view->getNaviframeEvasObj(), EINA_TRUE);

	if (path) {
		WDEBUG("image path %s", path);
		contacts_record_h record = control->__inputData->getImageRecord();
//		control->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_EX_OTHER);
		control->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_OTHER);
		control->__basicInfoUpdateImage();
		free(path);
	}
//	else {
//		elm_object_focus_set(cinput_d->image_focus_btn, EINA_TRUE);
//	}
}

void CtInputViewControl::__launchGallery(CtInputViewControl *control)
{
	WHIT();

	app_control_h service = NULL;
	int ret = 0;

	elm_object_tree_focus_allow_set(control->__view->getNaviframeEvasObj(), EINA_FALSE);

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

	ret = app_control_send_launch_request(service, __galleryReplyCb, control);
	WPWARN(APP_CONTROL_ERROR_NONE != ret, "app_control_send_launch_request() failed(%d)", ret);
	app_control_destroy(service);
}

void CtInputViewControl::__cameraReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WHIT();
	char *path = NULL;
	CtInputViewControl *control = (CtInputViewControl*)data;

	elm_object_tree_focus_allow_set(control->__view->getNaviframeEvasObj(), EINA_TRUE);

	app_control_get_extra_data(reply, APP_CONTROL_DATA_SELECTED, &path);
	if (path && *path) {
		contacts_record_h record = control->__inputData->getImageRecord();
		control->__inputData->setImage(record, path, CONTACTS_IMAGE_TYPE_OTHER);
		control->__basicInfoUpdateImage();
	}
	free(path);
}

void CtInputViewControl::__launchCamera(CtInputViewControl *control)
{
	app_control_h service;
	app_control_create(&service);
	app_control_set_operation(service, APP_CONTROL_OPERATION_CREATE_CONTENT);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	app_control_set_mime(service, "image/jpg");
	app_control_add_extra_data(service, "CALLER", "contacts");

	int err = app_control_send_launch_request(service, __cameraReplyCb, control);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", err);
	app_control_destroy(service);
}

void CtInputViewControl::__updateEntryReturnKeyType(Elm_Object_Item* deleteItem)
{
	WENTER();
	Elm_Object_Item *item = __genlistItems[INPUT_BASIC_INFO];
	Evas_Object* entry = NULL;
	Evas_Object* box = NULL;
	Eina_List* list = NULL;
	Eina_List* cursor = NULL;
	void *tmp = NULL;
	Evas_Object* layout = NULL;

	while (item) {
		if (item == __genlistItems[INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		if (item == deleteItem) {
			item = elm_genlist_item_next_get(item);
			continue;
		}

		if (item == __genlistItems[INPUT_BASIC_INFO]) {
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

	WLEAVE();
}

void CtInputViewControl::__setEntryUnfocus()
{
	Elm_Object_Item *item = __genlistItems[INPUT_BASIC_INFO];
	bool find = false;
	Evas_Object* entry = NULL;
	Evas_Object* box = NULL;
	Eina_List* list = NULL;
	Eina_List* cursor = NULL;
	void *tmp = NULL;
	Evas_Object* layout = NULL;

	while (item) {
		if (item == __genlistItems[INPUT_IMPORTANT_FIELDS]) {
			break;
		}

		if (item == __genlistItems[INPUT_BASIC_INFO]) {
			box = elm_object_item_part_content_get(item, "elm.icon.1");
			if (box) {
				list = elm_box_children_get(box);
				EINA_LIST_FOREACH(list, cursor, tmp) {
					layout = (Evas_Object*)tmp;
					if (layout) {
						entry = elm_object_part_content_get(layout, "elm.icon.1");
						if (entry && elm_object_focus_get(entry)) {
							elm_object_focus_set(entry, EINA_FALSE);
							find = true;
							break;
						}
					}
				}
			}
		}
		else {
			if (true == find) {
				break;
			}

			CtInputViewGenlistItem *viewItem = (CtInputViewGenlistItem *)elm_object_item_data_get(item);
			if (viewItem) {
				entry = viewItem->getEntry();

				if (entry && elm_object_focus_get(entry)) {
					elm_object_focus_set(entry, EINA_FALSE);
					break;
				}
			}
		}
		item = elm_genlist_item_next_get(item);
	}
}

void CtInputViewControl::__updateBasicInfoItem()
{
	WHIT();
	if(__genlistItems[INPUT_BASIC_INFO]) {
		elm_object_item_signal_emit(__genlistItems[INPUT_BASIC_INFO], "image,enable", "");
	}
}

void CtInputViewControl::__createEventDataPopup()
{
	WHIT();
	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_ST_BODY_SET_DATE");
	popup->addButton("IDS_PB_BUTTON_CANCEL", NULL);
	popup->addButton("IDS_PB_SK_SET", [popup, this](bool* destroyPopup) {
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

void CtInputViewControl::__appendEventItem(int year, int month, int day)
{
	WHIT();
	contacts_record_h record = NULL;
	CtInputItems checkExistItem = INPUT_BASIC_INFO;

	record = __inputData->addEvent(year * 10000 + month * 100 + day);

	if (__genlistItems[INPUT_EVENT]) {
		checkExistItem = INPUT_EVENT;
	}
	else if (__genlistItems[INPUT_ADDRESS]) {
		checkExistItem = INPUT_ADDRESS;
	}
	else if (__genlistItems[INPUT_NOTE]) {
		checkExistItem = INPUT_NOTE;
	}
	else if (__genlistItems[INPUT_EMAIL]) {
		checkExistItem = INPUT_EMAIL;
	}
	else if (__genlistItems[INPUT_NUMBER]) {
		checkExistItem = INPUT_NUMBER;
	}
	else {
		checkExistItem = INPUT_BASIC_INFO;
	}
	WDEBUG("INPUT_EVENT item is after %d item", checkExistItem);

	if (!*__birthdayEventExist) {
		if(__inputData->getEventType(record) == CONTACTS_EVENT_TYPE_BIRTH)  {
			*__birthdayEventExist = true;
		}
	}

	CtInputViewEventItem *item = new CtInputViewEventItem(__view, __inputData, record, NULL, __birthdayEventExist,
		[this](){ __itemChanged(); },
		[this](Elm_Object_Item* item, bool isChanged, contacts_record_h record){ __eventItemDeleted( item, isChanged, record ); } );
	Elm_Object_Item* genItem = elm_genlist_item_insert_after(getEvasObj(), item->getItemClassStatic(), item, NULL, __genlistItems[checkExistItem], ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(genItem, ELM_OBJECT_SELECT_MODE_NONE);
	item->setElmObjectItem( genItem );
	__genlistItems[INPUT_EVENT] = genItem;
	elm_genlist_item_show(genItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void CtInputViewControl::__onSipUpCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	CtInputViewControl* control = (CtInputViewControl*) data;
	control->__sipIsShown = true;
}

void CtInputViewControl::__onSipDownCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	WPRET_M(NULL == data, "data is NULL");
	CtInputViewControl* control = (CtInputViewControl*) data;
	control->__sipIsShown = false;
	if (control->__needShowMorePopup) {
		control->__needShowMorePopup = false;
		control->__showMorePopup();
	}
}

void CtInputViewControl::__showMorePopup()
{
	WHIT();
	WPopup* popup = new WPopup();
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle("IDS_PB_OPT_ADD_FIELD");
	popup->setContent([this](Evas_Object* parent)->Evas_Object*
	{
		Elm_Object_Item *genItem = NULL;

		Evas_Object* genlist = elm_genlist_add(parent);
		elm_object_style_set( genlist, "popup" );
		elm_genlist_homogeneous_set(genlist, EINA_TRUE);
		elm_genlist_mode_set( genlist, ELM_LIST_COMPRESS );
		elm_scroller_content_min_limit( genlist, EINA_FALSE, EINA_TRUE ); // Automatically pop-up height is calculated

		CtInputViewBasicInfoItem* basicInfoItem = (CtInputViewBasicInfoItem*)elm_object_item_data_get(__genlistItems[INPUT_BASIC_INFO]);
		WPRET_VM(NULL == basicInfoItem, NULL, "basicInfoItem is NULL");

		for (int i = INPUT_IMPORTANT_ADDRESS; i < INPUT_IMPORTANT_MAX; i++) {
			//WDEBUG("item %d", i);
			if (i == INPUT_IMPORTANT_PHONETIC_INFO) {
				if (basicInfoItem->getAddPhoneticName()) {
					continue;
				}
			} else if (i == INPUT_IMPORTANT_NICKNAME) {
				if (basicInfoItem->getAddNickname()) {
					continue;
				}
			} else if (i == INPUT_IMPORTANT_ORGANIZATION) {
				if (basicInfoItem->getAddOrganization()) {
					continue;
				}
			}

			CtInputViewImportantSubItem *item = new CtInputViewImportantSubItem(i, false);
			genItem = elm_genlist_item_append( genlist, item->getItemClassStatic(), item, NULL, ELM_GENLIST_ITEM_NONE, __importantSubitemSelectedCb, this);
			item->setElmObjectItem( genItem );
		}

		elm_object_content_set(parent, genlist);
		evas_object_show(genlist);

		return genlist;
	});

	attachPopup( popup );
}
