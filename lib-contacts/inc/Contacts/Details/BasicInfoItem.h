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

#ifndef CONTACTS_DETAILS_BASIC_INFO_ITEM_H
#define CONTACTS_DETAILS_BASIC_INFO_ITEM_H

#include "App/AppControl.h"
#include "Ui/GenItem.h"
#include "Ux/SelectTypes.h"

#include <contacts.h>
#include <string>

namespace Contacts
{
	namespace Model
	{
		class Contact;
		class ContactField;
		class ContactBoolField;
		class ContactTextField;
		class ContactObject;
		class ContactCompoundObject;
	}

	namespace Details
	{
		/**
		 * @brief Genlist item displaying contact's image, name, phonetic name,
		 *        company and favorite status.
		 */
		class BasicInfoItem : public Ui::GenItem
		{
		public:
			/**
			 * @brief Back button pressed callback
			 */
			typedef std::function<void()> BackCallback;

			/**
			 * @brief Create basic info item.
			 * @param[in]   contact    Contact to display
			 */
			BasicInfoItem(Model::Contact &contact);

			/**
			 * @brief Set item selection mode.
			 * @param[in]   mode    Selection mode
			 */
			void setSelectMode(Ux::SelectMode mode);

			/**
			 * @brief Set "back" button callback.
			 * @param[in]   callback    "back" button callback
			 */
			void setBackCallback(BackCallback callback);

		protected:
			/**
			 * @see GenItem::getItemClass()
			 */
			virtual Elm_Genlist_Item_Class *getItemClass() const override;

			/**
			 * @see GenItem::getText()
			 */
			virtual char *getText(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenItem::getContent()
			 */
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			/**
			 * @see GenItem::getState()
			 */
			virtual Eina_Bool getState(Evas_Object *parent, const char *part) override;

		private:
			std::string formatCompanyString();
			Evas_Object *createBackButton(Evas_Object *parent);
			Evas_Object *createFavButton(Evas_Object *parent);

			void onBackPressed(Evas_Object *button, void *eventInfo);
			void onFavChanged(Evas_Object *check, void *eventInfo);
			void onImageClicked(Evas_Object *image, void *eventInfo);
			void onFieldUpdated(Model::ContactField &field, contacts_changed_e change);

			Model::Contact &m_Contact;
			Model::ContactBoolField *m_Favorite;

			Model::ContactObject &m_Image;
			Model::ContactTextField &m_ImagePath;

			Model::ContactTextField &m_Name;
			Model::ContactCompoundObject &m_PhoneticName;

			Model::ContactObject &m_Company;
			Model::ContactTextField &m_CompanyName;
			Model::ContactTextField &m_JobTitle;

			Ux::SelectMode m_SelectMode;
			BackCallback m_OnBackPressed;

			App::AppControl m_Request;
		};
	}
}

#endif /* CONTACTS_DETAILS_BASIC_INFO_ITEM_H */
