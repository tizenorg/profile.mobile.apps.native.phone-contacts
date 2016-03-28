/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef WIDGET_H
#define WIDGET_H

#include <Elementary.h>
#include <string>

#include "App/AppControl.h"
#include "App/Widget.h"
#include "WidgetItems.h"

class WidgetGengridItem;

/**
 * @brief Contacts homescreen widget.
 */
class Widget : public App::Widget
{
public:
	Widget();

private:
	virtual void onCreate(bundle *content) override;
	virtual void onDestroy(widget_app_destroy_type reason) override;
	virtual void onResize(int width, int height) override;
	void initializeItems(bundle *content);

	Evas_Object *createEmptyLayout(Evas_Object *parent);
	Evas_Object *createLayout(Evas_Object *parent);
	Evas_Object *createGengrid(Evas_Object *parent);
	Evas_Object *createEditButton(Evas_Object *parent);
	Elm_Gengrid_Item_Class *getAddButtonItemClass();

	void setEmptyMode(bool isEmpty);
	void setEditMode(bool isEnabled);
	void updateAddButton();

	void onAddPressed(Evas_Object *obj, void *event_info);
	void onEditPressed(Evas_Object *obj, void *event_info);
	void onPickReply(app_control_h request, app_control_h reply, app_control_result_e result);

	void addItem(WidgetItem &item);
	void removeItem(WidgetGengridItem *item);

	void onAddItem(Common::ActionType actionType, int dataId);
	void onRemoveItem(WidgetGengridItem *item);

	App::AppControl m_AppControl;
	WidgetItems m_Items;
	bool m_EditMode;
	size_t m_MaxCount;

	int m_WidgetHeight;
	std::string m_LayoutPath;

	Evas_Object *m_Layout;
	Evas_Object *m_EditButton;
	Evas_Object *m_Gengrid;
	Elm_Object_Item *m_AddButton;
};

#endif /* WIDGET_H */
