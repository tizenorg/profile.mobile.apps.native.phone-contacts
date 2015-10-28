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

#ifndef _CT_SETTING_RADIO_POPUP_H_
#define _CT_SETTING_RADIO_POPUP_H_

#include "WPopup.h"
#include <string>
#include <vector>
#include <functional>


/**
 * @brief Popup with radio group widget
 */
class CtSettingRadioPopup : public WPopup
{
public:

	/**
	 * @brief Creates popup with radio group
	 *
	 * @param[in]    label            List of labels for radio items
	 * @param[in]    selectedIndex    Index of selected radio item
	 */
	CtSettingRadioPopup(const std::vector<std::string>& label, int selectedIndex);

	/**
	 * @brief Sets callback on radio item selection
	 *
	 * @param[in]    resultCb    Result callback function
	 */
	void setOnResultCb(std::function<void (int)> resultCb);

private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);
	static void __itemSelectedCb(void* data, Evas_Object* obj, void* event_info);

	virtual ~CtSettingRadioPopup();

private:
	struct __ItemData
	{
		__ItemData();
		CtSettingRadioPopup* popup;
		std::string label;
		int index;
	};

private:
	std::vector<std::string> __label;
	int __selectedIndex;
	std::function<void (int)> __resultCb;
};


#endif //_CT_SETTING_RADIO_POPUP_H_
