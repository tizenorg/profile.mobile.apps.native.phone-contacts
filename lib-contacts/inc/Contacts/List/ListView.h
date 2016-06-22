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
#include "Contacts/List/Model/PersonSearchProvider.h"

#include "Common/DataTypes.h"
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
		class SearchItem;
		class PersonGroupItem;
		class PersonItem;
		class ContactItem;

		/**
		 * @brief Contacts list view
		 */
		class EXPORT_API ListView : public Ux::SelectView
		{
		public:
			/**
			 * @brief Create person list view
			 * @param[in]   provider    Person provider
			 */
			explicit ListView(Model::PersonProvider *provider);

			/**
			 * @brief Create person list view
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
				SectionMax
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onDestroy() override;

			virtual void onNavigation(bool isCurrent) override;
			virtual bool onBackPressed() override;
			virtual void onMenuPressed() override;

			void onDeleteSelected();
			void onShareSelected();

			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;
			virtual void onSelectModeChanged(Ux::SelectMode selectMode) override;

			Ui::GenlistGroupItem *createMyProfileSection();
			Ui::GenlistGroupItem *createListSection(SectionId sectionId);
			void fillPersonList();

			Ui::Genlist *createGenlist(Evas_Object *parent);
			Evas_Object *createEmptyLayout(Evas_Object *parent);
			void setEmptyState(bool isEmpty);

			Ui::GenlistGroupItem *createSection(SectionId sectionId);
			void insertSection(Ui::GenlistGroupItem *section, SectionId sectionId);
			void updateSection(SectionId sectionId);
			void updateSections();

			Ui::GenlistItem *getNextSectionItem(SectionId sectionId);
			bool getSectionVisibility(SectionId sectionId);

			SearchItem *createSearchItem();
			Evas_Object *createAddButton(Evas_Object *parent);
			void updateAddButton();

			Evas_Object *createIndex(Evas_Object *parent);
			void updateIndex();
			Elm_Index_Item *insertIndexItem(const char *indexLetter,
					Elm_Index_Item *nextItem = nullptr);

			PersonGroupItem *getPersonGroupItem(const Utils::UniString &indexLetter);
			PersonGroupItem *insertPersonGroupItem(const Utils::UniString &indexLetter,
					PersonGroupItem *nextGroupItem = nullptr);
			void deletePersonGroupItem(PersonGroupItem *group);

			PersonItem *createPersonItem(Model::PersonSearchData &searchData);
			void insertPersonItem(PersonItem *item);
			void updatePersonItem(PersonItem *item, int changes);
			void deletePersonItem(PersonItem *item);
			PersonItem *getNextPersonItem(PersonGroupItem *group, const Model::Person &person);
			void linkPersonItems(PersonItem *sectionItem);

			void updateReorderedItem(int reorderedId, int previousId);

			void onAddPressed(Evas_Object *button, void *eventInfo);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onPersonInserted(::Model::DataItem &data);
			void onSectionUpdated(ContactItem *item, ::Common::ChangeType change, SectionId sectionId);
			void onSearchChanged(const char *str);

			Evas_Object *m_Box;
			Evas_Object *m_NoContent;
			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;
			Evas_Object *m_AddButton;
			bool m_IsCurrentView;
			bool m_IsSearching;

			SearchItem *m_SearchItem;
			Ui::GenlistGroupItem *m_Sections[SectionMax];

			std::map<Utils::UniString, PersonGroupItem *> m_PersonGroups;
			Model::PersonProvider *m_PersonProvider;
			Model::PersonSearchProvider m_SearchProvider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
