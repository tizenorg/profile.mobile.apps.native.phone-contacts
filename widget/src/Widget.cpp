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

#include "Widget.h"
#include "WidgetGengridItem.h"
#include "WWindow.h"

#include "WidgetItemLayout.h"
#include "WidgetLayout.h"
#include "WidgetLayoutMetrics.h"
#include "WidgetItemLayoutMetrics.h"

#include "CommonPath.h"
#include "ContactsAppControl.h"
#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsLocalization.h"

#include <notification.h>

#define WIDGET_ID_KEY "widget_id"

namespace
{
	const std::string layout4x4Path = ContactsCommon::getResourcePath(WIDGET_LAYOUT_4X4_EDJ);
	const std::string layout4x2Path = ContactsCommon::getResourcePath(WIDGET_LAYOUT_4X2_EDJ);
}

Widget::Widget()
	: m_EditMode(false), m_IsPicking(false), m_MaxCount(0),
	  m_WidgetHeight(0), m_Layout(nullptr), m_Gengrid(nullptr), m_AddButton(nullptr)
{
	using namespace std::placeholders;
	m_Items.setOnItemUpdate(std::bind(&Widget::onItemUpdate, this, _1));
	m_Items.setOnItemDelete(std::bind(&Widget::onItemDelete, this, _1));
}

void Widget::onCreate(bundle *content, int w, int h)
{
	initializeItems(content);

	if (getType() == WIDGET_SIZE_TYPE_4x4) {
		m_MaxCount = 12;
		m_WidgetHeight = WIDGET_HEIGHT_4X4;
		m_LayoutPath = layout4x4Path;
	} else {
		m_MaxCount = 4;
		m_WidgetHeight = WIDGET_HEIGHT_4X2;
		m_LayoutPath = layout4x2Path;
	}

	elm_theme_extension_add(nullptr, ContactsCommon::getResourcePath(WIDGET_ITEM_LAYOUT_EDJ).c_str());
	elm_theme_extension_add(nullptr, ContactsCommon::getResourcePath(COMMON_BUTTONS_EDJ).c_str());
	setLayout(m_Items.count() == 0);
}

void Widget::onDestroy(widget_app_destroy_type reason)
{
	if (reason == WIDGET_APP_DESTROY_TYPE_PERMANENT) {
		m_Items.destroy();
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
	WDEBUG("id: %d", id);

	m_Items.initialize(id);

	if (id == 0) {
		content = bundle_create();
		bundle_add_str(content, WIDGET_ID_KEY, std::to_string(m_Items.getWidgetId()).c_str());
		saveContent(content);
		bundle_free(content);
	}
}

Evas_Object *Widget::createNocontents(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, m_LayoutPath.c_str(), GROUP_NO_CONTACTS);
	edje_object_signal_callback_add(elm_layout_edje_get(layout), "mouse,clicked,*", "*",
			(Edje_Signal_Cb) &Widget::onAddPressed, this);

	elm_object_domain_translatable_part_text_set(layout, PART_TITLE, TEXT_DOMAIN,
			"IDS_PB_HEADER_CONTACTS_M_APPLICATION");
	elm_object_domain_translatable_part_text_set(layout, PART_HELP, TEXT_DOMAIN,
			"IDS_PB_NPBODY_TAP_HERE_TO_ADD_SHORTCUT");

	return layout;
}

Evas_Object *Widget::createGengrid(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, m_LayoutPath.c_str(), GROUP_CONTACTS);

	Evas_Object *button = elm_button_add(layout);
	elm_object_style_set(button, "custom_default");
	edje_object_text_class_set(elm_layout_edje_get(button), "button_text", "font=Tizen:style=Regular", BUTTON_TEXT_SIZE);
	elm_object_domain_translatable_text_set(button, TEXT_DOMAIN, "IDS_QP_ACBUTTON_EDIT_ABB");
	evas_object_smart_callback_add(button, "clicked", &Widget::onEditPressed, this);
	elm_object_part_content_set(layout, PART_BUTTON, button);

	m_Gengrid = elm_gengrid_add(parent);
	elm_gengrid_item_size_set(m_Gengrid,
			ITEM_WIDTH * getWidth() / WIDGET_WIDTH,
			ITEM_HEIGHT * getHeight() / m_WidgetHeight);

	elm_gengrid_align_set(m_Gengrid, 0.0, 0.0);
	elm_object_part_content_set(layout, PART_CONTENT, m_Gengrid);

	for (auto &item : m_Items) {
		addItem(item);
	}

	return layout;
}

Elm_Gengrid_Item_Class *Widget::getAddButtonItemClass()
{
	static Elm_Gengrid_Item_Class itc = { 0 };
	if (!itc.item_style) {
		itc.version = ELM_GENGRID_ITEM_CLASS_VERSION;
		itc.item_style = WIDGET_ITEM_STYLE;
		itc.func.content_get = [](void *data, Evas_Object *obj, const char *part) -> Evas_Object* {
			if (strcmp(part, PART_THUMBNAIL) == 0) {
				Widget *widget = (Widget*) data;
				Evas_Object *layout = elm_layout_add(obj);
				elm_layout_file_set(layout, widget->m_LayoutPath.c_str(), GROUP_ICON_ADD);
				return layout;
			}

			return nullptr;
		};
	}

	return &itc;
}

void Widget::setLayout(bool isEmpty)
{
	Evas_Object *parent = getWindow()->getBaseLayoutEvasObj();

	if (isEmpty) {
		m_Layout = createNocontents(parent);
		m_EditMode = false;
		m_Gengrid = nullptr;
		m_AddButton = nullptr;
	} else {
		m_Layout = createGengrid(parent);
	}

	elm_object_part_content_set(parent, "elm.swallow.content", m_Layout);
}

void Widget::setAddButton()
{
	if (m_EditMode && m_Items.count() < m_MaxCount) {
		if (!m_AddButton) {
			m_AddButton = elm_gengrid_item_append(m_Gengrid, getAddButtonItemClass(), this, &Widget::onAddPressed, this);
		}
	} else if (m_AddButton) {
		elm_object_item_del(m_AddButton);
		m_AddButton = nullptr;
	}
}

void Widget::onAddPressed(void *data, Evas_Object *obj, void *event_info)
{
	Widget *widget = (Widget*) data;
	if (widget->m_AddButton) {
		elm_gengrid_item_selected_set(widget->m_AddButton, EINA_FALSE);
	}

	if (!widget->m_IsPicking) {
		int err = launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_SINGLE, APP_CONTROL_DATA_TYPE_PHONE_OR_EMAIL,
				&Widget::onPickReply, data, false);
		WPRET_M(err != APP_CONTROL_ERROR_NONE, "launchContactPick() failed(%d)", err);
		widget->m_IsPicking = true;
	}
}

void Widget::onEditPressed(void *data, Evas_Object *obj, void *event_info)
{
	Widget *widget = (Widget*) data;
	widget->m_EditMode = !widget->m_EditMode;
	elm_object_domain_translatable_text_set(obj, TEXT_DOMAIN, widget->m_EditMode
			? "IDS_TPLATFORM_ACBUTTON_DONE_ABB"
			: "IDS_QP_ACBUTTON_EDIT_ABB");
	elm_gengrid_realized_items_update(widget->m_Gengrid);
	widget->setAddButton();
}

void Widget::onPickReply(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	Widget *widget = (Widget*) data;
	widget->m_IsPicking = false;

	char *type = nullptr;
	char **ids = nullptr;
	int count = 0;

	int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &ids, &count);
	WPRET_M(err != APP_CONTROL_ERROR_NONE, "app_control_get_extra_data() failed(%d)", err);

	app_control_get_extra_data(reply, APP_CONTROL_DATA_TYPE, &type);
	if (type && ids && ids[0]) {
		if (strcmp(type, APP_CONTROL_DATA_TYPE_PHONE_CALL) == 0) {
			widget->onAddItem(WidgetItemCall, atoi(ids[0]));
		} else if (strcmp(type, APP_CONTROL_DATA_TYPE_PHONE_MESSAGE) == 0) {
			widget->onAddItem(WidgetItemMessage, atoi(ids[0]));
		} else if (strcmp(type, APP_CONTROL_DATA_TYPE_EMAIL) == 0) {
			widget->onAddItem(WidgetItemEmail, atoi(ids[0]));
		}
	}

	free(type);
	for (int i = 0; i < count; ++i) {
		free(ids[i]);
	}
	free(ids);
}

void Widget::onItemUpdate(WidgetItem &item)
{
	WidgetGengridItem *gridItem = (WidgetGengridItem*) item.getUserData();
	elm_gengrid_item_update(gridItem->getObjectItem());
}

void Widget::onItemDelete(WidgetItem &item)
{
	WidgetGengridItem *gridItem = (WidgetGengridItem*) item.getUserData();
	removeItem(*gridItem);
}

void Widget::addItem(WidgetItem &item)
{
	if (m_Gengrid) {
		WidgetGengridItem *gridItem = new WidgetGengridItem(item, m_EditMode);
		item.setUserData(gridItem);

		gridItem->setOnDelete(std::bind(&Widget::onRemoveItem, this, std::placeholders::_1));
		gridItem->insert(m_Gengrid, m_AddButton);

		if (m_Items.count() == m_MaxCount) {
			setAddButton();
		}
	} else {
		setLayout(false);
	}
}

void Widget::removeItem(WidgetGengridItem &item)
{
	elm_object_item_del(item.getObjectItem());

	int count = elm_gengrid_items_count(m_Gengrid);
	if (m_EditMode) {
		--count;
	}

	if (count == 0) {
		setLayout(true);
	} else {
		if (m_Items.count() == m_MaxCount - 1){
			setAddButton();
		}
	}
}

void Widget::onAddItem(WidgetItemType type, int dataId)
{
	for (auto &item : m_Items) {
		if (item.getDataId() == dataId
		 && item.getType() == type) {
			notification_status_message_post(T_("IDS_PB_POP_ALREADY_EXISTS_LC"));
			return;
		}
	}

	if (m_Items.addItem(WidgetItem(type, dataId))) {
		addItem(*(--m_Items.end()));
	}
}

void Widget::onRemoveItem(WidgetGengridItem &item)
{
	if (m_Items.removeItem(item.getItem())) {
		removeItem(item);
	}
}
