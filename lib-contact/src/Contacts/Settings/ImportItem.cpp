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
#include "Contacts/Common/Utils.h"
#include "Contacts/Settings/ImportController.h"
#include "Contacts/Settings/ImportItem.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include <app_i18n.h>

using namespace Contacts::Common;
using namespace Contacts::Settings;

char *ImportItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.text") == 0) {
		return strdup(_("IDS_PB_MBODY_IMPORT"));
	}

	return nullptr;
}

void ImportItem::onSelected()
{
	m_AppControl = App::requestPickVcard(getDirectoryPath(STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_MAX).c_str());
	m_AppControl.launch(makeCallbackWithLastParam(&ImportItem::onPickResult), this);
}

void ImportItem::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result)
{
	char *extraResult = nullptr;
	int err = app_control_get_extra_data(reply, "result", &extraResult);
	RETM_IF_ERR(err, "app_control_get_extra_data() failed.");

	char *last = nullptr;
	char *path = strtok_r(extraResult, "?", &last);

	std::vector<std::string> vcards;
	int totalCount = 0;
	while (path) {
		vcards.push_back(path);

		int count = 0;
		err = contacts_vcard_get_entity_count(path, &count);
		WARN_IF_ERR(err, "contacts_vcard_get_entity_count() failed.");
		totalCount += count;

		path = strtok_r(nullptr, "?", &last);
	}
	free(extraResult);

	ImportController *controller = new ImportController(getParent()->getEvasObject(),
			"IDS_PB_HEADER_IMPORT_CONTACTS_ABB2", totalCount, std::move(vcards));
	controller->run();
}
