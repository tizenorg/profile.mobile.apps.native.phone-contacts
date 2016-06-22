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

#include "Ui/GengridItem.h"

class WidgetItem;

/**
 * @brief Widget gengrid item.
 */
class WidgetGengridItem : public Ui::GengridItem
{
public:
	typedef std::function<void()> DeleteCallback;

	/**
	 * @brief Create widget gengrid item.
	 * @param[in]   Widget item associated with gengrid item
	 */
	WidgetGengridItem(WidgetItem &item);

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

private:
	virtual Elm_Gengrid_Item_Class *getItemClass() const override;
	virtual char *getText(Evas_Object *parent, const char *part) override;
	virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

	Evas_Object *createThumbnail(Evas_Object *parent);
	Evas_Object *createTypeIcon(Evas_Object *parent);
	Evas_Object *createDeleteButton(Evas_Object *parent);

	virtual void onSelected() override;
	void onDeletePressed(Evas_Object *obj, void *eventInfo);
	void onItemChanged(int changes);

	WidgetItem &m_Item;
	bool m_EditMode;
	DeleteCallback m_OnDelete;
};

#endif /* WIDGET_GENGRID_ITEM_H */
