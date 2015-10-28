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

#ifndef __CT_LIST_UG_SINGLE_PICK_CONTROLLER_H_
#define __CT_LIST_UG_SINGLE_PICK_CONTROLLER_H_

#include "WPopup.h"

#include "ContactsCommon.h"
#include "CtListView.h"
#include "CtType.h"
#include "CtListController.h"

class CtListView;
class CtListModelDb;
class CtListUgSinglePickController : public CtListController
{
public:
	CtListUgSinglePickController(CtListResultType resultType, CtListView *listView, CtListModelDb* listData);
	virtual ~CtListUgSinglePickController();

public:
	virtual void onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectCreate();
	virtual void onSelectDone();
	virtual void onSelectBack();
	virtual void onSelectCancel();
	virtual void onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectMenu() {}

private:
	void __doReplySelectedInfo(CtListDataPerson *personData);
	Evas_Object* __createDataListPopup(CtListDataPerson *personData, int dataType);
	Evas_Object* __createNumberPopup(CtListDataPerson *personData);
	static void __onSelectDataPopupItem(void *data, Evas_Object *obj, void *event_info);
	static Eina_Bool __doReplyIdle(void* data);
private:
	CtListUgSinglePickController();

private:
	typedef struct {
		CtListUgSinglePickController *controller;

		contacts_record_h record;
		Elm_Object_Item *popupItem;
		int dataType;
	}CtListPopupItemData;

private:
	CtListView* __listView;
	CtListModelDb* __listModel;
	CtListResultType __resultType;

	CtListDataPerson *__selectedPersonData;
	Ecore_Idler *__idler;
};

#endif //__CT_LIST_UG_SINGLE_PICK_CONTROLLER_H_

