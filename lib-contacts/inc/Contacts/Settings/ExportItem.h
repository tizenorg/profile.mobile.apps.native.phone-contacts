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

#ifndef CONTACTS_SETTINGS_EXPORT_ITEM_H
#define CONTACTS_SETTINGS_EXPORT_ITEM_H

#include "App/AppControl.h"
#include "Ui/GenItem.h"

namespace Contacts
{
	namespace Settings
	{
		class ExportController;

		/**
		 * @brief Export item
		 */
		class ExportItem : public Ui::GenItem
		{
		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual void onSelected() override;
			void onPickResult(app_control_h request, app_control_h reply,
					app_control_result_e result);
			void onExportFinish(ExportController *exporter);

			App::AppControl m_AppControl;
		};
	}
}

#endif /* CONTACTS_SETTINGS_EXPORT_ITEM_H */
