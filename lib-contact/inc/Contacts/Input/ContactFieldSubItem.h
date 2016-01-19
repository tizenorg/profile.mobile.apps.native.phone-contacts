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

#ifndef CONTACTS_INPUT_CONTACT_FIELD_SUB_ITEM_H
#define CONTACTS_INPUT_CONTACT_FIELD_SUB_ITEM_H

#include "Ui/GenlistItem.h"

namespace Ui
{
	class Control;
}

namespace Contacts
{
	namespace Model
	{
		class ContactField;
	}

	namespace Input
	{
		/**
		 * @brief Genlist item representing contact field of TypeText or TypeDate type.
		 */
		class ContactFieldSubItem : virtual public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Create field genlist item.
			 * @param[in]   field   Contact field of TypeText or TypeDate type
			 */
			ContactFieldSubItem(Model::ContactField &field);

			/**
			 * @return ContactField associated with the item.
			 */
			Model::ContactField &getField() const;

			/**
			 * @return Control that represents associated field.
			 */
			Ui::Control *getFieldControl() const;

			/**
			 * @see GenlistItem::isFocusable()
			 */
			virtual bool isFocusable() const override;

		protected:
			/**
			 * @see GenlistItem::getItemClass()
			 */
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

			/**
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::getState()
			 */
			virtual Eina_Bool getState(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onFocused()
			 */
			virtual void onFocused() override;

			/**
			 * @brief Create control that corresponds to represented field type.
			 * @param[in]   parent  Parent object
			 * @return Field control.
			 */
			Ui::Control *createFieldControl(Evas_Object *parent);

			/**
			 * @brief Enable entry "return" button to allow navigation to the next entry.
			 * @param[in]   entry   Entry to set up
			 */
			void enableEntryReturnButton(Evas_Object *entry);

		private:
			void onEntryFocused(Evas_Object *entry, void *eventInfo);
			void onEntryActivated(Evas_Object *entry, void *eventInfo);

			Model::ContactField &m_Field;
			Ui::GenlistItem *m_NextFocusItem;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_FIELD_SUB_ITEM_H */
