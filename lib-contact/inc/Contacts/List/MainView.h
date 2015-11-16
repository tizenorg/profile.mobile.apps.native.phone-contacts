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

#ifndef CONTACTS_LIST_MAIN_VIEW_H
#define CONTACTS_LIST_MAIN_VIEW_H

#include "Ui/View.h"

namespace Ui
{
	class Genlist;
	class GenlistItem;
}

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Contacts list view
		 */
		class EXPORT_API MainView : public Ui::View
		{
		public:
			MainView();
			virtual ~MainView() override;

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			Evas_Object *createIndex(Evas_Object *parent);

			virtual void onPageAttached() override;
			virtual void onCreated() override;

			void fillList();
			Ui::GenlistItem *appendGroupItem(const char *indexLetter);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);
			void onCreatePressed();
			void onDatabaseChanged(const char *uri);

			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;
		};
	}
}

#endif /* CONTACTS_LIST_MAIN_VIEW_H */
