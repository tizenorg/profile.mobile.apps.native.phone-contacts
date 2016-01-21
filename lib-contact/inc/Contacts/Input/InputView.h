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

#ifndef CONTACTS_INPUT_INPUT_VIEW_H
#define CONTACTS_INPUT_INPUT_VIEW_H

#include "Contacts/Model/Contact.h"
#include "Ui/View.h"

namespace Ui
{
	class Genlist;
	class GenlistItem;
}

namespace Contacts
{
	namespace Input
	{
		class ContactFieldItem;
		class AddFieldsItem;

		/**
		 * @brief Contact/My profile create/edit view.
		 */
		class InputView : public Ui::View
		{
		public:
			/**
			 * @brief Type of object to create or edit.
			 */
			enum Type
			{
				TypeContact = Model::ObjectTypeContact,
				TypeMyProfile = Model::ObjectTypeMyProfile
			};

			/**
			 * @brief Create input view.
			 * @param[in]   recordId    Contact or My Profile record ID
			 * @param[in]   type        Type of edited object
			 */
			InputView(int recordId = 0, Type type = TypeContact);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onPageAttached() override;
			virtual bool onBackPressed() override;

			ContactFieldItem *createFieldItem(Model::ContactObject &field);
			Ui::GenlistItem *getNextFieldItem(Model::ContactFieldId fieldId);

			Model::ContactObject &addField(Model::ContactFieldId fieldId);
			void removeField(Model::ContactObject &field);

			ContactFieldItem *addFieldItem(Model::ContactObject &field);
			void removeFieldItem(ContactFieldItem *item);

			void onAddField(Model::ContactFieldId fieldId);
			void onRemoveField(ContactFieldItem *item);
			void onFillChanged(bool isFilled);

			void onDonePressed(Evas_Object *button, void *eventInfo);
			void onCancelPressed(Evas_Object *button, void *eventInfo);
			bool onCancel();

			int m_RecordId;
			Model::Contact m_Contact;

			Evas_Object *m_DoneButton;
			Ui::Genlist *m_Genlist;

			ContactFieldItem *m_Items[Model::FieldEnd];
			AddFieldsItem *m_AddFieldsItem;
		};
	}
}

#endif /* CONTACTS_INPUT_INPUT_VIEW_H */
