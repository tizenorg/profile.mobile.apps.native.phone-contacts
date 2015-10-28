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

#include "WidgetGengridItem.h"
#include "WidgetItemLayout.h"
#include "WidgetItem.h"

#include "ContactsCommon.h"
#include "ContactsDebug.h"
#include "ContactsThumbnail.h"

#include <app_control.h>
#include <string>

namespace
{
	const char *iconFileName[] = {
		ICON_CALL,
		ICON_MESSAGE,
		ICON_EMAIL
	};

	const char *appControlOperation[] = {
		APP_CONTROL_OPERATION_CALL,
		APP_CONTROL_OPERATION_COMPOSE,
		APP_CONTROL_OPERATION_COMPOSE
	};

	const char *appControlScheme[] = {
		"tel:",
		"sms:",
		"mailto:"
	};

	const std::string layoutPath = ContactsCommon::getResourcePath(WIDGET_ITEM_LAYOUT_EDJ);
}

WidgetGengridItem::WidgetGengridItem(const WidgetItem &item, const bool &editMode)
	: m_Item(item),  m_EditMode(editMode), m_ObjectItem(nullptr)
{
}

void WidgetGengridItem::insert(Evas_Object *gengrid, Elm_Object_Item *nextItem)
{
	if (nextItem) {
		m_ObjectItem = elm_gengrid_item_insert_before(gengrid, getItemClass(), this, nextItem,
				&WidgetGengridItem::onSelect, this);
	} else {
		m_ObjectItem = elm_gengrid_item_append(gengrid, getItemClass(), this,
				&WidgetGengridItem::onSelect, this);
	}
}

const WidgetItem &WidgetGengridItem::getItem() const
{
	return m_Item;
}

Elm_Object_Item *WidgetGengridItem::getObjectItem() const
{
	return m_ObjectItem;
}

void WidgetGengridItem::setOnDelete(DeletePressedCb callback)
{
	m_OnDeletePressed = std::move(callback);
}

Elm_Gengrid_Item_Class *WidgetGengridItem::getItemClass()
{
	static Elm_Gengrid_Item_Class itc;
	itc.version = ELM_GENGRID_ITEM_CLASS_VERSION,
	itc.refcount = 0,
	itc.delete_me = EINA_FALSE,
//	TODO uncomment when homogeneous member will be implemented
//	itc.homogeneous = EINA_TRUE,
	itc.item_style = WIDGET_ITEM_STYLE,
	itc.decorate_item_style = nullptr,
	itc.decorate_all_item_style = nullptr,
	itc.func.text_get = &WidgetGengridItem::onTextGet,
	itc.func.content_get = &WidgetGengridItem::onContentGet,
	itc.func.state_get = nullptr,
	itc.func.del = [](void *data, Evas_Object *obj) {
		delete static_cast<WidgetGengridItem*>(data);
	};

	return &itc;
}

char *WidgetGengridItem::onTextGet(void *data, Evas_Object *parent, const char *part)
{
	WidgetGengridItem *item = (WidgetGengridItem*) data;
	if (strcmp(part, PART_NAME) == 0) {
		const char *name = item->getItem().getDisplayName();
		return name ? strdup(name) : NULL;
	}

	return nullptr;
}

Evas_Object *WidgetGengridItem::onContentGet(void *data, Evas_Object *parent, const char *part)
{
	WidgetGengridItem *item = (WidgetGengridItem*) data;

	if (strcmp(part, PART_THUMBNAIL) == 0) {
		return item->createThumbnail(parent);
	} else if (strcmp(part, PART_ICON_TYPE) == 0) {
		return item->createTypeIcon(parent);
	} else if (item->m_EditMode && strcmp(part, PART_ICON_DELETE) == 0) {
		return item->createDeleteButton(parent);
	}

	return nullptr;
}

Evas_Object *WidgetGengridItem::createThumbnail(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_THUMBNAIL);

	Evas_Object *image = ::createThumbnail(layout, THUMBNAIL_166, m_Item.getThumbnailPath());
	elm_object_part_content_set(layout, PART_THUMBNAIL, image);

	return layout;
}

Evas_Object *WidgetGengridItem::createTypeIcon(Evas_Object *parent)
{
	std::string path = ContactsCommon::getResourcePath(iconFileName[m_Item.getType()]);

	Evas_Object *icon = elm_image_add(parent);
	elm_image_file_set(icon, path.c_str(), nullptr);

	return icon;
}

Evas_Object *WidgetGengridItem::createDeleteButton(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "transparent");
	evas_object_smart_callback_add(button, "clicked", onDeletePressed, this);

	Evas_Object *image = elm_image_add(button);
	elm_image_file_set(image, layoutPath.c_str(), GROUP_BUTTON_DELETE);
	elm_object_part_content_set(button, "elm.swallow.content", image);

	return button;
}

void WidgetGengridItem::onDeletePressed(void *data, Evas_Object *obj, void *eventInfo)
{
	WidgetGengridItem *item = (WidgetGengridItem*) data;
	if (item->m_OnDeletePressed) {
		item->m_OnDeletePressed(*item);
	}
}

void WidgetGengridItem::onSelect(void *data, Evas_Object *obj, void *eventInfo)
{
	elm_gengrid_item_selected_set((Elm_Gengrid_Item*) eventInfo, EINA_FALSE);

	WidgetGengridItem *item = (WidgetGengridItem*) data;
	if (item->m_EditMode) {
		return;
	}

	WidgetItemType type = item->getItem().getType();
	const char *itemData = item->getItem().getData();
	if (!itemData) {
		return;
	}

	app_control_h request;
	app_control_create(&request);
	app_control_set_operation(request, appControlOperation[type]);
	app_control_set_uri(request, std::string(appControlScheme[type]).append(itemData).c_str());
	int result = app_control_send_launch_request(request, nullptr, nullptr);
	WPWARN(result != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", result);
	app_control_destroy(request);
}
