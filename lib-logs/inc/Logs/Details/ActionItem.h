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

#ifndef LOGS_DETAILS_ACTION_ITEM_H
#define LOGS_DETAILS_ACTION_ITEM_H

#include "Logs/Model/LogGroup.h"
#include "Ui/GenlistItem.h"
#include "Common/Actions.h"

namespace Logs
{
	namespace Model
	{
		class Log;
	}

	namespace Details
	{
		/**
		 * @brief Genlist item representing a item which value can be used to
		 *        initiate some external action.
		 */
		class ActionItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Create action item.
			 * @param[in]   log   Log for display
			 */
			ActionItem(Model::Log *log);

			/**
			 * @brief Update log and item fields.
			 * @param[in]   log    Log for display
			 */
			void updateLog(Model::Log *log);

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
			 * @see GenlistItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onSelected()
			 */
			virtual void onSelected() override;

		private:
			bool isSavedLog();
			char *getStrNumberType();
			Evas_Object *createEntryNumber(Evas_Object *parent);
			Evas_Object *createActionButton(Evas_Object *parent, Common::ActionType actionType);
			void executeAction(Common::ActionType actionType);
			void onButtonPressed(Evas_Object *button, void *eventInfo);

			Model::Log *m_Log;
			App::AppControl m_AppControl;
		};
	}
}

#endif /* LOGS_DETAILS_ACTION_ITEM_H */
