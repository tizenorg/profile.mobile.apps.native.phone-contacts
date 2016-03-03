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
		class SelectAllItem;
		class PersonGroupItem;
		class PersonItem;

		namespace Model
		{
			class Person;
		}

		/**
		 * @brief Contacts list view
		 */
		class EXPORT_API ListView : public Ui::View
		{
		public:
			/**
			 * @brief Create new person list view
			 * @param]in]   filterType  Defines how to filter person list
			 */
			ListView(int filterType = FilterNone);
			virtual ~ListView() override;

			/**
			 * @brief Set selection mode.
			 * @param[in]   selectMode  Item selection mode
			 */
			void setSelectMode(SelectMode selectMode);

			/**
			 * @brief Set item selection limit
			 * @remark Used only in ModeMultiSelectWithLimit view mode
			 * @param[in]   count   Maximal selectable items count
			 */
			void setSelectLimit(size_t count);

			/**
			 * @brief Set view result callback
			 * @remark Used in Singlepick/Multipick view modes
			 * @param[in]   callback    Done pressed callback
			 */
			void setSelectCallback(SelectCallback callback);

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

			void fillSearchItem();
			void fillSelectAllItem();
			void fillMyProfile();
			void fillFavorites();
			void fillMfc();
			void fillPersonList();

			void setFavouriteItemsMode(SelectMode selectMode);
			void setPersonItemsMode(SelectMode selectMode);

			void addSection(SectionId sectionId);
			void removeSection(SectionId sectionId);
			Ui::GenlistItem *getNextSectionItem(SectionId currentSection);
			bool getSectionVisibility(SelectMode selectMode, SectionId sectionId);

			void updateTitle();
			void updateSelectAllState();
			void updatePageMode();
			void updateSectionsMode();

			void createAddButton();
			void deleteAddButton();
			void createPageButtons();
			void deletePageButtons();

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

			void launchPersonDetail(PersonItem *item);

			void onIndexChanged(Evas_Object *index, Elm_Object_Item *indexItem);
			void onIndexSelected(Evas_Object *index, Elm_Object_Item *indexItem);

			void onAddPressed(Evas_Object *button, void *eventInfo);
			void onDonePressed(Evas_Object *button, void *eventInfo);
			void onCancelPressed(Evas_Object *button, void *eventInfo);

			void onPersonItemInserted(PersonItem *item);
			void onPersonItemDelete(PersonItem *item);

			void onPersonInserted(ContactData &person);
			void onPersonUpdated(PersonItem *item, int changes);
			void onDeleted(PersonItem *item);
			void onPersonSelected(const Model::Person &person);

			void onSelectAllChecked(bool isChecked);
			void onItemChecked(PersonItem *item, bool isChecked);
			void onItemSelected(PersonItem *item);

			Ui::Genlist *m_Genlist;
			Evas_Object *m_Index;

			Evas_Object *m_AddButton;
			Evas_Object *m_DoneButton;
			Evas_Object *m_CancelButton;

			Ui::GenlistItem *m_Sections[SectionMax];
			std::map<Utils::UniString, PersonGroupItem *> m_PersonGroups;

			Model::PersonProvider m_Provider;

			size_t m_PersonCount;
			size_t m_SelectCount;
			size_t m_SelectLimit;

			SelectMode m_SelectMode;
			SelectCallback m_OnSelected;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_VIEW_H */
