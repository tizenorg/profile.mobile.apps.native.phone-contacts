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

#ifndef _PH_LOG_LIST_VIEW_H_
#define _PH_LOG_LIST_VIEW_H_
#include <system_settings.h>

#include "WView.h"

#include "IContactsTabPageView.h"

#include "PhDbListener.h"

#include "PhLogGenlistHelper.h"

#define LOGLIST_SUB_TEXT_SIZE 22

/**
 * @brief Phone log list view
 */
class PhLogListView : public PhDbListener, public IContactsTabPageView
{
public:
	/**
	 * @brief Creates log list view
	 */
	PhLogListView();

public:
	/**
	 * @brief Gets genlist helper.
	 *
	 * @return genlist helper.
	 *
	 * @see PhLogGenlistHelper
	 */
	PhLogGenlistHelper *getGlHelper(void);

	/**
	 * @brief Called when log list needs to be updated.
	 */
	virtual void updateList(void) {};

protected:
	virtual void setGenlistLayout();
	virtual void setNoContentLayout(void);

	virtual void onTabSelect(Elm_Object_Item *naviItem);
	virtual void onTabUnselect(Elm_Object_Item *naviItem);
	virtual bool onTabPop(){return true;}
	virtual void onTabScrollStart() {return;}
	virtual void onTabScrollStop() {return;}
	static void formatTimeDateChangedCb(system_settings_key_e key, void *data);
	virtual void onCreated(void);
	virtual void onDestroy(void);

protected:
	virtual ~PhLogListView();

private:
	PhLogGenlistHelper* __glHelper;
};

#endif //_PH_LOG_LIST_VIEW_H_


