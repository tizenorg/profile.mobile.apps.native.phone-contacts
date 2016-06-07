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

#include "WidgetGengridItem.h"
#include "WidgetItemLayout.h"
#include "WidgetItem.h"

#include "App/Path.h"
#include "Ui/Thumbnail.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"
#include "Utils/String.h"

using namespace std::placeholders;

namespace
{
	const std::string layoutPath = App::getResourcePath(WIDGET_ITEM_LAYOUT_EDJ);
}

WidgetGengridItem::WidgetGengridItem(WidgetItem &item)
	: m_Item(item),  m_EditMode(false)
{
	m_Item.setChangeCallback(std::bind(&WidgetGengridItem::onItemChanged, this, _1));
}

void WidgetGengridItem::setEditMode(bool isEnabled)
{
	m_EditMode = isEnabled;
	elm_gengrid_item_fields_update(getObjectItem(), PART_ICON_DELETE, ELM_GENGRID_ITEM_FIELD_CONTENT);
}

void WidgetGengridItem::setDeleteCallback(DeleteCallback callback)
{
	m_OnDelete = std::move(callback);
}

WidgetItem &WidgetGengridItem::getItem() const
{
	return m_Item;
}

Elm_Gengrid_Item_Class *WidgetGengridItem::getItemClass() const
{
	static Elm_Gengrid_Item_Class itc = createItemClass(WIDGET_ITEM_STYLE);
	return &itc;
}

char *WidgetGengridItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_NAME) == 0) {
		return Utils::safeDup(m_Item.getName());
	}

	return nullptr;
}

Evas_Object *WidgetGengridItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_THUMBNAIL) == 0) {
		return createThumbnail(parent);
	} else if (strcmp(part, PART_ICON_TYPE) == 0) {
		return createTypeIcon(parent);
	} else if (m_EditMode && strcmp(part, PART_ICON_DELETE) == 0) {
		return createDeleteButton(parent);
	}

	return nullptr;
}

Evas_Object *WidgetGengridItem::createThumbnail(Evas_Object *parent)
{
	using Ui::Thumbnail;

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), GROUP_THUMBNAIL);

	Thumbnail *image = Thumbnail::create(layout, Thumbnail::SizeMedium, m_Item.getImagePath());
	elm_object_part_content_set(layout, PART_THUMBNAIL, image->getEvasObject());

	return layout;
}

Evas_Object *WidgetGengridItem::createTypeIcon(Evas_Object *parent)
{
	static const char *icons[] = {
		/* ActionCall    = */ ICON_CALL,
		/* ActionMessage = */ ICON_MESSAGE,
		/* ActionEmail   = */ ICON_EMAIL
	};

	std::string path = App::getResourcePath(icons[m_Item.getActionType()]);

	Evas_Object *icon = elm_image_add(parent);
	elm_image_file_set(icon, path.c_str(), nullptr);

	return icon;
}

Evas_Object *WidgetGengridItem::createDeleteButton(Evas_Object *parent)
{
	Evas_Object *button = elm_image_add(parent);
	elm_image_file_set(button, layoutPath.c_str(), GROUP_BUTTON_DELETE);
	evas_object_smart_callback_add(button, "clicked",
			makeCallback(&WidgetGengridItem::onDeletePressed), this);

	return button;
}

void WidgetGengridItem::onSelected()
{
	if (m_EditMode) {
		return;
	}

	App::AppControl request = Common::requestAction(m_Item.getActionType(), m_Item.getData());
	request.launch(nullptr, nullptr, false);
	request.detach();
}

void WidgetGengridItem::onDeletePressed(Evas_Object *obj, void *eventInfo)
{
	if (m_OnDelete) {
		m_OnDelete();
	}
}

void WidgetGengridItem::onItemChanged(int changes)
{
	if (changes & WidgetItem::ChangeName) {
		elm_gengrid_item_fields_update(getObjectItem(), PART_NAME, ELM_GENGRID_ITEM_FIELD_TEXT);
	}
	if (changes & WidgetItem::ChangeImage) {
		elm_gengrid_item_fields_update(getObjectItem(), PART_THUMBNAIL, ELM_GENGRID_ITEM_FIELD_CONTENT);
	}
}
