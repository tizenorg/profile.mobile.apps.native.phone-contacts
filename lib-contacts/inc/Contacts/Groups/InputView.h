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

#ifndef CONTACTS_GROUPS_INPUT_VIEW_H
#define CONTACTS_GROUPS_INPUT_VIEW_H

#include "Ui/View.h"

namespace Ui
{
	class Genlist;
}

namespace Contacts
{
	namespace Groups
	{
		class AddMembersItem;
		class NameItem;
		class RingtoneItem;

		namespace Model
		{
			class Group;
		}

		/**
		 * @brief Create/Edit group view.
		 */
		class InputView : public Ui::View
		{
		public:
			/**
			 * @brief Constructor.
			 * @param[in]   id  Group id
			 */
			InputView(int id = 0);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;

			void createPageButtons();

			void onCancelPressed(Evas_Object *button, void *eventInfo);
			void onDonePressed(Evas_Object *button, void *eventInfo);
			void onNameFilled(bool isFilled);

			bool isAlreadyExists();
			void save();

			int m_Id;
			Evas_Object *m_CancelButton;
			Evas_Object *m_DoneButton;
			Ui::Genlist *m_Genlist;

			AddMembersItem *m_AddMembersItem;
			NameItem *m_NameItem;
			RingtoneItem *m_RingtoneItem;
		};
	}
}

#endif /* CONTACTS_GROUPS_INPUT_VIEW_H */
