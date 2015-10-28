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

#ifndef PHLOGDETAILVIEW_H_
#define PHLOGDETAILVIEW_H_

#include "PhLogDbMgr.h"
#include "PhLogListView.h"
#include "PhLogDetailContactInfoControl.h"
#include "PhLogDetailButtonInfoControl.h"

class PhLogDetailView : public PhLogListView
{
public:
	PhLogDetailView(const PhLogDbMgr::RecordInfo *recordInfo);

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onCreated();
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);

private:
	PhLogDetailView();

	void __setMoreBtn(Elm_Object_Item *naviItem);
	void __setContactInfo();
	void __setGenlistItcs();
	void __setList();

	//from PhLogListView
	virtual void updateList(void);

	virtual void onDbChanged();
	static void __detailBackBtnCb(void *data, Evas_Object *obj, const char *emission, const char *source);

private:
	virtual ~PhLogDetailView();

private:
	std::string __number;
	std::string __callDesc;

	Evas_Object* __mainLayout;

	Elm_Genlist_Item_Class *__itcLogInfo;
	Elm_Genlist_Item_Class *__itcGroupIndex;
	Elm_Genlist_Item_Class *__itcHistoryList;

	std::list<PhLogDbMgr::RecordInfo*> *__detailList;

	PhLogDetailContactInfoControl *__contactInfo;

	const PhLogDbMgr::RecordInfo *__recordInfo;
};

#endif /* PHLOGDETAILVIEW_H_ */
