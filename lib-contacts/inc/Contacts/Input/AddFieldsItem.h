/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_INPUT_ADD_FIELDS_ITEM_H
#define CONTACTS_INPUT_ADD_FIELDS_ITEM_H

#include "Contacts/Model/ContactFields.h"
#include "Ui/GenItem.h"
#include <functional>

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief Genlist item with buttons for adding contact fields.
		 */
		class AddFieldsItem : public Ui::GenItem
		{
		public:
			/**
			 * @brief Add field callback.
			 * @param[in]   ID of field to add
			 */
			typedef std::function<void(Model::ContactFieldId)> AddFieldCallback;

			AddFieldsItem();

			/**
			 * @brief Set add field callback.
			 * @param[in]   callback    Callback to be called when add field requested
			 */
			void setAddFieldCallback(AddFieldCallback callback);

			/**
			 * @brief Set whether field can be added or not.
			 * @param[in]   fieldId     Field ID to set the state for
			 * @param[in]   isEnabled  Whether option to add field is enabled
			 */
			void setAddFieldState(Model::ContactFieldId fieldId, bool isEnabled);

		private:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual void onUnrealized() override;

			Evas_Object *createButton(Evas_Object *parent, const char *text,
					const char *iconPath, Evas_Smart_Cb callback, void *data);
			Evas_Object *createAddFieldButton(Evas_Object *parent,
					Model::ContactFieldId fieldId, const char *iconPath);

			void onAddField(Model::ContactFieldId fieldId);
			void onAddButtonPressed(Evas_Object *button, void *eventInfo);
			void onMoreButtonPressed(Evas_Object *button, void *eventInfo);

			AddFieldCallback m_OnAddField;
			bool m_AddFieldStates[Model::FieldEnd];
			Evas_Object *m_Buttons[Model::FieldEnd];
		};
	}
}

#endif /* CONTACTS_INPUT_ADD_FIELDS_ITEM_H */
