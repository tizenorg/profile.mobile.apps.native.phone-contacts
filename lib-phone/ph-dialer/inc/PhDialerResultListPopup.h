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

#ifndef _PH_DIALER_RESULT_LIST_POPUP_H_
#define _PH_DIALER_RESULT_LIST_POPUP_H_

#include "PhDialerPredictiveNumberTypes.h"
#include "WPopup.h"
#include <Elementary.h>

class PhDialerEntry;

/**
 * @brief Predictive search result list popup.
 */
class PhDialerResultListPopup : public WPopup
{
public:
	/**
	 * @brief Constructor
	 * @param[in]	result		Search data
	 * @param[in]	entry		Dialer entry
	 */
	PhDialerResultListPopup(const Ph::Dialer::SearchList &result, PhDialerEntry &entry);

private:
	virtual Evas_Object *onCreate(Evas_Object *parent, void *param);
	Evas_Object *createContactList(Evas_Object *parent);
	Elm_Genlist_Item_Class *createItemClass();
	static char *getItemText(void *data, Evas_Object *obj, const char *part);

	static void onItemClicked(void *data, Evas_Object *obj, void *event_info);

	const Ph::Dialer::SearchList &m_Result;
	PhDialerEntry &m_Entry;
};

#endif //_PH_DIALER_RESULT_LIST_POPUP_H_
