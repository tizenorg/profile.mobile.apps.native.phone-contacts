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

#ifndef __CT_LIST_UG_VCARD_VIEW_CONTROLLER_H_
#define __CT_LIST_UG_VCARD_VIEW_CONTROLLER_H_

#include <string>
#include <app.h>
#include "WPopup.h"

#include "ContactsCommon.h"
#include "CtListView.h"
#include "CtType.h"
#include "CtListController.h"



class CtListView;
class CtListModelVcard;
class CtProgressController;

class CtListUgVcardViewController : public CtListController
{
public:
	CtListUgVcardViewController(const std::string& vcardPath, CtListView *listView, CtListModelVcard* listData);
	virtual ~CtListUgVcardViewController();

public:
	virtual void onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectDone();
	virtual void onSelectCancel();
	virtual void onSelectBack();
	virtual void onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState);
	virtual void onSelectMenu() {}

	static void onImportFinished(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data);
private:
	CtListUgVcardViewController();
	void __getVcardPathFromUri(void);
	void __finishImportCb(CtProgressController &controller);

private:
	std::string __vcardPath;
	CtListView* __listView;
	CtListModelVcard* __listModel;
	bool __showLoadingPopup;
};

#endif //__CT_LIST_UG_VCARD_VIEW_CONTROLLER_H_

