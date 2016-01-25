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

#ifndef CONTACTS_LIST_PERSON_ITEM_H
#define CONTACTS_LIST_PERSON_ITEM_H

#include "Contacts/List/Model/Person.h"
#include "Ui/GenlistItem.h"
#include <functional>

#define PART_PERSON_NAME        "elm.text"
#define PART_PERSON_THUMBNAIL   "elm.swallow.icon"
#define PART_CHECK              "elm.swallow.end"

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Person list item
		 */
		class PersonItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Item select callback
			 */
			typedef std::function<void()> SelectedCallback;

			/**
			 * @brief Represents item mode
			 */
			enum ItemMode
			{
				DefaultMode,    /*< Usual mode */
				PickMode        /*< Mode with ability to select item */
			};

			/**
			 * @brief Create person item
			 * @param[in]   person      Person object
			 * @param[in]   mode        Item mode
			 */
			explicit PersonItem(Model::PersonPtr person, ItemMode mode = DefaultMode);

			/**
			 * @brief Set item mode
			 * @param[in]   mode    Item mode
			 */
			void setMode(ItemMode mode);

			/**
			 * @remark Used in item pick mode
			 * @return true if item is checked, otherwise false
			 */
			bool isChecked() const;

			/**
			 * @return Person object
			 */
			const Model::Person &getPerson() const;

			/**
			 * @brief Set person object
			 * @param[in]   person      Person object
			 */
			void setPerson(Model::PersonPtr person);

			/**
			 * @brief Set item selected callback
			 * @param[in]   callback    Callback, that will be invoked on item selection
			 */
			void setSelectedCallback(SelectedCallback callback);

		protected:
			virtual void onSelected() override;

		private:
			Elm_Check *getCheck() const;

			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Model::PersonPtr m_Person;

			ItemMode m_ItemMode;
			bool m_Checked;

			SelectedCallback m_OnSelected;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_ITEM_H */
