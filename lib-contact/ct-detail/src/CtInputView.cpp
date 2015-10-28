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

#include <app_preference.h>
#include <contacts.h>

#include "WPopup.h"
#include "WToastPopup.h"

#include "CommonPath.h"
#include "ContactsDebug.h"
#include "ContactsAppControl.h"

#include "CtString.h"
#include "CtPath.h"
#include "CtInputData.h"
#include "CtInputView.h"
#include "CtCommon.h"
#include "CtDetailView.h"
#include "CtInputViewControl.h"
#include "CtRegistryMgr.h"
#include "CtThemeMgr.h"

#define __ITEM_HEIGHT 98

CtInputView::CtInputView(contacts_record_h contactRecord)
{
	WHIT();
	__inputData = std::shared_ptr<CtData>( new CtInputData(contactRecord) );
	__initialize();
}

CtInputView::CtInputView(int personId, const char* number, const char* email, const char* url)
{
	WHIT();
	__inputData = std::shared_ptr<CtData>( new CtInputData(personId, number, email, url) );
	__initialize();
}

CtInputView::CtInputView(const char* name, const char* number, const char* email, const char* url)
{
	__inputData = std::shared_ptr<CtData>( new CtInputData(name, number, email, url) );
	__initialize();
}

CtInputView::~CtInputView()
{
	WHIT();

	destroyPopup();
	if (__windowVisibilityChangeHandler) {
		ecore_event_handler_del(__windowVisibilityChangeHandler);
		__windowVisibilityChangeHandler = NULL;
	}
	if(__popupIdler)
		ecore_idler_del(__popupIdler);
	auto sp = __inputControl.lock();
	if ( sp ) {
		CtInputViewControl* control = static_cast<CtInputViewControl*>(sp.get());
		control->setIsDestroy(true);
	}
}

Evas_Object* CtInputView::getNaviframeEvasObj()
{
	if( getNaviframe() )
		return getNaviframe()->getEvasObj();
	else
		return NULL;
}

Evas_Object* CtInputView::getWindowEvasObj()
{
	if(getWindow()) {
		return getWindow()->getEvasObj();
	}
	else {
		WASSERT_EX(1, "window null");
	}
	return NULL;
}

Evas_Object* CtInputView::getConformantEvasObject()
{
	if(getWindow()) {
		return getWindow()->getConformantEvasObj();
	}
	else {
		WASSERT_EX(1, "window null");
	}
	return NULL;
}

void CtInputView::setOnResult(const std::function<void(int, CtResultType)>& handlerFunc)
{
	__resultHandler = handlerFunc;
}

void CtInputView::__initialize()
{

	__saveBtn = NULL;
	__saved = false;
	__discard = false;
	__changed = false;
	__popupIdler = NULL;
	__windowVisibilityChangeHandler = NULL;

	__savedAddressbookId = __inputData->getAddressbookId();
	__isCreate = __inputData->getContactId() > 0 ? false : true;
}

void CtInputView::onDbChanged(const char* viewUri)
{
	WENTER();
	if (__inputData->checkChangedTypeIsDeleted()) {
		WDEBUG("Contact is deleted");
		__discard = true;
		popOut();
	}
}

void CtInputView::onCompleteGetContacts(int offset)
{
	WENTER();
}

void CtInputView::onConditionChagned()
{
	WENTER();
}

Evas_Object* CtInputView::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();
	Evas_Object *base = createBaseLayout(parent);
	__baseLayout = base;

	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	std::string buttonStylesEdjePath = ContactsCommon::getResourcePath(COMMON_BUTTONS_EDJ);
	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->addExtensionTheme(buttonStylesEdjePath.c_str());
	CtThemeMgr::getInstance()->setObjectTheme(__baseLayout);

	if (__isCreate)
	{
		setTitle(V_("IDS_PB_BUTTON_CREATE_ABB2"));
	}
	else
	{
		setTitle(V_("IDS_PB_OPT_EDIT"));
	}
	return base;
}

void CtInputView::onLanguageChanged()
{
	WHIT();

	destroyPopup();

	auto sp = __inputControl.lock();
	if (sp) {
		CtInputViewControl* control = static_cast<CtInputViewControl*>(sp.get());
		control->destroyPopup();
	}

	elm_object_text_set(__saveBtn, V_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));

	Evas_Object* cancelBtn = elm_object_item_part_content_get(getNaviItem(), "title_left_btn");
	elm_object_text_set(cancelBtn, V_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));

	if (__isCreate) {
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", V_("IDS_PB_BUTTON_CREATE_ABB2"));
	}
	else {
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", V_("IDS_PB_OPT_EDIT"));
	}

	Evas_Object* genlist = elm_object_part_content_get(getEvasObj(), "elm.swallow.content");
	if(genlist)
		elm_genlist_realized_items_update(genlist);
}

void CtInputView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();
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

	createInputViewControl(__baseLayout);
	__inputData->attachListener(this);

	WLEAVE();
}

void CtInputView::createInputViewControl(Evas_Object *base)
{
	WHIT();

	auto sp = __inputControl.lock();
	if ( sp ) {
		CtInputViewControl* control = static_cast<CtInputViewControl*>(sp.get());
		control->setIsDestroy(true);
	}

	Evas_Object* genlist = elm_object_part_content_unset(base, "elm.swallow.content");
	if (genlist) {
		evas_object_del(genlist);
	}

	CtInputViewControl* control = new CtInputViewControl( __inputData, this );
	__inputControl = control->getWeakPtr();
	control->create( base, NULL);
	elm_object_part_content_set( base, "elm.swallow.content", control->getEvasObj() );
}

void CtInputView::updateSaveBtn(Eina_Bool empty)
{
	WHIT();
	elm_object_disabled_set(__saveBtn, empty);
	WPRET_M(true == empty, "all fields are empty, can't save");
}

void CtInputView::__createToastPopup(const char* msg)
{
	WToastPopup* popup = new WToastPopup( msg );
	popup->create( getWindowEvasObj(), NULL);
}

int CtInputView::__getPersonIdByContactId(int contactId)
{
	int personId = 0;
	contacts_record_h record = NULL;
	int err = CONTACTS_ERROR_NONE;

	err = contacts_db_get_record(_contacts_contact._uri, contactId, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_db_get_record() Failed(%d)", err);

	err = contacts_record_get_int(record, _contacts_contact.person_id, &personId);
	if (CONTACTS_ERROR_NONE != err) {
		contacts_record_destroy(record, true);
		WDEBUG("contacts_record_get_int() Failed(%d)", err);

		return 0;
	}
	contacts_record_destroy(record, true);

	return personId;
}

void CtInputView::__saveCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtInputView *view = (CtInputView*)data;

	int err = CONTACTS_ERROR_NONE;
	int contactId = 0;
	const char* addressbookName = view->__inputData->getAddressbookName();

	// add contact
	if (view->__isCreate) {
		WDEBUG("addressbook_name %s", addressbookName);
		if(addressbookName) {
			int groupId = ctCommonGetEasDefaultGroupId(view->__inputData->getAddressbookId());
			if (groupId > 0) { // EAS default group
				// In the case of EAS contacts, the contacts is required to add the default EAS contact group
				view->__inputData->addGroupRelation(groupId);
			}

			CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_INSERTED_PERSON, 1);
			err = view->__inputData->insertContact(&contactId);
			if(CONTACTS_ERROR_NONE == err)
				CtRegistryMgr::getInstance()->setInt(REGISTRY_KEY_INSERTED_PERSON_ID, contactId);

			if (CONTACTS_ERROR_NONE != err) {
				WERROR("insert failed %d", err);
				if(CONTACTS_ERROR_FILE_NO_SPACE == err)
					view->__createToastPopup(V_("IDS_PB_POP_NOT_ENOUGH_MEMORY"));
				else
					view->__createToastPopup(V_("IDS_PB_POP_FAILED"));
			} else {
				char* name = ctCommonGetAddressBookName(view->__inputData->getAddressbookId());
				WDEBUG("name = %s", name);
				int ret = preference_set_string(CT_KEY_INPUT_BASE_ADDRESSBOOK, name);
				WPWARN(ret, "preference_set_string(%d) Failed", ret);
				SAFE_FREE(name);
			}
		}
	} else {
		if(addressbookName) {
			err = view->__inputData->updateContact();
		}
		contactId = view->__inputData->getContactId();
	}

	int personId = view->__getPersonIdByContactId(contactId);
	if(view->__resultHandler) {
		view->__resultHandler(personId,
				CONTACTS_ERROR_NONE == err ? RESULT_SUCCESS : RESULT_FAILED);
	}
	view->__saved = true;

	if (view->__isCreate) {
		view->getNaviframe()->push(new CtDetailView(personId));
		view->destroy();
	} else {
		view->popOut();
	}
}

void CtInputView::__cancelCb(void *data, Evas_Object *obj, void *event_info)
{
	CtInputView *view = (CtInputView*)data;
	if (view->__resultHandler) {
		view->__resultHandler(0, RESULT_CANCELLED);
	}

	view->popOut();
}

void CtInputView::onTabSelect(Elm_Object_Item *naviItem)
{
	WHIT();

	elm_object_focus_set(getEvasObj(), EINA_TRUE);
	destroyPopup();

}
void CtInputView::onTabUnselect(Elm_Object_Item *naviItem)
{
	WHIT();
	destroyPopup();
}

bool CtInputView::onTabPop()
{
	WHIT();
	return TRUE;
}

void CtInputView::onDestroy()
{
	WHIT();
}

void CtInputView::onEvent(int eventType)
{
	WHIT();
}

bool CtInputView::onPop()
{
	bool empty = false;
	bool isChanged = false;
	CtInputViewControl* control = NULL;

	auto sp = __inputControl.lock();
	if( sp ) {
		control = static_cast<CtInputViewControl*>(sp.get());
		empty = control->getIsEmpty();
		isChanged = control->getIsChanged();
	}
	if(!__saved) {
		WDEBUG("canceled");
		if (__discard) {
			return true;
		}

		if (__isCreate && empty) {
			return true;
		}

		Eina_Bool changed = isChanged;
		if (!__isCreate) { // edit view
			if (__changed) {
				changed = EINA_TRUE;
			}
		} else {
			// in create view, if empty is false, so changed should be true
			if (!empty) {
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
				__discard = true;
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
