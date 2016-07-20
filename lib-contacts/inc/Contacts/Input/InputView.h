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

#ifndef CONTACTS_INPUT_INPUT_VIEW_H
#define CONTACTS_INPUT_INPUT_VIEW_H

#include "Contacts/Model/Contact.h"
#include "Ui/View.h"

namespace Ui
{
	class Genlist;
	class GenItem;
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
		class EXPORT_API InputView : public Ui::View
		{
		public:
			/**
			 * @brief Called when contact editing is finished.
			 * @param[in]   Edited contact ID
			 */
			typedef std::function<void(int)> ResultCallback;

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

			/**
			 * @brief Add field with text value.
			 * @remark If the field can't have multiple instances,
			 *         then existing single field's value is reassigned instead.
			 * @param[in]   fieldId     Contact field ID
			 * @param[in]   value       Field value
			 */
			void addField(Model::ContactFieldId fieldId, const char *value);

			/**
			 * @brief Set editing result callback.
			 * @param[in]   callback    Callback to be called when editing is finished.
			 */
			void setResultCallback(ResultCallback callback);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;
			virtual bool onBackPressed() override;

			ContactFieldItem *createFieldItem(Model::ContactObject &field);
			Ui::GenItem *getNextFieldItem(Model::ContactFieldId fieldId);

			Model::ContactObject &addField(Model::ContactFieldId fieldId);
			void removeField(Model::ContactObject &field);

			ContactFieldItem *addFieldItem(Model::ContactObject &field);
			void removeFieldItem(ContactFieldItem *item);

			void addEventField();
			void addRingtoneField();

			void onAddField(Model::ContactFieldId fieldId);
			void onRemoveField(ContactFieldItem *item);
			void onContactFilled(bool isFilled);

			void onDonePressed(Evas_Object *button, void *eventInfo);
			void onCancelPressed(Evas_Object *button, void *eventInfo);

			bool onCancel();
			void onSave();

			Model::Contact m_Contact;
			ResultCallback m_OnResult;

			Evas_Object *m_DoneButton;
			Ui::Genlist *m_Genlist;

			ContactFieldItem *m_Items[Model::FieldEnd];
			AddFieldsItem *m_AddFieldsItem;
		};
	}
}

#endif /* CONTACTS_INPUT_INPUT_VIEW_H */
