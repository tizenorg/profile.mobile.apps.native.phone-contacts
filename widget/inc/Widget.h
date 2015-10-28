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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <app_control.h>
#include <Elementary.h>
#include <string>

#include "WWidget.h"
#include "WidgetItems.h"

class WidgetGengridItem;

/**
 * @brief Contacts homescreen widget.
 */
class Widget : public WWidget
{
public:
	Widget();

private:
	virtual void onCreate(bundle *content, int w, int h);
	virtual void onDestroy(widget_app_destroy_type reason);
	void initializeItems(bundle *content);

	Evas_Object *createNocontents(Evas_Object *parent);
	Evas_Object *createGengrid(Evas_Object *parent);
	Elm_Gengrid_Item_Class *getAddButtonItemClass();

	void setLayout(bool isEmpty);
	void setAddButton();

	static void onAddPressed(void *data, Evas_Object *obj, void *event_info);
	static void onEditPressed(void *data, Evas_Object *obj, void *event_info);
	static void onPickReply(app_control_h request, app_control_h reply, app_control_result_e result, void *data);

	void onItemUpdate(WidgetItem &item);
	void onItemDelete(WidgetItem &item);

	void addItem(WidgetItem &item);
	void removeItem(WidgetGengridItem &item);

	void onAddItem(WidgetItemType type, int dataId);
	void onRemoveItem(WidgetGengridItem &item);

	WidgetItems m_Items;
	bool m_EditMode;
	bool m_IsPicking;
	size_t m_MaxCount;

	int m_WidgetHeight;
	std::string m_LayoutPath;

	Evas_Object *m_Layout;
	Evas_Object *m_Gengrid;
	Elm_Object_Item *m_AddButton;
};

#endif /* _WIDGET_H_ */
