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

#ifndef _PH_DIALER_VIEW_H_
#define _PH_DIALER_VIEW_H_

#include "IContactsTabPageView.h"
#include <string>

class WButton;
class PhDialerEntry;
class PhDialerSearchController;

/**
 * @brief Phone dialer view.
 */
class WAPP_ASSIST_EXPORT PhDialerView : public IContactsTabPageView
{
public:
	PhDialerView();
	~PhDialerView();

	/**
	 * @brief Sets predictive layout
	 * @param[in]	layout		Layout with predictive search results
	 */
	void setPredictiveLayout(Evas_Object *layout);

	/**
	 * @return dialer entry
	 */
	PhDialerEntry *getEntry() const;

	/**
	 * @brief set number
	 * @param[in]	number		Number
	 */
	void setNumber(const std::string &number);

private:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *viewParam);
	virtual void onPush(Elm_Object_Item *naviItem);
	virtual void onTabSelect(Elm_Object_Item *naviItem);
	virtual void onTabUnselect(Elm_Object_Item *naviItem);
	virtual bool onTabPop();
	virtual void onTabScrollStart();
	virtual void onTabScrollStop();

	Evas_Object *createKeypad(Evas_Object *parent);
	Evas_Object *createCallButton(Evas_Object *parent);
	Evas_Object *createBackspaceButton(Evas_Object *parent);

	void onKeyPressed(WButton &button);
	bool onKeyLongpressed(WButton &button);

	void onBackspacePressed(WButton &button);
	bool onBackspaceLongpressed(WButton &button);

	static void onCallPressed(void *data, Evas_Object *obj, void *event_info);
	static void onMenuPressed(void *data, Evas_Object *obj, void *event_info);

	void onSendMessage();
	void onAddWait();
	void onAddPause();
	void onSpeeddialSettings();
	void onCallSettings();

	static void launchCall(const std::string &number);
	void launchSpeeddial(int digit);
	static std::string getSpeeddialNumber(int digit);
	static std::string getLastNumber();

	PhDialerEntry *m_Entry;
	PhDialerSearchController *m_Controller;
	std::string m_InitialNumber;
};

#endif /* _PH_DIALER_VIEW_H_ */
