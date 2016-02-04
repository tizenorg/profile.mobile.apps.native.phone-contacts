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

#ifndef CONTACTS_DETAILS_MULTILINE_FIELD_ITEM_H
#define CONTACTS_DETAILS_MULTILINE_FIELD_ITEM_H

#include "Contacts/Details/FieldItem.h"

namespace Contacts
{
	namespace Details
	{
		/**
		 * @brief Genlist item for multiline represention of ContactObject's field.
		 */
		class MultilineFieldItem : public FieldItem
		{
		public:
			using FieldItem::FieldItem;

		protected:
			/**
			 * @see GenlistItem::getItemClass()
			 */
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

			/**
			 * @see GenlistItem::getText()
			 */
			virtual char *getText(Evas_Object *parent, const char *part) override;

			/**
			 * @see FieldItem::onFieldUpdated()
			 */
			virtual void onFieldUpdated(Model::ContactField &field, contacts_changed_e change) override;
		};
	}
}

#endif /* CONTACTS_DETAILS_MULTILINE_FIELD_ITEM_H */
