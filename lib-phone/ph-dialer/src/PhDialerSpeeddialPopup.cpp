/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "PhDialerSpeeddialPopup.h"
#include "PhCommon.h"

#include "ContactsDebug.h"
#include "ContactsAppControl.h"
#include "ContactsLocalization.h"

#include <contacts.h>
#include <Elementary.h>
#include <notification.h>

PhDialerSpeeddialPopup::PhDialerSpeeddialPopup(int speedNumber)
	: m_SpeedNumber(speedNumber)
{
}

Evas_Object *PhDialerSpeeddialPopup::onCreate(Evas_Object* parent, void* param)
{
	setTextTranslatable(TEXT_DOMAIN);
	setTitle("IDS_KPD_HEADER_ASSIGN_AS_SPEED_DIAL_NUMBER_ABB");
	setContent("IDS_KPD_POP_THERE_IS_NO_CONTACT_ASSIGNED_TO_THIS_SPEED_DIAL_NUMBER_TAP_OK_TO_ASSIGN_ONE_NOW");

	using namespace std::placeholders;
	addButton("IDS_LOGS_BUTTON_CANCEL_ABB3", nullptr);
	addButton("IDS_PB_BUTTON_OK_ABB2", std::bind(&PhDialerSpeeddialPopup::onOkPressed, this, _1));
	return WPopup::onCreate(parent, param);
}

void PhDialerSpeeddialPopup::onOkPressed(bool *destroyPopup)
{
	int err = launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_SINGLE,
			APP_CONTROL_DATA_TYPE_PHONE,
			&PhDialerSpeeddialPopup::onPickResult, this);
	WPRET_M(err != APP_CONTROL_ERROR_NONE, "launchContactPick() failed(0x%x)", err);
	*destroyPopup = false;
}

void PhDialerSpeeddialPopup::onPickResult(app_control_h request, app_control_h reply,
		app_control_result_e result, void *data)
{
	PhDialerSpeeddialPopup *popup = (PhDialerSpeeddialPopup*) data;

	char **numberIds = 0;
	int count = 0;
	int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &numberIds, &count);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "app_control_get_extra_data() failed(0x%x)", err);
	if (numberIds && numberIds[0]) {
		int numberId = atoi(numberIds[0]);
		if (numberId > 0) {
			if (PhCommon::addSpeedDialNumber(popup->m_SpeedNumber, numberId)) {
				notification_status_message_post(T_("IDS_KPD_TPOP_SPEED_DIAL_NUMBER_ASSIGNED"));
			} else {
				notification_status_message_post(T_("IDS_PB_POP_ALREADY_EXISTS_LC"));
			}
		}

		for (int i = 0; i < count; ++i) {
			free(numberIds[i]);
		}
	}

	free(numberIds);

	popup->destroy();
}
