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

#ifndef _PH_LOG_DETAIL_CHECK_VIEW_H_
#define _PH_LOG_DETAIL_CHECK_VIEW_H_

#include "ContactsCommon.h"

#include "PhLogListView.h"

class WAPP_ASSIST_EXPORT PhLogDetailCheckView : public PhLogListView
{
public:
	PhLogDetailCheckView(std::string number, std::string callDesc);

protected:
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onSetAsPageContent(Elm_Object_Item* parentNaviItem);
	virtual	Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onCreated(void);
private:
	virtual void onDbChanged();

	virtual void updateList(void);

	void __enableTitleBtn(Elm_Object_Item* naviItem);

	void __setGenlistItcs();

	void __setList(void);

	void __initializeTitleInfo();
	void __updateTitleInfo();

	static void __checkAllItems(PhLogGenlistHelper* glHelper);
	static void __uncheckAllItems(PhLogGenlistHelper* glHelper);
	static void __checkItem(PhLogGenlistHelper::CheckedItemData * itemData);
	static void __uncheckItem(PhLogGenlistHelper::CheckedItemData * itemData);
	static void __checkAllClickedCb(void *data, Evas_Object *obj, void *event_info);
	static void __selectItemCb(void *data, Evas_Object *obj, void *event_info);

private:
	virtual ~PhLogDetailCheckView();

private:
	Elm_Genlist_Item_Class *__itcGroupIndex;
	Elm_Genlist_Item_Class *__itcDeleteList;
	Elm_Genlist_Item_Class *__itcSelectAll;
	std::list<PhLogDbMgr::RecordInfo*>* __logList;
	Evas_Object *__checkAllButton;
	Evas_Object *__deleteButton;
	int __checkedCount;
	int __totalCount;
	bool __enableTitleButton;
	bool __enableCheckAllButton;
	std::list<PhLogDbMgr::RecordInfo*> *__checkedList;
	std::string __number;
	std::string __callDesc;
};

#endif /* _PH_LOG_DETAIL_CHECK_VIEW_H_ */
