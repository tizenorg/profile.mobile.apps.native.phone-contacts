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

#ifndef CONTACTS_DETAILS_ACTION_FIELD_ITEM_H
#define CONTACTS_DETAILS_ACTION_FIELD_ITEM_H

#include "Common/Actions.h"
#include "Contacts/Details/TypedFieldItem.h"

namespace Contacts
{
	namespace Details
	{
		/**
		 * @brief Genlist item representing a field which value can be used to
		 *        initiate some external action.
		 */
		class ActionFieldItem : virtual public FieldItem
		{
		public:
			/**
			 * @brief Create action item.
			 * @param[in]   object      Contact field of TypeObject type
			 * @param[in]   actionType  Action to perform using object's first field value
			 */
			ActionFieldItem(Model::ContactObject &object, Common::ActionType actionType);

		protected:
			/**
			 * @see GenItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenItem::onSelected()
			 */
			virtual void onSelected() override;

			/**
			 * @see SelectItem::onSelectModeChanged()
			 */
			virtual void onSelectModeChanged(Ux::SelectMode selectMode) override;

			/**
			 * @brief Create action button.
			 * @param[in]   parent      Parent widget
			 * @param[in]   actionId    Button's action ID
			 * @return Action button.
			 */
			Evas_Object *createActionButton(Evas_Object *parent, Common::ActionType actionType);

		private:
			void executeAction(Common::ActionType actionType);
			void onButtonPressed(Evas_Object *button, void *eventInfo);

			Common::ActionType m_ActionType;
			App::AppControl m_AppControl;
		};
	}
}

#endif /* CONTACTS_DETAILS_ACTION_FIELD_ITEM_H */
