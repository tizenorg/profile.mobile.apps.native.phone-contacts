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

#include "Phone/Dialer/SpeedDialPopup.h"
#include "App/AppControlRequest.h"
#include "Phone/Utils.h"
#include "Utils/Logger.h"

#include <contacts.h>
#include <notification.h>

using namespace Phone::Dialer;

SpeedDialPopup::SpeedDialPopup(int speedNumber)
	: m_SpeedNumber(speedNumber)
{
}

void SpeedDialPopup::onCreated()
{
	setTitle("IDS_KPD_HEADER_ASSIGN_AS_SPEED_DIAL_NUMBER_ABB");
	setText("IDS_KPD_POP_THERE_IS_NO_CONTACT_ASSIGNED_TO_THIS_SPEED_DIAL_NUMBER_TAP_OK_TO_ASSIGN_ONE_NOW");

	using namespace std::placeholders;
	addButton("IDS_LOGS_BUTTON_CANCEL_ABB3");
	addButton("IDS_PB_BUTTON_OK_ABB2", std::bind(&SpeedDialPopup::onOkPressed, this));
}

bool SpeedDialPopup::onOkPressed()
{
	App::AppControl request = App::requestContactPick(APP_CONTROL_SELECT_SINGLE,
			APP_CONTROL_RESULT_PHONE);

	int err = request.launch(&SpeedDialPopup::onPickResult, this);
	RETVM_IF(err != APP_CONTROL_ERROR_NONE, true, "launchContactPick() failed(0x%x)", err);

	request.detach();
	return false;
}

void SpeedDialPopup::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result, void *data)
{
	SpeedDialPopup *popup = (SpeedDialPopup*) data;

	char **numberIds = nullptr;
	int count = 0;

	int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &numberIds, &count);
	RETM_IF(err != APP_CONTROL_ERROR_NONE, "app_control_get_extra_data() failed(0x%x)", err);

	if (numberIds && numberIds[0]) {
		int numberId = atoi(numberIds[0]);
		if (numberId > 0) {
			if (Phone::addSpeedDialNumber(popup->m_SpeedNumber, numberId)) {
				notification_status_message_post(_("IDS_KPD_TPOP_SPEED_DIAL_NUMBER_ASSIGNED"));
			} else {
				notification_status_message_post(_("IDS_PB_POP_ALREADY_EXISTS_LC"));
			}
		}
	}

	for (int i = 0; i < count; ++i) {
		free(numberIds[i]);
	}
	free(numberIds);

	delete popup;
}
