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

#include "Phone/Dialer/SpeedDialPopup.h"
#include "Phone/Utils.h"

#include "App/AppControlRequest.h"
#include "Utils/Logger.h"

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

	addButton("IDS_KPD_BUTTON_CANCEL_ABB");
	addButton("IDS_KPD_BUTTON_OK_ABB2", std::bind(&SpeedDialPopup::onOkPressed, this));
}

bool SpeedDialPopup::onOkPressed()
{
	App::AppControl request = App::requestContactPick(APP_CONTROL_SELECT_SINGLE,
			APP_CONTROL_RESULT_PHONE);

	int err = request.launch(&SpeedDialPopup::onPickResult, this);
	request.detach();

	return err != APP_CONTROL_ERROR_NONE;
}

void SpeedDialPopup::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result, void *data)
{
	SpeedDialPopup *popup = (SpeedDialPopup *) data;

	int numberId = atoi(App::getSingleExtraData(reply, APP_CONTROL_DATA_SELECTED).c_str());
	if (numberId > 0) {
		bool isAssigned = Phone::addSpeedDialNumber(popup->m_SpeedNumber, numberId);
		notification_status_message_post(_(isAssigned
				? "IDS_KPD_TPOP_SPEED_DIAL_NUMBER_ASSIGNED"
				: "IDS_PB_POP_ALREADY_EXISTS_LC"));
	}

	popup->close();
}
