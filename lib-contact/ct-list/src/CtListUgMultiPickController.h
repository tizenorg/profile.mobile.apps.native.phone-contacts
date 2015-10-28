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

#ifndef __CT_LIST_UG_MULTI_PICK_CONTROLLER_H_
#define __CT_LIST_UG_MULTI_PICK_CONTROLLER_H_

#include "WPopup.h"

#include "ContactsCommon.h"
#include "CtListView.h"
#include "CtType.h"
#include "CtListController.h"
#include "CtListDataPerson.h"


class CtListView;
class CtListModelDb;
class CtProgressController;
class CtListUgMultiPickController : public CtListController
{
public:
	CtListUgMultiPickController(CtListResultType resultType, CtListView *listView, CtListModelDb* listData);
	CtListUgMultiPickController(CtListCtrlType ctrlType, CtListView *listView, CtListModelDb* listData);
	virtual ~CtListUgMultiPickController();

public:
	virtual void onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectDone();
	virtual void onSelectBack();
	virtual void onSelectCancel();
	virtual void onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectMenu() {}


private:
	typedef struct {
		CtListUgMultiPickController *controller;

		contacts_record_h record;
		Elm_Object_Item *popupItem;
		int dataType;
	}CtListPopupItemData;


private:
	void __doReplyCheckedInfo();
	bool __checkLimitCount(int checkCount, int limitCount);
	bool __checkMaxLinkCount(const int currentContactLinkedCount, const int checkedContactLinkedCount);
	bool __checkItem(Elm_Object_Item *item, CtListView *view, CtCheckState checkState);
	void __vcardReplyCheckedInfo();
	void __sendVcardResult(CtProgressController &controller);


private:
	//for check for data
	static void __onSelectDataPopupItem(void *data, Evas_Object *obj, void *event_info);

	void __updateTitleAndDone();
	bool __changeCheckState(Elm_Object_Item *item, CtCheckState checkState, bool updateTitleAndDone);
	static CtCheckState __getCheckState(Elm_Object_Item *item);
	Evas_Object* __createDataListPopup(Elm_Object_Item *item, int dataType);
	void __setCheckedInfo(Elm_Object_Item *item, contacts_record_h record, CtListItemDataType dataType);
private:
	CtListUgMultiPickController();
private:
	CtListView* __listView;
	CtListModelDb* __listModel;
	CtListCtrlType __ctrlType;
	//use for child data copy
	CtListDataPerson *__selectedPersonData;
	int __linkedCount;
};

#endif //__CT_LIST_UG_MULTI_PICK_CONTROLLER_H_

