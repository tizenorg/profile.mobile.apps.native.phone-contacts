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

#ifndef _CT_DETAIL_VIEW_H_
#define _CT_DETAIL_VIEW_H_

#include <Ecore.h>
#include <Elementary.h>
#include <contacts.h>

#include "WDefine.h"
#include "WPopup.h"
#include "WNaviframe.h"
#include "WView.h"

#include "ContactsCommon.h"

#include "ICtChangeListener.h"
#include "CtType.h"
#include "CtView.h"

class CtDetailData;
class CtDetailViewBasicInfoControl;

/**
 * @brief View with detailed contact information
 */
class WAPP_ASSIST_EXPORT CtDetailView: public WView, public CtView, public ICtChangeListener
{
public:

	/**
	 * @brief Create "Contact detail" view
	 * @param[in]   personId    Person ID
	 */
	CtDetailView(int personId);

	/**
	 * @see WView::getClassName()
	 */
	virtual const char* getClassName();

	/**
	 * @return ID of person associated with current view
	 */
	int getPersoinId(void);


protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(int eventType);

protected:
	virtual void onLanguageChanged();

private:

	virtual void onDbChanged(const char* viewUri);
	virtual void onCompleteGetContacts(int offset);
	virtual void onConditionChagned();

private:
	typedef enum {
		DETAIL_NUMBER = 0,
		DETAIL_EMAIL,
		DETAIL_ADDRESS,
		DETAIL_MESSENGER,
		DETAIL_WEBSITE,
		DETAIL_RELATIONSHIP,
		DETAIL_EVENT,
		DETAIL_NOTE,
		DETAIL_ADDRBOOK,
		DETAIL_ITEMS_MAX,
	} CtDetailItems;
	void __initialize();
	Evas_Object* __getWindow(void);

	void __setItemClassStyle();
	void __addAllItems();
	void __addItem(CtDetailItems itemType);

	void __addNumberItems();
	void __addEmailItems();
	void __addNoteItems();
	void __addAddressItems();
	void __addEventItems();
	void __addRelationItems();
	void __addWebsiteItems();
	void __addMessengerItems();
	void __addAddressbookItems();
	void __updateGenlist();

	static void __onBasicInfoFavoritesClickedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onNumberCallIconSelectedCb(void *data, Evas_Object *obj, const char *emission, const char *source);
	static void __onNumberMessageIconSelectedCb(void *data, Evas_Object *obj, const char *emission, const char *source);
	static void __onPhonenumberItemSelectedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onEmailItemSelectedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onWebsiteItemSelectedCb(void *data, Evas_Object *obj, void *event_info);

	static void __onEditCb(void *data, Evas_Object *obj, void *event_info);
	static void __onDeleteCb(void *data, Evas_Object *obj, void *event_info);
	static int __deleteContacts(void *data);
	static void __onShareCb(void *data, Evas_Object *obj, void *event_info);

	static void __detailBackBtnCb(void *data, Evas_Object *obj, const char *emission, const char *source);

	void __createBaseInfo();
	void __updateNumberFields();
	int __getHeightByItemCount(Evas_Object* win, int count);

	static void __linkContactReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data);
	static void __launchVideoCall(CtDetailView *p, Eina_List *numberList);

	bool __checkConditionForGenlistHomogeneous(CtDetailData *detailData);
private:
	CtDetailView();
	virtual ~CtDetailView();

private:
	CtDetailData *__detailData;
	CtDetailViewBasicInfoControl *__basicInfo;

	Evas_Object *__mainLayout;
	Evas_Object *__genlist;
	Elm_Object_Item *__genlistItems[DETAIL_ITEMS_MAX];
	Elm_Object_Item *__genlistItemNumberFirst;
	Elm_Object_Item *__genlistItemNumberLast;

	int __appendIndex;
	bool __checkStatus;
	Elm_Object_Item* __emailItem;
	bool __needHomogeneous;
};

#endif /* _CT_DETAIL_VIEW_H_ */
