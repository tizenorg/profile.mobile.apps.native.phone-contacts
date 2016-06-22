/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_LIST_REMOVE_FAV_VIEW_H
#define CONTACTS_LIST_REMOVE_FAV_VIEW_H

#include "Common/DataTypes.h"
#include "Ux/SelectView.h"

namespace Ui
{
	class Genlist;
	class GenlistGroupItem;
}

namespace Contacts
{
	namespace List
	{
		class ContactItem;

		/**
		 * @brief The Remove Favorites View
		 */
		class EXPORT_API RemoveFavView : public Ux::SelectView
		{
		public:
			/**
			 * @brief Create the view
			 */
			RemoveFavView();

		private:
			enum SectionId
			{
				SectionFirst,
				SectionFavorites = SectionFirst,
				SectionMfc,
				SectionMax
			};

			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onDestroy() override;

			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;

			Ui::GenlistGroupItem *createListSection(SectionId sectionId);
			void createSections();
			void insertSection(Ui::GenlistGroupItem *section, SectionId sectionId);
			void updateSection(SectionId sectionId);
			void updateSections();

			Ui::GenlistItem *getNextSectionItem(SectionId sectionId);

			void onSectionUpdated(ContactItem *item, ::Common::ChangeType change, SectionId sectionId);

			Ui::Genlist *m_Genlist;
			Ui::GenlistGroupItem *m_Sections[SectionMax];
		};
	}
}

#endif /* CONTACTS_LIST_REMOVE_FAV_VIEW_H */
