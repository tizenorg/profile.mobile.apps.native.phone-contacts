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

#ifndef LOGS_LIST_LOG_VIEW_H
#define LOGS_LIST_LOG_VIEW_H

#include "Ui/View.h"
#include "Ui/Genlist.h"
#include "Logs/Model/LogProvider.h"
#include "Logs/List/LogItem.h"

#include <time.h>

namespace Logs
{
	namespace List
	{
		class LogGroupItem;

		/**
		 * @brief Log list view
		 */
		class EXPORT_API LogView : public Ui::View
		{
		public:
			/**
			 * @brief Create new log list view
			 * @param]in]   filterType    Defines how to filter log list
			 */
			LogView(Model::LogProvider::FilterType filterType = Model::LogProvider::None);

			virtual ~LogView();

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached() override;
			virtual void onMenuPressed() override;

			void fillGenlist();
			void insertLogGroupItem(const Model::Log *log, LogGroupItem **group,
					Ui::Genlist::Position position = Ui::Genlist::Before);

			LogGroupItem *createLogGroupItem(tm date, Ui::Genlist::Position position = Ui::Genlist::Before);
			LogItem *createLogItem(Model::LogGroup *group);

			bool compareTime(const tm itemDate, const tm nowDate);

			void setMode(LogItem::Mode mode);

			Model::LogProvider *m_LogProvider;
			Ui::Genlist *m_Genlist;
			LogItem::Mode m_Mode;
		};
	}
}

#endif /* LOGS_LIST_LOG_VIEW_H */
