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

#ifndef CONTACTS_DETAILS_FIELD_ITEM_H
#define CONTACTS_DETAILS_FIELD_ITEM_H

#include "Contacts/Common/SelectMode.h"
#include "Ui/GenlistCheckItem.h"
#include <contacts.h>

namespace Contacts
{
	namespace Model
	{
		class ContactField;
		class ContactObject;
	}

	namespace Details
	{
		/**
		 * @brief Genlist item representing ContactObject and its first field.
		 */
		class FieldItem : public Ui::GenlistCheckItem
		{
		public:
			/**
			 * @brief Called when the item (or an action) is selected in #SelectSingle mode.
			 * @param[in]   Selection result
			 */
			typedef std::function<void(SelectResult)> SelectCallback;

			/**
			 * @brief Create genlist item representing ContactObject.
			 * @param[in]   object  Contact field of TypeObject type
			 */
			FieldItem(Model::ContactObject &object);

			/**
			 * @brief Set item selection mode and result type.
			 * @param[in]   mode    Selection mode
			 * @param[in]   type    Selection result type
			 */
			void setSelectMode(SelectMode mode, ResultType type);

			/**
			 * @brief Set selection callback.
			 * @param[in]   callback    Callback to be called when item is selected.
			 */
			void setSelectCallback(SelectCallback callback);

			/**
			 * @return ContactField associated with the item.
			 */
			Model::ContactField &getField() const;

			/**
			 * @return ContactObject associated with the item.
			 */
			Model::ContactObject &getObject() const;

			/**
			 * @return Item selection mode.
			 */
			SelectMode getSelectMode() const;

			/**
			 * @return Item selection result type.
			 */
			ResultType getResultType() const;

		protected:
			friend class DetailsView;

			/**
			 * @see GenlistItem::getItemClass()
			 */
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

			/**
			 * @see GenlistItem::getText()
			 */
			virtual char *getText(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistCheckItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onSelected()
			 */
			virtual void onSelected() override;

			/**
			 * @brief Should be called to report selection result.
			 * @param[in]   resultType  Selection result type
			 */
			void onSelected(unsigned resultType);

			/**
			 * @see ContactField::UpdatedCallback
			 */
			virtual void onFieldUpdated(Model::ContactField &field, contacts_changed_e change);

		private:
			Model::ContactObject &m_Object;
			Model::ContactField &m_Field;

			SelectMode m_SelectMode;
			ResultType m_ResultType;

			SelectCallback m_OnSelected;
		};
	}
}

#endif /* CONTACTS_DETAILS_FIELD_ITEM_H */
