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

#ifndef WIDGET_GENGRID_ITEM_H
#define WIDGET_GENGRID_ITEM_H

#include <Elementary.h>
#include <functional>

class WidgetItem;

/**
 * @brief Widget gengrid item.
 */
class WidgetGengridItem
{
public:
	typedef std::function<void()> DeleteCallback;

	/**
	 * @brief Create widget gengrid item.
	 * @param[in]   Widget item associated with gengrid item
	 */
	WidgetGengridItem(WidgetItem &item);

	/**
	 * @brief Insert item before @a nextItem, or append if not specified.
	 * @param[in]   gengrid	    Parent gengrid
	 * @param[in]   nextItem    Next item to insert before it or nullptr to append.
	 */
	void insert(Evas_Object *gengrid, Elm_Object_Item *nextItem);

	/**
	 * @brief Set item edit mode.
	 * @param[in]   isEnabled   Whether edit mode is enabled
	 */
	void setEditMode(bool isEnabled);

	/**
	 * @brief Set item delete callback.
	 * @param[in]   callback    Delete button press callback
	 */
	void setDeleteCallback(DeleteCallback callback);

	/**
	 * @return Widget item associated with gengrid item.
	 */
	WidgetItem &getItem() const;

	/**
	 * @return Underlying gengrid item.
	 */
	Elm_Object_Item *getObjectItem() const;

private:
	static Elm_Gengrid_Item_Class *getItemClass();

	char *getText(Evas_Object *parent, const char *part);
	Evas_Object *getContent(Evas_Object *parent, const char *part);

	Evas_Object *createThumbnail(Evas_Object *parent);
	Evas_Object *createTypeIcon(Evas_Object *parent);
	Evas_Object *createDeleteButton(Evas_Object *parent);

	void onSelected(Evas_Object *obj, void *eventInfo);
	void onDeletePressed(Evas_Object *obj, void *eventInfo);
	void onItemChanged(int changes);

	WidgetItem &m_Item;
	bool m_EditMode;

	Elm_Object_Item *m_ObjectItem;
	DeleteCallback m_OnDelete;
};

#endif /* WIDGET_GENGRID_ITEM_H */
