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

#include "Contacts/List/Model/PersonProvider.h"
#include "Ui/View.h"
#include "Utils/UniString.h"
#include <map>

namespace Ui
{
	class Genlist;
	class GenlistItem;
	class GenlistGroupItem;
}

namespace Contacts
{
	namespace List
	{
		class GroupItem;
		class PersonItem;
		class PersonGroupItem;

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
			void fillMyProfile();

			void insertMyProfileGroupItem();
			Ui::GenlistGroupItem *getNextMyProfileGroupItem();

			Elm_Index_Item *insertIndexItem(const char *indexLetter,
					Elm_Index_Item *nextItem = nullptr);

			PersonGroupItem *insertGroupItem(Utils::UniString indexLetter,
					PersonGroupItem *nextGroup = nullptr);
			void deleteGroupItem(PersonGroupItem *group);
			PersonGroupItem *getNextGroupItem(const Utils::UniString &indexLetter);

			PersonItem *createPersonItem(Model::PersonPtr person);
			void insertPersonItem(PersonItem *item);
			void updatePersonItem(PersonItem *item, Model::PersonPtr person);
			void deletePersonItem(PersonItem *item);
			PersonItem *getNextPersonItem(PersonGroupItem *group, const Model::Person &person);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onCreatePressed();
			virtual void onMenuPressed() override;

			void onPersonInserted(Model::PersonPtr person);
			void onPersonChanged(Model::PersonPtr person, contacts_changed_e changeType, PersonItem *item);

			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;

			GroupItem *m_MyProfileGroup;
			std::map<Utils::UniString, PersonGroupItem *> m_Groups;

			Model::PersonProvider m_Provider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
