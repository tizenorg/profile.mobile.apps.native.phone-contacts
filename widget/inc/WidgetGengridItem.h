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

#ifndef _WIDGET_GENGRID_ITEM_H_
#define _WIDGET_GENGRID_ITEM_H_

#include <Elementary.h>
#include <functional>

class WidgetItem;

/**
 * @brief Widget gengrid item.
 */
class WidgetGengridItem
{
public:
	typedef std::function<void(WidgetGengridItem&)> DeletePressedCb;

	/**
	 * @brief Create widget gengrid item.
	 * @param[in]   Widget item associated with gengrid item
	 */
	WidgetGengridItem(const WidgetItem &item, const bool &editMode);

	/**
	 * @brief Insert item before @a nextItem, or append if not specified.
	 * @param[in]   gengrid	    Parent gengrid
	 * @param[in]   nextItem    Next item to insert before it or nullptr to append.
	 */
	void insert(Evas_Object *gengrid, Elm_Object_Item *nextItem);

	/**
	 * @return Widget item associated with gengrid item.
	 */
	const WidgetItem &getItem() const;

	/**
	 * @return Underlying gengrid item.
	 */
	Elm_Object_Item *getObjectItem() const;

	/**
	 * @brief Set delete button callback.
	 * @param[in]   callback    Delete button press callback
	 */
	void setOnDelete(DeletePressedCb callback);

private:
	static Elm_Gengrid_Item_Class *getItemClass();

	static char *onTextGet(void *data, Evas_Object *parent, const char *part);
	static Evas_Object *onContentGet(void *data, Evas_Object *parent, const char *part);

	Evas_Object *createThumbnail(Evas_Object *parent);
	Evas_Object *createTypeIcon(Evas_Object *parent);
	Evas_Object *createDeleteButton(Evas_Object *parent);

	static void onDeletePressed(void *data, Evas_Object *obj, void *eventInfo);
	static void onSelect(void *data, Evas_Object *obj, void *eventInfo);

	const WidgetItem &m_Item;
	const bool &m_EditMode;

	Elm_Object_Item *m_ObjectItem;
	DeletePressedCb m_OnDeletePressed;
};

#endif /* _WIDGET_GENGRID_ITEM_H_ */
