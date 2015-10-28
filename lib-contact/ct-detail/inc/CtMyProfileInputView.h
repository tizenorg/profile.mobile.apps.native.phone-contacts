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

#ifndef _CT_MYPROFILE_INPUT_VIEW_H_
#define _CT_MYPROFILE_INPUT_VIEW_H_

#include <Elementary.h>

#include <contacts.h>

#include "WNaviframe.h"
#include "WPopup.h"
#include "WView.h"

#include "ContactsCommon.h"

#include "ICtChangeListener.h"
#include "CtType.h"
#include "CtView.h"

class CtData;

/**
 * @brief View for creating or editing my profile
 */
class WAPP_ASSIST_EXPORT CtMyProfileInputView: public WView, public CtView, public ICtChangeListener
{
public:
	/**
	 * @brief Create input view by @a contactRecord for my profile
	 * @param[in]    contactRecord    Record with contact information
	 */
	CtMyProfileInputView(contacts_record_h contactRecord);

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(int eventType);
	virtual bool onPop();

private:

	virtual void onDbChanged(const char* viewUri);
	virtual void onCompleteGetContacts(int offset);
	virtual void onConditionChagned();

protected:
	virtual void onLanguageChanged();

private:
	typedef struct {
		CtMyProfileInputView *view;
		contacts_record_h record;
		Elm_Object_Item *item;
		const char *view_uri;
		bool isChanged;
		char *oldVal;
	}CtMyProfileInputViewItem;

	void __initialize();
	void __addAllItems();
	void __addItem(int itemType);

	Evas_Object* __getWindowEvasObj(void);
	Evas_Object* __getNaviframeEvasObj(void);

	static void __onGallerySelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onCameraSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onRemoveSelCb(void *data, Evas_Object *obj, void *event_info);

	static void __importantSubitemSelectedCb(void* data, Evas_Object* obj, void* event_info);
	void __addNumberClickedCb();
	void __addEmailClickedCb();
	void __addNoteClickedCb();
	void __addMoreClickedCb();
	bool __checkIsEmpty();
	bool __checkIsChanged();
	void __updateSaveBtn();
	void __updateNoteBtn(bool isNoteInput);
	void __save();


	static void __saveCb(void *data, Evas_Object *obj, void *event_info);
	static void __cancelCb(void *data, Evas_Object *obj, void *event_info);
	static void __genlistRealizedCb(void *data, Evas_Object *obj, void *event_info);
	static void __genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info);

	static void __galleryReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	static void __launchGallery(CtMyProfileInputView *view);

	static void __cameraReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	static void __launchCamera(CtMyProfileInputView *view);

	void __itemChanged();
	void __basicInfoItemImageClicked(Evas_Object* obj);
	void __basicInfoItemDeleted( int deletedInfo);
	void __basicInfoItemShowCreateKeypad( Evas_Object* obj);
	void __basicInfoExpandClickedCb();
	void __basicInfoUpdateImage();

	Elm_Object_Item* __getLastItem(Elm_Object_Item* deleteItem);
	void __numberItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __emailItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __addressItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __messengerItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __websiteItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __eventItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __noteItemDeleted( Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __activateNextEntry(Evas_Object* obj, Elm_Object_Item* item);
	void __updateEntryReturnKeyType(Elm_Object_Item* deleteItem);
	void __updateBasicInfoItem();
	void __createEventDataPopup();
	void __appendEventItem(int year, int month, int day);

private:
	CtMyProfileInputView();
	virtual ~CtMyProfileInputView();

private:

	typedef enum {
		MYPROFILE_INPUT_BASIC_INFO = 0,
		MYPROFILE_INPUT_MSISDN,
		MYPROFILE_INPUT_NUMBER,
		MYPROFILE_INPUT_EMAIL,
		MYPROFILE_INPUT_NOTE,

		MYPROFILE_INPUT_ADDRESS,
		MYPROFILE_INPUT_EVENT,
		MYPROFILE_INPUT_URL,
		MYPROFILE_INPUT_MESSENGER,

		MYPROFILE_INPUT_IMPORTANT_FIELDS,
		MYPROFILE_INPUT_IMPORTANT_SUBITEM,
		MYPROFILE_INPUT_ITEMS_MAX,
	}CtMyProfileInputItems;

	typedef enum {
		MYPROFILE_INPUT_IMPORTANT_ADDRESS = 1,
		MYPROFILE_INPUT_IMPORTANT_EVENT,
		MYPROFILE_INPUT_IMPORTANT_NICKNAME,
		MYPROFILE_INPUT_IMPORTANT_ORGANIZATION,
		MYPROFILE_INPUT_IMPORTANT_URL,
		MYPROFILE_INPUT_IMPORTANT_MESSENGER,
		MYPROFILE_INPUT_IMPORTANT_PHONETIC_INFO,
		MYPROFILE_INPUT_IMPORTANT_MAX,
	}CtMyProfileInputSubItems;

	std::shared_ptr<bool> __birthdayEventExist;
	std::shared_ptr<CtData> __inputData;

	int __contactId;
	WPopup* __popup;
	Evas_Object *__genlist;
	Evas_Object *__ctxpopup;
	Evas_Object *__customEntry;
	int __savedAddressbookId;

	bool __saved;
	bool __discard;
	bool __changed;

	int __appendIndex;
	int __addedIndex;

	Elm_Genlist_Item_Class __inputItc[MYPROFILE_INPUT_ITEMS_MAX];
	Elm_Object_Item *__genlistItems[MYPROFILE_INPUT_ITEMS_MAX];

	Evas_Object *__nameEntry;
	char *__basicInfoFullName;
	Evas_Object* __saveBtn;

	bool __baseInfoExpanded;
	bool __isCreate;
};

#endif /* _CT_MYPROFILE_INPUT_VIEW_H_ */
