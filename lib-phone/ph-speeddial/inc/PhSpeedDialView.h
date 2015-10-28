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

#ifndef _PH_SPEED_DIAL_VIEW_H_
#define _PH_SPEED_DIAL_VIEW_H_

#include "WView.h"

#define SPEED_DIAL_ITEM_COUNT 9

class PhSpeedDialItem;

/**
 * @brief Phone speed dial settings view.
 */
class WAPP_ASSIST_EXPORT PhSpeedDialView : public WView
{
public:
	PhSpeedDialView();
	~PhSpeedDialView();

private:
	Evas_Object *onCreate(Evas_Object *parent, void *param);
	void onPushed(Elm_Object_Item *item);

	void fillItems();
	static void onDbChanged(const char *uri, void *data);

	PhSpeedDialItem *m_Items[SPEED_DIAL_ITEM_COUNT];
};

#endif /* _PH_SPEED_DIAL_VIEW_H_ */
