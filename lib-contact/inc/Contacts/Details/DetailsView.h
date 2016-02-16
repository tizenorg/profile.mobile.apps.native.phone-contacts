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

#ifndef CONTACTS_DETAILS_DETAILS_VIEW_H
#define CONTACTS_DETAILS_DETAILS_VIEW_H

#include "App/AppControl.h"
#include "Contacts/Common/SelectMode.h"
#include "Contacts/Model/Contact.h"
#include "Ui/View.h"

namespace Ui
{
	class Genlist;
}

namespace Contacts
{
	namespace Details
	{
		class BasicInfoItem;
		class FieldItem;

		/**
		 * @brief Contact/My profile details view.
		 */
		class DetailsView : public Ui::View
		{
		public:
			/**
			 * @brief Type of object to view.
			 */
			enum Type
			{
				TypeContact = Model::ObjectTypeContact,
				TypeMyProfile = Model::ObjectTypeMyProfile
			};

			/**
			 * @brief Create details view.
			 * @param[in]   recordId    Contact or My Profile record ID
			 * @param[in]   type        Type of viewed object
			 * @param[in]   filterType  Filter to use to display contact fields
			 */
			DetailsView(int recordId, Type type = TypeContact, int filterType = FilterNone);

			/**
			 * @brief Set selection callback.
			 * @param[in]   callback    Callback to be called when selection is done
			 */
			void setSelectCallback(SelectCallback callback);

			/**
			 * @brief Set selection mode and result type.
			 * @param[in]   mode    Selection mode
			 * @param[in]   type    Selection result type
			 */
			void setSelectMode(SelectMode mode, ResultType type = ResultItem);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onPageAttached() override;
			virtual void onMenuPressed() override;

			void createPageButtons();
			void destroyPageButtons();

			void updatePageTitle();
			void updatePageMode();

			FieldItem *createFieldItem(Model::ContactObject &field);
			FieldItem *getNextFieldItem(Model::ContactFieldId fieldId);

			FieldItem *addFieldItem(Model::ContactObject &field);
			void removeFieldItem(FieldItem *item);

			void onItemChecked(bool isChecked);
			void onSingleSelected(SelectResult result);
			void onMultiSelected();

			void onArrayUpdated(Model::ContactField &field, contacts_changed_e change);
			void onObjectUpdated(FieldItem *item, Model::ContactField &field, contacts_changed_e change);

			void onDonePressed(Evas_Object *button, void *eventInfo);
			void onCancelPressed(Evas_Object *button, void *eventInfo);

			int m_RecordId;
			Model::Contact m_Contact;

			int m_FilterType;
			SelectMode m_SelectMode;
			ResultType m_ResultType;
			SelectCallback m_OnSelected;

			Evas_Object *m_DoneButton;
			Evas_Object *m_CancelButton;

			Ui::Genlist *m_Genlist;
			BasicInfoItem *m_BasicInfoItem;
			FieldItem *m_Items[Model::FieldEnd];
			size_t m_SelectCount;

			App::AppControl m_AppControl;
		};
	}
}

#endif /* CONTACTS_DETAILS_DETAILS_VIEW_H */
