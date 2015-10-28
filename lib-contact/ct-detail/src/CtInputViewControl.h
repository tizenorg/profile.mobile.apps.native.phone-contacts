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

#ifndef _CT_INPUT_VIEW_CONTROL_H_
#define _CT_INPUT_VIEW_CONTROL_H_

#include <Elementary.h>

#include <contacts.h>
#include <system_settings.h>

#include "WNaviframe.h"
#include "WPopup.h"

#include "ContactsCommon.h"

#include "ICtChangeListener.h"
#include "CtType.h"
#include "CtView.h"
#include "CtData.h"


class CtInputData;
class CtInputView;

class CtInputViewControl: public WControl
{
public:
	CtInputViewControl( std::shared_ptr<CtData>& inputData, CtInputView* view );

public:
	int getAddedIndex() { return __addedIndex; }

	bool getIsCreate();
	bool getIsEmpty();
	bool getIsChanged();
	void setIsDestroy(bool isDestroy);
	Evas_Object* getViewEvasObj();
	Evas_Object* getNaviframeEvasObj();

	void showTopItem();
	void basicInfoItemShowCreateKeypad( Evas_Object* obj);

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();

private:
	void __initialize();
	void __addAllItems(Evas_Object* genlist);
	static void __genlistRealizedCb(void *data, Evas_Object *obj, void *event_info);
	static void __genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info);


	static void __importantSubitemSelectedCb(void* data, Evas_Object* obj, void* event_info);
	void __addNumberClickedCb();
	void __addEmailClickedCb();
	void __addNoteClickedCb();
	void __addMoreClickedCb();

	static void __onGallerySelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onCameraSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onRemoveSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __galleryReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	static void __launchGallery(CtInputViewControl *control);
	static void __cameraReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	static void __launchCamera(CtInputViewControl *control);

	void __numberItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __emailItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __messengerItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __addressItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __websiteItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __relationItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __eventItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);
	void __noteItemDeleted(Elm_Object_Item* deleteItem, bool isChanged, contacts_record_h record);

	bool __checkIsChanged();
	bool __checkIsEmpty();
	void __updateSaveBtn();
	void __updateNoteBtn(bool isNoteInput);

	void __itemChanged();
	void __basicInfoItemImageClicked(Evas_Object* obj);
	void __basicInfoItemDeleted( int deletedInfo);
	void __activateNextEntry(Evas_Object* obj, Elm_Object_Item* item);
	void __basicInfoExpandClickedCb();
	void __basicInfoUpdateImage();
	Elm_Object_Item* __getLastItem(Elm_Object_Item* deleteItem);
	void __updateEntryReturnKeyType(Elm_Object_Item* deleteItem);
	void __setEntryUnfocus();

	static void __regionFormatChangedCb(system_settings_key_e key, void *data);
	void __updateBasicInfoItem();
	void __createEventDataPopup();
	void __appendEventItem(int year, int month, int day);
	static void __onSipUpCb(void *data, Evas_Object *obj, void *event_info);
	static void __onSipDownCb(void *data, Evas_Object *obj, void *event_info);
	void __showMorePopup();

private:
	CtInputViewControl();
	virtual ~CtInputViewControl();

private:

	typedef enum {
		INPUT_BASIC_INFO = 0,
		INPUT_NUMBER,
		INPUT_EMAIL,
		INPUT_NOTE,
		INPUT_ADDRESS,

		INPUT_EVENT,
		INPUT_RELATIONSHIP,
		INPUT_URL,
		INPUT_MESSENGER,

		INPUT_IMPORTANT_FIELDS,
		INPUT_IMPORTANT_SUBITEM,
		INPUT_ITEMS_MAX,
	}CtInputItems;

	typedef enum {
		INPUT_IMPORTANT_NONE = 0,
		INPUT_IMPORTANT_ADDRESS,
		INPUT_IMPORTANT_EVENT,
		INPUT_IMPORTANT_NICKNAME,
		INPUT_IMPORTANT_RELATIONSHIP,
		INPUT_IMPORTANT_ORGANIZATION,
		INPUT_IMPORTANT_URL,
		INPUT_IMPORTANT_MESSENGER,
		INPUT_IMPORTANT_PHONETIC_INFO,
		INPUT_IMPORTANT_MAX,
	}CtInputSubItems;

	Evas_Object *__nameEntry;
	Evas_Object* __genlist;
	Evas_Object* __conformant;

	bool __changed;
	bool __isDestroy;
	bool __needResetTheme;
	bool __needShowMorePopup;
	bool __sipIsShown;

	int __addedIndex;

	Elm_Object_Item *__genlistItems[INPUT_ITEMS_MAX];

	//
	std::shared_ptr<bool> __birthdayEventExist;

	std::shared_ptr<CtData> __inputData;
	CtInputView* __view;

	Ecore_Timer* __keypadTimer;
};

#endif /* _CT_INPUT_VIEW_H_ */
