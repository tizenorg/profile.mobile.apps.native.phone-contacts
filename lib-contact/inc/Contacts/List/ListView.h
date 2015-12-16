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

#ifndef CONTACTS_LIST_LIST_VIEW_H
#define CONTACTS_LIST_LIST_VIEW_H

#include "Ui/View.h"
#include "Contacts/List/Model/ContactProvider.h"
#include <map>

namespace Ui
{
	class Genlist;
	class GenlistItem;
}

namespace Contacts
{
	namespace List
	{
		class ContactItem;
		class ContactGroupItem;

		/**
		 * @brief Contacts list view
		 */
		class EXPORT_API ListView : public Ui::View
		{
		public:
			ListView();

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			Evas_Object *createIndex(Evas_Object *parent);

			virtual void onPageAttached() override;
			virtual void onCreated() override;

			void fillList();

			ContactGroupItem *insertGroupItem(const char *indexLetter,
					ContactGroupItem *nextGroup = nullptr);
			ContactGroupItem *getNextGroupItem(const char *indexLetter);

			ContactItem *insertItem(Model::ContactPtr contact, ContactGroupItem *group,
					ContactItem *nextItem = nullptr);
			ContactItem *getNextItem(ContactGroupItem *group, const Model::Contact &contact);

			void onItemSelected(Evas_Object *genlist, Elm_Object_Item *genlistItem);
			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onCreatePressed();
			virtual void onMenuPressed() override;

			void onContactInserted(Model::ContactPtr contact);

			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;
			std::map<std::string, ContactGroupItem *> m_Groups;

			Model::ContactProvider m_Provider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
