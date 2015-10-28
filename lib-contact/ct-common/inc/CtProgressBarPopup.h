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

#ifndef _CT_PROGRESS_BAR_POPUP_H_
#define _CT_PROGRESS_BAR_POPUP_H_

#include "WPopup.h"
#include <string>

/**
 * @brief Progress bar pop-up class.
 */
class CtProgressBarPopup : public WPopup
{
public:

	/**
	 * @brief Constructor. Initializes this instance of CtProgressBarPopup with the specified value.
	 * @param[in]   totalCount    Total count
	 * @param[in]   description   The popup title
	 * @param[in]   autoUpdate    Set auto update if true
	 */
	CtProgressBarPopup(int totalCount, const char* description, bool autoUpdate);

	/**
	 * @brief Set progress remain count
	 * @param[in]   count Progress remain count
	 */
	void setProgressRemainCount(int count);

	/**
	 * @brief Set progress status count
	 * @param[in]   count Progress status count
	 */
	void setProgressStatusCount(int count);

private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	Evas_Object* __createContent(Evas_Object *parent);

private:
	CtProgressBarPopup();
	virtual ~CtProgressBarPopup();

private:

	int __totalCount;
	std::string __description;
	Evas_Object* __progressbar;
	Evas_Object* __contentLayout;
	bool __timerUpdate;
	int __remainCount;
	int __statusCount;
	Ecore_Timer* __timer;

};

#endif //_CT_PROGRESS_BAR_POPUP_H_

