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

#include "Ui/View.h"

namespace Ui
{
	class Genlist;
}

namespace Logs
{
	namespace Model
	{
		class LogGroup;
	}
	namespace Details
	{
		/**
		 * @brief Log details view.
		 */
		class DetailsView : public Ui::View
		{
		public:

			/**
			 * @brief Create log details view.
			 */
			DetailsView(Model::LogGroup *group);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;

			Model::LogGroup *m_Group;
			Ui::Genlist *m_Genlist;
		};
	}
}

#endif /* LOGS_DETAILS_DETAILS_VIEW_H */
