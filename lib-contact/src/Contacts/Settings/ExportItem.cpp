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

#include "App/AppControlRequest.h"
#include "Contacts/Settings/ExportItem.h"
#include "Contacts/Settings/ExportController.h"
#include "Ui/Genlist.h"
#include "Ui/Popup.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

using namespace Contacts::Settings;

#define BUFFER_SIZE        1024

char *ExportItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_MBODY_EXPORT"));
	}

	return nullptr;
}

void ExportItem::onSelected()
{
	m_AppControl = App::requestContactPick(APP_CONTROL_SELECT_MULTIPLE, APP_CONTROL_RESULT_ID);
	m_AppControl.launch(makeCallbackWithLastParam(&ExportItem::onPickResult), this);
}

void ExportItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	std::vector<int> personIdList = App::getIntExtraDataArray(reply, APP_CONTROL_DATA_SELECTED);
	RETM_IF(personIdList.empty(), "Person id list is empty.");

	ExportController *exporter = new ExportController(
			getParent()->getEvasObject(), "IDS_PB_HEADER_EXPORT_CONTACTS_ABB",
			std::move(personIdList), StorageDevice);
	exporter->run();
}
