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

#ifndef LOGS_MODEL_LOG_ITEM_H
#define LOGS_MODEL_LOG_ITEM_H

#include "Ui/GenlistCheckItem.h"
#include "Logs/Model/LogGroup.h"
#include <vector>

#define PART_LOG_NAME           "elm.text"
#define PART_LOG_NUMBER         "elm.text.sub"
#define PART_LOG_COUNT          "elm.text.end"
#define PART_LOG_TYPE           "elm.text.sub.end"

#define PART_PERSON_THUMBNAIL   "elm.swallow.icon"
#define PART_END                "elm.swallow.end"

namespace Logs
{
	namespace List
	{
		using namespace Logs::Model;

		class LogItem : public Ui::GenlistCheckItem
		{
		public:
			/**
			 * @brief Item delete callback
			 */
			typedef std::function<void(LogItem *item)> DeleteCallback;
			/**
			 * @brief Represents item mode
			 */
			enum Mode
			{
				ModeDefault,    /*< Usual mode */
				ModePick        /*< Mode with ability to select item */
			};

			/**
			 * @brief Create log item
			 * @param[in]   group       Log group
			 * @param[in]   mode        Item mode
			 */
			LogItem(LogGroup *group, Mode mode);

			/**
			 * @brief Set item mode
			 * @param[in]   mode    Item mode
			 */
			void setMode(LogItem::Mode mode);

			/**
			 * @brief Set delete item callback
			 * @param[in]   callback    Delete callback
			 */
			void setDeleteCallback(DeleteCallback callback);

			/**
			 * @brief Remove group
			 */
			void removeGroup();

		private:
			virtual char *getText(Evas_Object *parent, const char *part);
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part);

			Evas_Object *createThumbnail(Evas_Object *parent);
			Evas_Object *createLayout(Evas_Object *parent, const char *layoutName);
			Evas_Object *createIcon(Evas_Object *parent, const char *path);
			const char *getImagePath(int type);

			void updateItem();
			void setUpdateCallback();

			DeleteCallback m_OnDelete;
			LogGroup *m_Group;
			Mode m_Mode;
		};
	}
}

#endif /* LOGS_MODEL_LOG_ITEM_H */
