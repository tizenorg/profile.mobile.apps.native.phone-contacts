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

#include "Contacts/Settings/ImportItem.h"
#include "Contacts/Settings/ImportController.h"
#include "Contacts/Common/Utils.h"

#include "App/AppControlRequest.h"
#include "Ui/Genlist.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include <app_i18n.h>
#include <notification.h>

using namespace Contacts::Common;
using namespace Contacts::Settings;

#define BUFFER_SIZE 1024

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

	char *pathEnd = nullptr;
	char *path = strtok_r(extraResult, "?", &pathEnd);

	std::vector<std::string> paths;
	int totalCount = 0;
	while (path) {
		int count = 0;
		err = contacts_vcard_get_entity_count(path, &count);
		WARN_IF_ERR(err, "contacts_vcard_get_entity_count() failed.");
		totalCount += count;

		paths.push_back(path);
		path = strtok_r(nullptr, "?", &pathEnd);
	}
	free(extraResult);

	ImportController *importer = new ImportController(getParent()->getEvasObject(),
			"IDS_PB_HEADER_IMPORT_CONTACTS_ABB2", totalCount, std::move(paths));
	importer->setFinishCallback(std::bind(&ImportItem::onImportFinish, this, importer));
	importer->run();
}

void ImportItem::onImportFinish(ImportController *importer)
{
	int count = importer->getTotalCount();
	RETM_IF(count <= 0, "invalid count");
	int err = NOTIFICATION_ERROR_NONE;

	if (count == 1) {
		err = notification_status_message_post(_("IDS_PB_TPOP_1_CONTACT_IMPORTED"));
	} else {
		char text[BUFFER_SIZE] = { 0, };
		snprintf(text, sizeof(text), _("IDS_PB_TPOP_PD_CONTACTS_IMPORTED"), count);
		err = notification_status_message_post(text);
	}
	WARN_IF_ERR(err, "notification_status_message_post() failed.");
}
