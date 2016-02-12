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

#ifndef CONTACTS_DETAILS_ACTION_FIELD_ITEM_H
#define CONTACTS_DETAILS_ACTION_FIELD_ITEM_H

#include "Contacts/Details/TypedFieldItem.h"
#include "App/AppControl.h"

namespace Contacts
{
	namespace Details
	{
		/**
		 * @brief Genlist item representing a field which value can be used to
		 *        initiate some external action.
		 */
		class ActionFieldItem : public TypedFieldItem
		{
		public:
			enum ActionId
			{
				ActionCall    = Contacts::ActionCall,    /**< Make a telephony call using value as a number */
				ActionMessage = Contacts::ActionMessage, /**< Compose an SMS using value as recipient */
				ActionEmail   = Contacts::ActionEmail,   /**< Compose an E-mail using value as an address */
				ActionUrl                                /**< Open webpage in browser using value as URL */
			};

			/**
			 * @brief Create action item.
			 * @param[in]   object      Contact field of TypeObject type
			 * @param[in]   actionId    Action to perform using object's first field value
			 */
			ActionFieldItem(Model::ContactObject &object, ActionId actionId);

		protected:
			using FieldItem::onSelected;

			/**
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onSelected()
			 */
			virtual void onSelected() override;

			/**
			 * @brief Create action button.
			 * @param[in]   parent      Parent widget
			 * @param[in]   actionId    Button's action ID
			 * @return Action button.
			 */
			Evas_Object *createActionButton(Evas_Object *parent, ActionId actionId);

		private:
			void showActionPopup();
			void executeAction(ActionId actionId);
			void onButtonPressed(Evas_Object *button, void *eventInfo);

			ActionId m_ActionId;
			App::AppControl m_AppControl;
		};
	}
}

#endif /* CONTACTS_DETAILS_ACTION_FIELD_ITEM_H */
