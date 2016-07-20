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

#include "OperationViewController.h"

#include "Common/Database/Queries.h"
#include "Contacts/Details/DetailsView.h"
#include "Contacts/List/VcardView.h"

#include "App/AppControlRequest.h"
#include "Ui/Navigator.h"
#include "Utils/Logger.h"

#include <string.h>
#include <sys/stat.h>

#define APP_CONTROL_URI_PATH "file://"

using namespace Common::Database;
using namespace Contacts;
using namespace Contacts::Details;
using namespace Contacts::List;

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
		std::string path = getUrn(APP_CONTROL_URI_PATH);
		struct stat buffer;
		if (stat(path.c_str(), &buffer) == 0) {
			view = new VcardView(path);
		} else {
			ERR("Failed to open file: \"%s\". %s", path.c_str(), strerror(errno));
		}
	}

	free(mime);

	if (view) {
		getNavigator()->navigateTo(view);
	} else {
		ui_app_exit();
	}
}
