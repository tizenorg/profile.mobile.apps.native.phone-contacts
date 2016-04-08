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

#include "Contacts/Common/ContactSelectTypes.h"
#include "Contacts/List/Model/PersonProvider.h"

#include "Ux/SelectView.h"
#include "Utils/UniString.h"

#include <map>

namespace Ui
{
	class Genlist;
	class GenlistGroupItem;
}

namespace Contacts
{
	namespace List
	{
		class PersonItem;
		class PersonGroupItem;

		/**
		 * @brief Contacts list view
		 */
		class EXPORT_API ListView : public Ux::SelectView
		{
		public:
			/**
			 * @brief Create new person list view
			 * @param[in]   filterType  Defines how to filter person list
			 */
			explicit ListView(int filterType = FilterNone);
			virtual ~ListView() override;

		private:
			enum SectionId
			{
				SectionFirst,
				SectionMyProfile = SectionFirst,
				SectionFavorites,
				SectionMfc,
				SectionPerson,
				SectionMax
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;
			virtual void onCreated() override;
			virtual void onMenuPressed() override;

			void onDeleteSelected();
			void onShareSelected();

			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;
			virtual void onSelectModeChanged(Ux::SelectMode selectMode) override;

			void fillMyProfile();
			void fillFavorites();
			void fillMfc();
			void fillPersonList();

			void setIndexState(Evas_Object *layout, bool state);
			Ui::Genlist *createGenlist(Evas_Object *parent);
			Evas_Object *createNoContentLayout(Evas_Object *parent);
			void showNoContentLayout();
			void hideNoContentLayout();
			void updateNoContentLayout();

			void setFavouriteItemsMode(Ux::SelectMode selectMode);

			void addSection(SectionId sectionId);
			void removeSection(SectionId sectionId);
			Ui::GenlistItem *getNextSectionItem(SectionId currentSection);
			bool getSectionVisibility(Ux::SelectMode selectMode, SectionId sectionId);

			void updatePageMode();
			void updateSectionsMode();

			void createAddButton();
			void deleteAddButton();

			void insertMyProfileGroupItem();
			void updateMyProfileItem(const char *view_uri);

			Evas_Object *createIndex(Evas_Object *parent);
			Elm_Index_Item *insertIndexItem(const char *indexLetter,
					Elm_Index_Item *nextItem = nullptr);

			PersonGroupItem *insertPersonGroupItem(Utils::UniString indexLetter,
					PersonGroupItem *nextGroup = nullptr);
			void deletePersonGroupItem(PersonGroupItem *group);
			PersonGroupItem *getNextPersonGroupItem(const Utils::UniString &indexLetter);

			PersonItem *createPersonItem(Model::Person &person);
			void insertPersonItem(PersonItem *item);
			void updatePersonItem(PersonItem *item, int changes);
			void deletePersonItem(PersonItem *item);
			PersonItem *getNextPersonItem(PersonGroupItem *group, const Model::Person &person);

			virtual void onItemPressed(Ux::SelectItem *item) override;
			void onAddPressed(Evas_Object *button, void *eventInfo);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onPersonInserted(Contacts::Model::ContactData &person);
			void onPersonUpdated(PersonItem *item, int changes);
			void onPersonDeleted(PersonItem *item);

			Evas_Object *m_Box;
			Evas_Object *m_NoContent;
			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;
			Evas_Object *m_AddButton;

			Ui::GenlistGroupItem *m_Sections[SectionMax];

			std::map<Utils::UniString, PersonGroupItem *> m_PersonGroups;
			Model::PersonProvider m_Provider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
