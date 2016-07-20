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
	class GenGroupItem;
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
			 * @brief Sections types
			 */
			enum SectionId
			{
				SectionFirst,
				SectionMyProfile = SectionFirst,
				SectionFavorites,
				SectionMfc,
				SectionMax
			};

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

			/**
			 * @brief Set section visibility
			 * @param[in]   section  Section
			 * @param[in]   isVisible Visibility
			 */
			void setSectionVisibility(SectionId section, bool isVisible);

			/**
			 * @brief Set add button visibility
			 * @param[in]   isVisible   Visibility
			 */
			void setAddButtonVisibility(bool isVisible);

			/**
			 * @brief Set help text for no content layout.
			 * @param[in]   text    Help text
			 */
			void setNoContentHelpText(const char *text);

		protected:
			Model::PersonProvider *getProvider() const;

			/**
			 * @brief Called when updating is finished.
			 */
			virtual void onUpdateFinished();

		private:
			struct Section
			{
				Ui::GenGroupItem *m_Item = nullptr;
				bool m_IsVisible = true;
			};

			enum State
			{
				StateSearching = 1 << 0,
				StateSelecting = 1 << 1
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onDestroy() override;

			virtual void onNavigation(bool isCurrent) override;
			virtual bool onBackPressed() override;
			virtual void onMenuPressed() override;

			void onDeleteSelected();
			void onShareSelected();
			bool onSelectFinished();
			void onManageFavoritesSelected();

			virtual void onSelectAllInsert(Ui::GenItem *item) override;
			virtual void onSelectModeChanged(Ux::SelectMode selectMode) override;

			void setState(State state, bool isEnabled);

			Ui::GenGroupItem *createMyProfileSection();
			Ui::GenGroupItem *createListSection(SectionId sectionId);
			void fillPersonList();

			Ui::Genlist *createGenlist(Evas_Object *parent);
			Evas_Object *createEmptyLayout(Evas_Object *parent);
			void updateEmptyLayout();
			void updateEmptyState();

			Ui::GenGroupItem *createSection(SectionId sectionId);
			void insertSection(Ui::GenGroupItem *section, SectionId sectionId);
			void updateSection(SectionId sectionId);
			void updateSections();

			Ui::GenItem *getNextSectionItem(SectionId sectionId);
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

			std::string m_NoContentHelpText;

			bool m_IsCurrentView;
			bool m_IsSearching;
			bool m_IsEmpty;
			bool m_HasAddButton;
			std::vector<State> m_StateHistory;

			SearchItem *m_SearchItem;
			Section m_Sections[SectionMax];

			std::map<Utils::UniString, PersonGroupItem *> m_PersonGroups;
			Model::PersonProvider *m_PersonProvider;
			Model::PersonSearchProvider m_SearchProvider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
