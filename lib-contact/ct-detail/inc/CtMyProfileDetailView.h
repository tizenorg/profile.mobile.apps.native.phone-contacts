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

#ifndef _CT_MYPROFILE_DETAIL_VIEW_H_
#define _CT_MYPROFILE_DETAIL_VIEW_H_

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

class CtMyProfileDetailData;

/**
 * @brief View with detail information about my profile
 */
class WAPP_ASSIST_EXPORT CtMyProfileDetailView: public WView, public CtView, public ICtChangeListener
{
public:
	CtMyProfileDetailView();

	/**
	 * @see WView::getClassName()
	 */
	virtual const char* getClassName();

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(int eventType);
	virtual void onLanguageChanged();
	virtual void onDbChanged(const char* viewUri);
	virtual void onCompleteGetContacts(int offset);
	virtual void onConditionChagned();

private:
	typedef enum {
		MYPROFILE_DETAIL_MSISDN = 0,
		MYPROFILE_DETAIL_NUMBER,
		MYPROFILE_DETAIL_EMAIL,
		MYPROFILE_DETAIL_NOTE,
		MYPROFILE_DETAIL_ADDRESS,
		MYPROFILE_DETAIL_EVENT,
		MYPROFILE_DETAIL_RELATIONSHIP,
		MYPROFILE_DETAIL_WEBSITE,
		MYPROFILE_DETAIL_MESSENGER,
		MYPROFILE_DETAIL_ITEMS_MAX,
	} CtMyProfileDetailItems;
	void __initialize();
	Evas_Object* __getWindow(void);

	void __setItemClassStyle();
	void __addAllItems();
	void __addItem(int itemType);

	void __createBaseInfo();

	void __addMsisdnItem();
	void __addNumberItems();
	void __addEmailItems();
	void __addNoteItems();
	void __addAddressItems();
	void __addEventItems();
	void __addRelationItems();
	void __addWebsiteItems();
	void __addMessengerItems();

	static void __basicInfoBackBtnCb(void *data, Evas_Object *obj, const char *emission, const char *source);
	static void __onShareCb(void *data, Evas_Object *obj, void *event_info);
	static void __onDeleteCb(void *data, Evas_Object *obj, void *event_info);
	bool __isContactSyncToAnyAccount(contacts_record_h personRecord);
	int __deleteContacts(void *data);

	void __updateGenlist();

	int getPersonId(void);

	virtual ~CtMyProfileDetailView();

private:
	CtMyProfileDetailData *__detailData;

	Evas_Object *__mainLayout;
	Evas_Object *__genlist;
	Elm_Object_Item *__genlistItems[MYPROFILE_DETAIL_ITEMS_MAX];
};

#endif /* _CT_MYPROFILE_DETAIL_VIEW_H_ */
