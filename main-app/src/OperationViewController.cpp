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

#include "OperationViewController.h"

#include "MainApp.h"
#include "Contacts/Details/DetailsView.h"
#include "Contacts/List/ListView.h"
#include "Contacts/Settings/ImportController.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"

#include "App/AppControlRequest.h"
#include "Ui/Navigator.h"

#include <notification.h>
#include <string.h>

using namespace Contacts;
using namespace Contacts::Details;
using namespace Contacts::List;

#define BUFFER_SIZE        1024

OperationViewController::OperationViewController()
	: OperationController(OperationView)
{
}

void OperationViewController::onRequest(Operation operation, app_control_h request)
{
	Ui::View *view = nullptr;
	char *mime = nullptr;
	app_control_get_mime(request, &mime);

	if (mime && strcmp(mime, APP_CONTROL_MIME_CONTACT) == 0) {
		int personId = App::getIntExtraData(request, APP_CONTROL_DATA_ID);
		if (personId > 0) {
			view = new DetailsView(getDisplayContactId(personId));
		}
	} else {
		char *uri = NULL;
		app_control_get_uri(request, &uri);
		if (uri) {
			view = new ListView(uri);
			ListView *listView = (ListView *)view;
			listView->setSelectMode(SelectMulti);
			listView->setSelectCallback(std::bind(&OperationViewController::onSelectResult, this,
					std::placeholders::_1, listView, std::string(uri)));

			free(uri);
		}
	}

	free(mime);

	if (view) {
		getNavigator()->navigateTo(view);
	} else {
		ui_app_exit();
	}
}

bool OperationViewController::onSelectResult(SelectResults results, ListView *view, std::string uri)
{
	std::vector<contacts_record_h> records;
	for (auto &&result : results) {
		records.push_back((contacts_record_h)result.value.data);
	}
	int count = records.size();
	if (count) {
		std::vector<std::string> vcards;
		vcards.push_back(uri);
		Settings::ImportController *importer = new Settings::ImportController(view->getEvasObject(),
				"IDS_PB_HEADER_IMPORT_CONTACTS_ABB2", count, std::move(vcards), std::move(records));
		importer->setFinishCallback(std::bind(&OperationViewController::onImportFinish, this, importer, view));
		importer->run();
	}

	return false;
}

void OperationViewController::onImportFinish(Settings::ImportController *importer, ListView *view)
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

	view->getPage()->close();
}
