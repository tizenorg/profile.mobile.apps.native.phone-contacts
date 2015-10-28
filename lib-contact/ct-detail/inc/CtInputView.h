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

#ifndef _CT_INPUT_VIEW_H_
#define _CT_INPUT_VIEW_H_

#include <Elementary.h>

#include <contacts.h>

#include "WNaviframe.h"
#include "WPopup.h"
#include "WView.h"

#include "ContactsCommon.h"

#include "IContactsTabPageView.h"
#include "ICtChangeListener.h"
#include "CtType.h"
#include "CtView.h"

class CtData;
class CtInputViewControl;

/**
 * @brief "Create new contact" view
 */
class WAPP_ASSIST_EXPORT CtInputView: public CtView, public IContactsTabPageView, public ICtChangeListener
{
public:
	/**
	 * @brief Create input view by @a contactRecord
	 * @param[in]    contactRecord    Record with contact information
	 */
	CtInputView(contacts_record_h contactRecord);

	/**
	 * @brief Create input view
	 * @param[in]    personId    Person ID
	 * @param[in]    number      Phone number
	 * @param[in]    email       Contact's email
	 * @param[in]    uri         Contact's web site
	 */
	CtInputView(int personId, const char* number, const char* email, const char* url);

	/**
	 * @brief Create input view
	 * @param[in]    name        Contact's name
	 * @param[in]    number      Phone number
	 * @param[in]    email       Contact's email
	 * @param[in]    uri         Contact's web site
	 */
	CtInputView(const char* name, const char* number, const char* email, const char* url);

public:

	/**
	 * @return true if it is "Create view", false if "Edit view"
	 */
	bool getIsCreate() { return __isCreate; }

	/**
	 * @return Evas_Object of naviframe on success, otherwise NULL
	 */
	Evas_Object* getNaviframeEvasObj();

	/**
	 * @return Evas_Object of window on success, otherwise NULL
	 */
	Evas_Object* getWindowEvasObj();

	/**
	 * @return Evas_Object of conformant on success, otherwise NULL
	 */
	Evas_Object* getConformantEvasObject();

	/**
	 * @brief Set function on view completion
	 * @param[in]    handlerFunc    Function that invokes on view closure
	 */
	void setOnResult(const std::function<void(int, CtResultType)>& handlerFunc);

	/**
	 * @brief Create view controller
	 * @param[in]    base    Parent layout
	 */
	void createInputViewControl(Evas_Object *base);

	/**
	 * @brief Enable/Disable save button
	 * @param[in]    empty    Indicates whether any information was entered
	 */
	void updateSaveBtn(Eina_Bool empty);

protected:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(int eventType);
	virtual bool onPop();

protected:
	virtual void onLanguageChanged();

private:
	virtual void onTabSelect(Elm_Object_Item *naviItem);
	virtual void onTabUnselect(Elm_Object_Item *naviItem);
	virtual bool onTabPop();
	virtual void onTabScrollStart() {return;}
	virtual void onTabScrollStop() {return;}

	virtual void onDbChanged(const char* viewUri);
	virtual void onCompleteGetContacts(int offset);
	virtual void onConditionChagned();

	void __initialize();

	void __createToastPopup(const char* msg);
	static void __saveCb(void *data, Evas_Object *obj, void *event_info);
	static void __cancelCb(void *data, Evas_Object *obj, void *event_info);
	int __getPersonIdByContactId(int contactId);

	static Eina_Bool __createAddressbookPopupIdlerCb(void *data);

private:
	CtInputView();
	virtual ~CtInputView();

private:

	WUiObjectPtr __inputControl;
	std::shared_ptr<CtData> __inputData;

	int __savedAddressbookId;
	bool __isCreate;
	bool __saved;
	bool __discard;
	bool __changed;

	Evas_Object* __saveBtn;
	Evas_Object* __baseLayout;

	Ecore_Idler *__popupIdler;
	Ecore_Event_Handler* __windowVisibilityChangeHandler;
	std::function<void(int, CtResultType)> __resultHandler;
};

#endif /* _CT_INPUT_VIEW_H_ */
