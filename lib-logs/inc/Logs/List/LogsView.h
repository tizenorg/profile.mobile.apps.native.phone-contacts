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
#include <system_settings.h>

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
			 * @brief Determines how to filter log list
			 */
			enum FilterType
			{
				FilterAll,
				FilterMissed
			};

			/**
			 * @brief Create new log list view
			 * @param[in]   filterType    Defines how to filter log list
			 */
			LogsView(FilterType filterType = FilterAll);
			virtual ~LogsView() override;

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onPageAttached() override;
			virtual void onMenuPressed() override;

			void fillLayout();
			void updateLayout(bool isEmpty);
			Evas_Object *createNoContentsLayout(Evas_Object *parent);
			void fillGenlist();

			bool shouldDisplayLogs(const Model::LogGroup *logGroup) const;
			LogItem *createLogItem(Model::LogGroup *logGroup);
			LogItem *insertLogItem(Model::LogGroup *logGroup);
			LogGroupItem *insertLogGroupItem(tm date);

			void setMode(ItemMode mode);

			void onSettingsChanged(system_settings_key_e key);
			void onSelectViewBy();
			void onLogInserted(Model::LogGroup *group);
			void onLogItemDelete(LogItem *item);

			Model::LogProvider m_LogProvider;
			ItemMode m_Mode;
			FilterType m_FilterType;

			Ui::Genlist *m_Genlist;
			LogGroupItem *m_LastGroupItem;
		};
	}
}

#endif /* LOGS_LIST_LOGS_VIEW_H */
