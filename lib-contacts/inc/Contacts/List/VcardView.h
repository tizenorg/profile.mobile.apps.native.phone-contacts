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

#ifndef CONTACTS_LIST_VCARD_VIEW_H
#define CONTACTS_LIST_VCARD_VIEW_H

#include "Ux/SelectView.h"
#include "Contacts/List/Model/VcardProvider.h"

namespace Ui
{
	class Genlist;
}

namespace Contacts
{
	namespace Settings
	{
		class ImportController;
	}

	namespace List
	{
		class EXPORT_API VcardView : public Ux::SelectView
		{
		public:
			/**
			 * @brief Create new vcard contact list view
			 * @param[in]   vcardPath   Path of the vcard file
			 */
			explicit VcardView(std::string vcardPath);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onSelectAllInsert(Ui::GenItem *item) override;

			bool onSelectResult(Ux::SelectResults results);
			void onImportFinished(Settings::ImportController *importer);

			Ui::Genlist *m_Genlist;
			std::string m_VcardPath;
			Model::VcardProvider m_Provider;
		};
	}
}

#endif /* CONTACTS_LIST_VCARD_VIEW_H */
