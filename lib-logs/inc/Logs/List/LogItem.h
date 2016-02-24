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

#ifndef LOGS_LIST_LOG_ITEM_H
#define LOGS_LIST_LOG_ITEM_H

#include "Ui/GenlistCheckItem.h"

namespace Logs
{
	namespace Model
	{
		class LogGroup;
	}
	namespace List
	{
		/**
		 * @brief Represents item mode
		 */
		enum class ItemMode
		{
			Default,    /*< Usual mode */
			Pick        /*< Mode with ability to select item */
		};

		class LogItem : public Ui::GenlistCheckItem
		{
		public:
			/**
			 * @brief Item delete callback
			 */
			typedef std::function<void(LogItem *item)> DeleteCallback;

			typedef std::function<void(Model::LogGroup *group)> DetailsCallback;

			/**
			 * @brief Create log item
			 * @param[in]   group       Log group
			 * @param[in]   mode        Item mode
			 */
			LogItem(Model::LogGroup *group, ItemMode mode = ItemMode::Default);

			/**
			 * @brief Set item mode
			 * @param[in]   mode    Item mode
			 */
			void setMode(ItemMode mode);

			/**
			 * @brief Set delete item callback
			 * @param[in]   callback    Delete callback
			 */
			void setDeleteCallback(DeleteCallback callback);

			/**
			 * @brief Set log details callback
			 * @param[in]   callback    Details callback
			 */
			void setDetailsCallback(DetailsCallback callback);

			/**
			 * @brief Remove group
			 */
			void removeGroup();

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Evas_Object *createThumbnail(Evas_Object *parent);
			Evas_Object *createLayout(Evas_Object *parent, const char *layoutName);
			Evas_Object *createIcon(Evas_Object *parent, const char *path);
			const char *getImagePath(int type);
			void onInfoIconPressed();

			void updateItem();
			void setUpdateCallback();

			DeleteCallback m_OnDelete;
			DetailsCallback m_OnDetails;
			Model::LogGroup *m_Group;
			ItemMode m_Mode;
		};
	}
}

#endif /* LOGS_LIST_LOG_ITEM_H */
