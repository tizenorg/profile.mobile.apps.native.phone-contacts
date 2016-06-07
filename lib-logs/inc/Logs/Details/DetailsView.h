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

#ifndef LOGS_DETAILS_DETAILS_VIEW_H
#define LOGS_DETAILS_DETAILS_VIEW_H

#include "Logs/Model/LogGroup.h"
#include "Ux/SelectView.h"
#include "Logs/Model/NumberLogProvider.h"

namespace Ui
{
	class Genlist;
}

namespace Logs
{
	namespace Model
	{
		class Log;
	}

	namespace List
	{
		class LogGroupItem;
	}

	namespace Details
	{
		class BasicInfoItem;
		class ActionItem;
		class LogDetailItem;

		/**
		 * @brief Log details view.
		 */
		class DetailsView : public Ux::SelectView
		{
		public:
			/**
			 * @brief Create log details view.
			 * @param[in]   log   Log
			 */
			DetailsView(Model::Log *log);

		private:
			virtual ~DetailsView() override;
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual bool onBackPressed() override;
			virtual void onMenuPressed() override;

			virtual void onSelectModeChanged(Ux::SelectMode selectMode) override;
			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;

			void fillGenList();
			void insertBasicInfoItem();
			void insertActionItem();
			void insertLogGroupList();
			void insertGroupAndLogDetailItems(Model::LogGroup *group);
			void insertLogGroupItem(tm date);
			void insertLogDetailItems(Model::LogGroup *group);
			void insertLogDetailItem(Model::Log *log);

			List::LogGroupItem *getLastGroupItem();
			void setLastGroupItem(List::LogGroupItem *groupItem);

			bool onSelected(Ux::SelectResults results);
			bool onCanceled();

			void onLogGroupInserted(Model::LogGroup *group);
			void onGroupChanged(int type);
			void onLogAdded(Model::Log *log);
			void onLogRemoved(LogDetailItem *logItem);

			Model::Log *m_Log;
			Model::NumberLogProvider *m_LogProvider;
			Ui::Genlist *m_Genlist;
			BasicInfoItem *m_BasicInfoItem;
			ActionItem *m_ActionItem;
			List::LogGroupItem *m_LastGroupItem;
		};
	}
}

#endif /* LOGS_DETAILS_DETAILS_VIEW_H */
