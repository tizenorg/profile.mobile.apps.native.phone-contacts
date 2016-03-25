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

#include "Contacts/SelectView.h"
#include "Logs/Model/LogGroup.h"

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
		class DetailsView : public Contacts::SelectView
		{
		public:
			/**
			 * @brief Create log details view.
			 * @param[in]   group   Log group
			 */
			DetailsView(Model::LogGroup *group);

		private:
			virtual ~DetailsView() override;
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual bool onBackPressed() override;
			virtual void onMenuPressed() override;

			virtual void onSelectModeChanged(Contacts::SelectMode selectMode) override;
			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;

			void fillGenList();
			void insertBasicInfoItem();
			void insertActionItem();
			void insertLogGroupItem();
			void insertLogDetailItem(Model::Log *log);
			void insertLogDetailItems();

			bool onSelected(Contacts::SelectResults results);
			bool onCanceled();
			void onGroupChanged(int type);
			void onLogAdded(Model::Log *log);
			void onLogRemoved(LogDetailItem *logItem);

			Model::LogGroup *m_Group;
			Ui::Genlist *m_Genlist;
			BasicInfoItem *m_BasicInfoItem;
			ActionItem *m_ActionItem;
			List::LogGroupItem *m_GroupItem;
			Model::LogGroup::ChangeCbHandle m_GroupChangeCbHandle;
		};
	}
}

#endif /* LOGS_DETAILS_DETAILS_VIEW_H */
