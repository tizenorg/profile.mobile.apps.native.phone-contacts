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

#ifndef __CT_LIST_BASIC_CONTROLLER_H_
#define __CT_LIST_BASIC_CONTROLLER_H_

#include "WPopup.h"
#include "ContactsCommon.h"
#include "CtType.h"
#include "CtListController.h"
#include "CtProgressBarPopup.h"
#include "CtListDataPerson.h"

namespace std {
class thread;
}

class CtThread;
class CtListView;
class CtListModelDb;
class CtProgressController;
class CtListBasicController : public CtListController
{
public:
	CtListBasicController(CtListCtrlType ctrlType, CtListView *listView, CtListModelDb* listModel);
	CtListBasicController();
	virtual ~CtListBasicController();

public:
	virtual void onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectCreate();
	virtual void onSelectDone();
	virtual void onSelectBack();
	virtual void onSelectCancel();
	virtual void onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectMenu();

private:
	int __setImage(contacts_record_h record, char *image);
	int __removeImage(contacts_record_h contactRecord, contacts_record_h imageRecord);
	bool __setFavoriteImage(int personId, bool set, contacts_record_h contact_record);
	void __doDeleteContacts(void);
	void __doShareContacts();
	void __doFavoritesContacts(bool addFirstOne);
	bool __getHasReadOnlyMode(char* addressbookIDs);
	void __doSetFavoritesOrder();
	void __moreManageFavoritesItemOperation();

	std::string __makeCheckedDataPath(int personId);

private:
	static void __onAddFavoritesSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onRemoveFavoritesSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onReorderFavoritesSelCb(void *data, Evas_Object *obj, void *event_info);
	static void __onPreferenceChangedCb(const char *key, void *data);

	//for check for data
	bool __changeCheckState(Elm_Object_Item *item, bool checkState);
	static bool __getCheckState(Elm_Object_Item *item);


	static void __onDoFavouritesThreadRunCb(void *data);
	static void __onDoFavouritesThreadEndCb(void *data);
	static void __onDoDeleteThreadRunCb(void *data);
	static void __onDoDeleteThreadEndCb(void* data);


private:
	CtListView* __listView;
	CtListModelDb* __listModel;
	CtListCtrlType __ctrlType;
	static bool __stopDoAsyncAction;
	CtThread *__doActionThread;

	std::unique_ptr<std::vector<std::shared_ptr<CtListDataPerson>>> __checkedData;

	CtProgressBarPopup* __progressPopup;
	WUiObjectPtr __popupMonitor;

	int __updateDataType;
	bool __canceled;
	void* __settingDllHandle;
	void* __detailDllHandle;
	void* __cmDllHandle;
};

#endif //__CT_LIST_BASIC_CONTROLLER_H_

