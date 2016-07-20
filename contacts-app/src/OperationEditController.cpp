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

#include "OperationEditController.h"

#include "Common/Database/Queries.h"
#include "Contacts/Input/InputView.h"
#include "Contacts/List/ListView.h"

#include "App/AppControlRequest.h"
#include "Ui/Navigator.h"

#include <string>

using namespace Common::Database;
using namespace Contacts;
using namespace Contacts::Input;
using namespace Contacts::List;
using namespace Contacts::Model;
using namespace std::placeholders;

OperationEditController::OperationEditController()
	: OperationController(OperationAdd | OperationEdit)
{
}

void OperationEditController::onRequest(Operation operation, app_control_h request)
{
	Ui::View *view = nullptr;
	int personId = App::getIntExtraData(request, APP_CONTROL_DATA_ID);

	if (personId == 0 && operation == OperationEdit) {
		auto *listView = new ListView();
		listView->setSelectMode(Ux::SelectSingle);
		listView->setSelectCallback(std::bind(&OperationEditController::onSelectResult, this, _1));
		view = listView;
	} else {
		view = createInputView(personId);
	}

	getNavigator()->navigateTo(view);
}

Ui::View *OperationEditController::createInputView(int personId)
{
	char *name = nullptr;
	char *number = nullptr;
	char *email = nullptr;
	char *url = nullptr;

	app_control_h request = getRequest();
	app_control_get_extra_data(request, APP_CONTROL_DATA_NAME, &name);
	app_control_get_extra_data(request, APP_CONTROL_DATA_PHONE, &number);
	app_control_get_extra_data(request, APP_CONTROL_DATA_EMAIL, &email);
	app_control_get_extra_data(request, APP_CONTROL_DATA_URL, &url);

	auto view = new InputView(getDisplayContactId(personId));
	view->addField(FieldName, name);
	view->addField(FieldNumber, number);
	view->addField(FieldEmail, email);
	view->addField(FieldUrl, url);
	view->setResultCallback(std::bind(&OperationEditController::onInputResult, this, _1));

	free(name);
	free(number);
	free(email);
	free(url);

	return view;
}

bool OperationEditController::onSelectResult(Ux::SelectResults results)
{
	Ui::View *view = createInputView(results.front().value.id);
	getNavigator()->navigateTo(view);
	return true;
}

void OperationEditController::onInputResult(int contactId)
{
	int personId = getPersonId(contactId);

	app_control_h reply;
	app_control_create(&reply);
	app_control_add_extra_data(reply, APP_CONTROL_DATA_ID, std::to_string(personId).c_str());
	app_control_reply_to_launch_request(reply, getRequest(), APP_CONTROL_RESULT_SUCCEEDED);
	app_control_destroy(reply);
}
