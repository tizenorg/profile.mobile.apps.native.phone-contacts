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

#ifndef UI_GENLIST_H
#define UI_GENLIST_H

#include "Ui/Control.h"

namespace Ui
{
	class GenlistItem;

	class EXPORT_API Genlist : public Control
	{
	public:
		/**
		 * @return Genlist first Item.
		 */
		GenlistItem *getFirstItem() const;

		/**
		 * @return Genlist last Item.
		 */
		GenlistItem *getLastItem() const;

		/**
		 * @brief Insert or append genlist Item.
		 * @param[in]   item    Item to insert
		 * @param[in]   parent  Parent item
		 * @param[in]   prev    Previous item or nullptr to append
		 * @return New genlist object item
		 */
		Elm_Object_Item *insert(GenlistItem *item,
					GenlistItem *parent = nullptr,
					GenlistItem *prev = nullptr);

		/**
		 * @brief Update specific parts of realized items
		 * @param[in]   parts   The name of item parts
		 * @param[in]   type    The type of item parts
		 * @see elm_genlist_item_fields_update()
		 */
		void update(const char *parts, Elm_Genlist_Item_Field_Type type);

	private:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

		static void onItemSelected(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
		static void onItemExpanded(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
		static void onItemContracted(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
		static void onItemRealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
		static void onItemUnrealized(void *data, Evas_Object *obj, Elm_Object_Item *objectItem);
	};
}

#endif /* UI_GENLIST_H */
