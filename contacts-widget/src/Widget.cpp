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

#include "Widget.h"
#include "WidgetGengridItem.h"

#include "App/AppControlRequest.h"
#include "App/Path.h"
#include "Ui/Gengrid.h"
#include "Ui/Scale.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include "AppsCommonButtons.h"
#include "AddButtonItem.h"
#include "WidgetItemLayout.h"
#include "WidgetLayout.h"
#include "WidgetLayoutMetrics.h"
#include "WidgetItemLayoutMetrics.h"

#include <notification.h>

#define WIDGET_ID_KEY "widget_id"

#define GRID_COLS 4
#define SMALL_GRID_ROWS 1
#define LARGE_GRID_ROWS 3

using namespace Common;
using namespace std::placeholders;

namespace
{
	const std::string layoutPath = App::getResourcePath(WIDGET_LAYOUT_EDJ);
}

Widget::Widget()
	: m_EditMode(false), m_MaxCount(0),
	  m_Layout(nullptr), m_EditButton(nullptr), m_Gengrid(nullptr), m_AddButton(nullptr)
{
}

void Widget::onCreate(bundle *content)
{
	elm_theme_extension_add(nullptr, App::getResourcePath(APPS_COMMON_BUTTONS_EDJ).c_str());
	elm_theme_extension_add(nullptr, App::getResourcePath(WIDGET_ITEM_LAYOUT_EDJ).c_str());

	initializeItems(content);
	setEmptyMode(m_Items.count() == 0);
}

void Widget::onDestroy(widget_app_destroy_type reason)
{
	if (reason == WIDGET_APP_DESTROY_TYPE_PERMANENT) {
		m_Items.destroy();
	}
}

void Widget::onResize(int width, int height)
{
	/* FIXME: onResize() called twice with non-standard size (720x720) on the second time */
	if (!m_MaxCount) {
		bool isLarge = (getType() == WIDGET_SIZE_TYPE_4x4);
		m_MaxCount = GRID_COLS * (isLarge ? LARGE_GRID_ROWS : SMALL_GRID_ROWS);
	}
	if (m_Gengrid) {
		elm_gengrid_item_size_set(m_Gengrid->getEvasObject(), getWidth() / GRID_COLS, Ui::getScaledValue(ITEM_H));
	}
}

void Widget::initializeItems(bundle *content)
{
	int id = 0;
	if (content) {
		char *idStr = nullptr;
		bundle_get_str(content, WIDGET_ID_KEY, &idStr);

		if (idStr) {
			id = atoi(idStr);
		}
	}

	m_Items.initialize(id);

	if (id == 0) {
		content = bundle_create();
		bundle_add_str(content, WIDGET_ID_KEY, std::to_string(m_Items.getWidgetId()).c_str());
		saveContent(content);
		bundle_free(content);
	}
}

Evas_Object *Widget::createEmptyLayout(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_NO_CONTACTS);
	edje_object_signal_callback_add(elm_layout_edje_get(layout), "mouse,clicked,*", "*",
			(Edje_Signal_Cb) makeCallback(&Widget::onAddPressed), this);

	elm_object_translatable_part_text_set(layout, PART_TITLE,
			"IDS_PB_BODY_CONTACTS");
	elm_object_translatable_part_text_set(layout, PART_HELP,
			"IDS_PB_NPBODY_TAP_HERE_TO_ADD_SHORTCUT");

	return layout;
}

Evas_Object *Widget::createLayout(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_CONTACTS);
	elm_object_part_content_set(layout, PART_BUTTON, createEditButton(layout));
	elm_object_part_content_set(layout, PART_CONTENT, createGengrid(layout));

	return layout;
}

Evas_Object *Widget::createGengrid(Evas_Object *parent)
{
	m_Gengrid = new Ui::Gengrid();
	m_Gengrid->create(parent);
	elm_gengrid_align_set(m_Gengrid->getEvasObject(), 0.0, 0.0);
	elm_gengrid_item_size_set(m_Gengrid->getEvasObject(), getWidth() / GRID_COLS, Ui::getScaledValue(ITEM_H));

	return m_Gengrid->getEvasObject();
}

Evas_Object *Widget::createEditButton(Evas_Object *parent)
{
	m_EditButton = elm_button_add(parent);
	elm_object_style_set(m_EditButton, BUTTON_STYLE_CUSTOM_SMALL);
	elm_object_translatable_text_set(m_EditButton, "IDS_HS_ACBUTTON_EDIT");
	evas_object_smart_callback_add(m_EditButton, "clicked",
			makeCallback(&Widget::onEditPressed), this);

	Evas_Object *edje = elm_layout_edje_get(m_EditButton);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_NORMAL, COLOR_BUTTON_EDIT_NORMAL,
			0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_color_class_set(edje, BUTTON_COLOR_CLASS_PRESSED, COLOR_BUTTON_EDIT_PRESSED,
			0, 0, 0, 0, 0, 0, 0, 0);
	edje_object_text_class_set(edje, "button_text",
			"font=Tizen:style=Regular", BUTTON_TEXT_SIZE);

	return m_EditButton;
}

void Widget::setEmptyMode(bool isEmpty)
{
	Evas_Object *parent = getWindow()->getBaseLayout();

	if (isEmpty) {
		m_Layout = createEmptyLayout(parent);
		m_EditMode = false;
		m_EditButton = nullptr;
		m_Gengrid = nullptr;
		m_AddButton = nullptr;
	} else {
		m_Layout = createLayout(parent);
		for (auto &item : m_Items) {
			addItem(item);
		}
	}

	elm_object_part_content_set(parent, "elm.swallow.content", m_Layout);
}

void Widget::setEditMode(bool isEnabled)
{
	m_EditMode = isEnabled;
	elm_object_translatable_text_set(m_EditButton, m_EditMode
			? "IDS_TPLATFORM_ACBUTTON_DONE_ABB"
			: "IDS_HS_ACBUTTON_EDIT");

	for (auto &&item : *m_Gengrid) {
		if (item != m_AddButton) {
			auto widgetItem = static_cast<WidgetGengridItem *>(item);
			widgetItem->setEditMode(m_EditMode);
		}
	}

	updateAddButton();
}

void Widget::updateAddButton()
{
	if (m_EditMode && m_Items.count() < m_MaxCount) {
		if (!m_AddButton) {
			m_AddButton = new AddButtonItem();
			m_AddButton->setSelectCallback([this] {
				onAddPressed(nullptr, nullptr);
			});
			m_Gengrid->insert(m_AddButton);
		}
	} else {
		if (m_AddButton) {
			delete m_AddButton;
			m_AddButton = nullptr;
		}
	}
}

void Widget::onAddPressed(Evas_Object *obj, void *event_info)
{
	m_AppControl = App::requestContactPick(APP_CONTROL_SELECT_SINGLE, APP_CONTROL_RESULT_ACTION);
	m_AppControl.launch(makeCallbackWithLastParam(&Widget::onPickReply), this, false);
}

void Widget::onEditPressed(Evas_Object *obj, void *event_info)
{
	setEditMode(!m_EditMode);
}

void Widget::onPickReply(app_control_h request, app_control_h reply, app_control_result_e result)
{
	char *type = nullptr;
	int err = app_control_get_extra_data(reply, APP_CONTROL_DATA_TYPE, &type);
	RETM_IF_ERR(err, "app_control_get_extra_data() failed.");

	int id = atoi(App::getSingleExtraData(reply, APP_CONTROL_DATA_SELECTED).c_str());
	RETM_IF(id <= 0, "Wrong data ID.");

	if (strcmp(type, APP_CONTROL_RESULT_CALL) == 0) {
		onAddItem(ActionCall, id);
	} else if (strcmp(type, APP_CONTROL_RESULT_MESSAGE) == 0) {
		onAddItem(ActionMessage, id);
	} else if (strcmp(type, APP_CONTROL_RESULT_EMAIL) == 0) {
		onAddItem(ActionEmail, id);
	}
}

void Widget::addItem(WidgetItem &item)
{
	if (!m_Gengrid) {
		setEmptyMode(false);
		return;
	}

	WidgetGengridItem *gridItem = new WidgetGengridItem(item);
	gridItem->setEditMode(m_EditMode);
	gridItem->setDeleteCallback(std::bind(&Widget::onRemoveItem, this, gridItem));
	m_Gengrid->insert(gridItem, m_AddButton);

	item.setDeleteCallback([this, gridItem] {
		removeItem(gridItem);
	});

	updateAddButton();
}

void Widget::removeItem(WidgetGengridItem *item)
{
	delete item;

	int count = elm_gengrid_items_count(m_Gengrid->getEvasObject());
	if (m_AddButton) {
		--count;
	}

	if (count == 0) {
		setEmptyMode(true);
		return;
	}

	updateAddButton();
}

void Widget::onAddItem(Common::ActionType actionType, int dataId)
{
	for (auto &item : m_Items) {
		if (item.getDataId() == dataId
		 && item.getActionType() == actionType) {
			notification_status_message_post(_("IDS_PB_POP_ALREADY_EXISTS_LC"));
			return;
		}
	}

	if (m_Items.addItem({ actionType, dataId })) {
		addItem(*(--m_Items.end()));
	}
}

void Widget::onRemoveItem(WidgetGengridItem *item)
{
	if (m_Items.removeItem(item->getItem())) {
		removeItem(item);
	}
}
