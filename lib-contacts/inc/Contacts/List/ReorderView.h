/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_LIST_REORDER_VIEW_H
#define CONTACTS_LIST_REORDER_VIEW_H

#include "Ui/View.h"

#include <vector>

namespace Ui
{
	class Genlist;
}

namespace Contacts
{
	namespace List
	{
		class ReorderView : public Ui::View
		{
		public:
			/**
			 * @brief Create new reorder favorites contact list view
			 */
			ReorderView();

		private:
			struct ReorderData
			{
				int personId;
				int prevPersonId;
				int nextPersonId;
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;

			void createPageButtons();

			void onReorder(Evas_Object *obj, void *event_info);
			void onCancelPressed(Evas_Object *button, void *eventInfo);
			void onDonePressed(Evas_Object *button, void *eventInfo);

			Evas_Object *m_CancelButton;
			Evas_Object *m_DoneButton;
			Ui::Genlist *m_Genlist;
			std::vector<ReorderData> m_ReorderDatas;
		};
	}
}



#endif /* CONTACTS_LIST_REORDER_VIEW_H */
