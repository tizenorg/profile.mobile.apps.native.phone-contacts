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

#include "Logs/Model/LogGroup.h"
#include "Ux/SelectItem.h"

#define PART_LOG_TIME           "elm.text.sub.end"

namespace Logs
{
	namespace Model
	{
		class LogGroup;
	}

	namespace List
	{
		class LogItem : public Ux::SelectItem
		{
		public:
			/**
			 * @brief Item delete callback
			 */
			typedef std::function<void(LogItem *item)> DeleteCallback;

			/**
			 * @brief Item details callback
			 */
			typedef std::function<void(LogItem *item)> DetailsCallback;

			/**
			 * @brief Create log item
			 * @param[in]   group       Log group
			 */
			LogItem(Model::LogGroup *group);
			virtual ~LogItem() override;

			/**
			 * @brief Set delete item callback
			 * @param[in]   callback    Delete callback
			 */
			void setDeleteCallback(DeleteCallback callback);

			/**
			 * @brief Get group
			 */
			Model::LogGroup *getGroup() const;

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual Ux::SelectResult getDefaultResult() const override;
			virtual void onSelected() override;
			void onInfoIconPressed();

			Evas_Object *createThumbnail(Evas_Object *parent);
			Evas_Object *createLayout(Evas_Object *parent, const char *layoutName);
			Evas_Object *createIcon(Evas_Object *parent, const char *path);
			const char *getImagePath(int type);

			void updateItem(int type);
			void setUpdateCallback();

			DeleteCallback m_OnDelete;
			DetailsCallback m_OnDetails;
			Model::LogGroup *m_Group;
			Model::LogGroup::ChangeCbHandle m_GroupChangeCbHandle;
		};
	}
}

#endif /* LOGS_LIST_LOG_ITEM_H */
