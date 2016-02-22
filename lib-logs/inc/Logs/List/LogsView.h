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

#ifndef LOGS_LIST_LOGS_VIEW_H
#define LOGS_LIST_LOGS_VIEW_H

#include "Ui/View.h"
#include "Logs/Model/LogProvider.h"

namespace Ui
{
	class Genlist;
}

namespace Logs
{
	namespace List
	{
		class LogGroupItem;
		class LogItem;
		enum class ItemMode;

		/**
		 * @brief Logs list view
		 */
		class EXPORT_API LogsView : public Ui::View
		{
		public:
			/**
			 * @brief Create new log list view
			 * @param[in]   filterType    Defines how to filter log list
			 */
			LogsView(Model::LogProvider::FilterType filterType = Model::LogProvider::FilterAll);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached() override;
			virtual void onMenuPressed() override;

			void onSelectViewBy();
			void fillGenlist();
			void onLogInserted(Model::LogGroup *group);
			LogGroupItem *insertLogItem(Model::LogGroup *group, LogGroupItem *groupItem);

			LogGroupItem *createLogGroupItem(tm date);
			LogItem *createLogItem(Model::LogGroup *group);

			void setMode(ItemMode mode);

			Model::LogProvider m_LogProvider;
			Ui::Genlist *m_Genlist;
			ItemMode m_Mode;
		};
	}
}

#endif /* LOGS_LIST_LOGS_VIEW_H */
