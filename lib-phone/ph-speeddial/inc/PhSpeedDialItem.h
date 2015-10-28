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

#ifndef _PH_SPEED_DIAL_ITEM_H_
#define _PH_SPEED_DIAL_ITEM_H_

#include <app_control.h>
#include <contacts.h>
#include "WControl.h"

/**
 * @brief Speed dial settings view item.
 */
class PhSpeedDialItem : public WControl
{
public:
	/**
	 * @brief Create speed dial settings item.
	 * @param[in]   number  Speed dial dialer number (1-9)
	 */
	PhSpeedDialItem(int number);
	~PhSpeedDialItem();

	/**
	 * @brief Set speed dial item data.
	 * @param[in]   record  Speed dial record to fetch the data from
	 */
	void setData(contacts_record_h record);

private:
	Evas_Object *onCreate(Evas_Object *parent, void *param);
	Evas_Object *createAddButton(Evas_Object *parent);
	Evas_Object *createDeleteButton(Evas_Object *parent);

	void setEmptyLayout(Evas_Object *layout);
	void setLayoutData(Evas_Object *layout, contacts_record_h record);
	void fetchData();
	void closeChooser();

	static void onPickResult(app_control_h request, app_control_h reply,
			app_control_result_e result, void *data);
	static void onItemPressed(void *data, Evas_Object *obj, void *event_info);
	static void onDeletePressed(void *data, Evas_Object *obj, void *event_info);

	int m_Number;
	app_control_h m_Chooser;
};

#endif /* _PH_SPEED_DIAL_ITEM_H_ */
