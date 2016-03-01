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

#ifndef LOGS_DETAILS_BASIC_INFO_ITEM_H
#define LOGS_DETAILS_BASIC_INFO_ITEM_H

#include "Ui/GenlistItem.h"
#include "App/AppControl.h"

namespace Logs
{
	namespace Model
	{
		class LogGroup;
		class Log;
	}

	namespace Details
	{
		/**
		 * @brief Genlist item displaying contact's image, name.
		 */
		class BasicInfoItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Back button pressed callback
			 */
			typedef std::function<void()> BackCallback;

			/**
			 * @brief Create basic info item.
			 * @param[in]   group    Log group for display
			 */
			BasicInfoItem(Model::LogGroup *group);

			/**
			 * @brief Set "back" button callback.
			 * @param[in]   callback    "back" button callback
			 */
			void setBackCallback(BackCallback callback);

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
			 * @see GenlistItem::getState()
			 */
			virtual Eina_Bool getState(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenlistItem::onSelected()
			 */
			virtual void onSelected() override;

		private:
			bool isSavedLog();
			Evas_Object *createBackButton(Evas_Object *parent);
			Evas_Object *createUnsavedButtons(Evas_Object *parent);
			Evas_Object *createUnsavedButton(Evas_Object *box, const char *text, Evas_Smart_Cb func);

			void onBackPressed(Evas_Object *button, void *eventInfo);
			void onCreatePressed();
			void onUpdatePressed();

			Model::LogGroup *m_Group;
			Model::Log *m_Log;
			App::AppControl m_AppControl;
			BackCallback m_OnBackPressed;
		};
	}
}

#endif /* LOGS_DETAILS_BASIC_INFO_ITEM_H */
