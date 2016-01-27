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
#include "Contacts/List/PersonItem.h"
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
			/**
			 * @brief Represents view mode
			 */
			enum Mode
			{
				ModeDefault,
				ModeMultipick,
				ModeMax
			};

			/**
			 * @brief Create new person list view
			 * @param]in]   personFilter    Defines how to filter person list
			 */
			ListView(Model::PersonProvider::FilterType personFilter =
					Model::PersonProvider::FilterNone);
			virtual ~ListView() override;

			/**
			 * @brief Set view mode
			 * @details View can be in Default, SinglePick or Multipick mode
			 * @param[in]   mode    View mode
			 */
			void setMode(Mode mode);

		private:
			enum SectionId
			{
				SectionFirst,
				SectionSearch = SectionFirst,
				SectionSelectAll,
				SectionMyProfile,
				SectionFavorites,
				SectionMfc,
				SectionPerson,
				SectionMax
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached() override;
			virtual void onCreated() override;
			virtual void onMenuPressed() override;

			void fillList();
			void fillSearchItem();
			void fillSelectAllItem();
			void fillMyProfile();
			void fillFavorites();
			void fillMfc();
			void fillPersonList();

			void setFavouriteItemsMode(PersonItem::Mode mode);
			void setPersonItemsMode(PersonItem::Mode mode);

			void addSection(SectionId sectionId);
			void removeSection(SectionId sectionId);
			Ui::GenlistGroupItem *getNextSectionItem(SectionId currentSection);
			bool getSectionVisibility(Mode mode, SectionId sectionId);

			PersonItem::Mode getItemMode(Mode viewMode);

			void updateTitle();

			void createNewContactButton();
			void deleteNewContactButton();

			void insertMyProfileGroupItem();
			void updateMyProfileItem(const char *view_uri);

			Evas_Object *createIndex(Evas_Object *parent);
			Elm_Index_Item *insertIndexItem(const char *indexLetter,
					Elm_Index_Item *nextItem = nullptr);

			PersonGroupItem *insertPersonGroupItem(Utils::UniString indexLetter,
					PersonGroupItem *nextGroup = nullptr);
			void deletePersonGroupItem(PersonGroupItem *group);
			PersonGroupItem *getNextPersonGroupItem(const Utils::UniString &indexLetter);

			PersonItem *createPersonItem(Model::PersonPtr person);
			void insertPersonItem(PersonItem *item);
			void updatePersonItem(PersonItem *item, Model::PersonPtr person);
			void deletePersonItem(PersonItem *item);
			PersonItem *getNextPersonItem(PersonGroupItem *group, const Model::Person &person);

			void launchPersonDetail(PersonItem *item);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onCreatePressed();

			void onPersonInserted(Model::PersonPtr person);
			void onPersonChanged(Model::PersonPtr person, contacts_changed_e changeType, PersonItem *item);

			void onItemChecked(PersonItem *item);

			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;

			Ui::GenlistGroupItem *m_Sections[SectionMax];
			std::map<Utils::UniString, PersonGroupItem *> m_PersonGroups;

			Model::PersonProvider m_Provider;

			Mode m_ViewMode;

			size_t m_PersonCount;
			size_t m_CheckedCount;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
