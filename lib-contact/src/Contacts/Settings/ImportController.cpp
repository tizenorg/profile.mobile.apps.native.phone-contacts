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

#include "Contacts/Settings/ImportController.h"
#include "Contacts/Utils.h"
#include "Ui/Popup.h"
#include "Utils/Logger.h"

#include <notification.h>

using namespace Contacts::Settings;

#define BUFFER_SIZE        1024

ImportController::ImportController(Evas_Object *parent, const char *title,
			size_t totalCount, std::vector<std::string> vcards)
	: ProgressController(parent, title, totalCount), m_isCancelPopup(false),
	  m_Pause(false), m_Thread(nullptr), m_Vcards(std::move(vcards))
{
	createCancelPopup(parent);
}

ImportController::~ImportController()
{
	delete m_CancelPopup;
}

void ImportController::createCancelPopup(Evas_Object *parent)
{
	m_CancelPopup = new Ui::Popup();
	m_CancelPopup->create(parent);
	m_CancelPopup->setTitle("IDS_PB_HEADER_CANCEL_IMPORTING_ABB");

	auto onContinueFunction = std::bind(&ImportController::onPopupContinue, this);
	m_CancelPopup->addButton("IDS_PB_BUTTON_CANCEL", onContinueFunction);
	m_CancelPopup->setBackCallback(onContinueFunction);
	m_CancelPopup->addButton("IDS_PB_BUTTON_OK_ABB2", std::bind(&ImportController::onPopupCancel, this));

	evas_object_hide(m_CancelPopup->getEvasObject());
}

bool ImportController::onPopupContinue()
{
	evas_object_hide(m_CancelPopup->getEvasObject());
	m_isCancelPopup = false;
	m_Pause = false;
	m_ContinueCondition.notify_one();

	return false;
}

bool ImportController::onPopupCancel()
{
	cancel();
	m_isCancelPopup = false;
	m_Pause = false;
	m_ContinueCondition.notify_one();
	m_CancelPopup = NULL;

	return true;
}

void ImportController::onStart(Ecore_Thread *thread)
{
	RETM_IF(!thread, "thread is null");
	m_Thread = thread;
	for (auto &&vcard : m_Vcards) {
		m_CurrentVcard = vcard;

		int err = contacts_vcard_parse_to_contact_foreach(vcard.c_str(), onVcardParse, this);
		WARN_IF_ERR(err, "contacts_vcard_parse_to_contact_foreach failed: ");
	}
}

void ImportController::onNotify()
{
	if (!m_isCancelPopup) {
		m_Pause = false;
		m_ContinueCondition.notify_one();
	}
}

void ImportController::onFinish()
{
	int count = m_ImportedContacts.size();
	RETM_IF(count <= 0, "invalid count");
	int err = NOTIFICATION_ERROR_NONE;

	if (count == 1) {
		err = notification_status_message_post(_("IDS_PB_TPOP_1_CONTACT_IMPORTED"));
	} else {
		char text[BUFFER_SIZE] = { 0, };
		snprintf(text, sizeof(text), _("IDS_PB_TPOP_PD_CONTACTS_IMPORTED"), count);
		err = notification_status_message_post(text);
	}
	WARN_IF_ERR(err, "notification_status_message_post: ");
}

void ImportController::onCanceled()
{
	contacts_db_delete_records(_contacts_contact._uri, m_ImportedContacts.data(), m_ImportedContacts.size());

	int err = notification_status_message_post(_("IDS_PB_SBODY_IMPORTING_CANCELLED_M_STATUS_ABB"));
	WARN_IF_ERR(err, "notification_status_message_post failed: ");
}

bool ImportController::onCancel()
{
	char text[BUFFER_SIZE] = { 0, };
	snprintf(text, sizeof(text), _("IDS_PB_POP_CONTACTS_WILL_STOP_BEING_IMPORTED_FROM_PS"), m_CurrentVcard.c_str());
	m_CancelPopup->setText(text);

	evas_object_show(m_CancelPopup->getEvasObject());
	m_isCancelPopup = true;
	m_Pause = true;

	return false;
}

bool ImportController::onVcardParse(contacts_record_h record, void *data)
{
	RETVM_IF(!record || !data, true, "invalid data");
	ImportController *importer = (ImportController *)data;

	int id = 0;
	int err = contacts_db_insert_record(record, &id);
	RETVM_IF_ERR(err, true, "contacts_db_insert_record failed: ");

	importer->m_ImportedContacts.push_back(id);

	if (!importer->onProgress(importer->m_ImportedContacts.size())) {
		DBG("Creating vcard canceled");
		contacts_disconnect_on_thread();
		ecore_thread_cancel(importer->m_Thread);
		return false;
	}
	importer->m_Pause = true;

	std::mutex condVariableMutex;
	std::unique_lock<std::mutex> locker(condVariableMutex);
	importer->m_ContinueCondition.wait(locker, [importer]{
		return !importer->m_Pause;
	});

	// Return true to continue to scan next contact, according to contacts_vcard_parse_cb specification.
	return true;
}
