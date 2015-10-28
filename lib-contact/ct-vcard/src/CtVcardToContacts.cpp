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

#include <notification.h>

#include "ContactsDebug.h"
#include "CtString.h"
#include "CtType.h"
#include "CtVcardToContacts.h"

#include "WPopup.h"

CtVcardToContacts::CtVcardToContacts(const char *title, int totalCount, std::vector<std::string> vcards)
: CtProgressController(title, totalCount)
, m_CancelPopup(NULL)
, m_Pause(false)
, m_Thread(NULL)
, m_Vcards(std::move(vcards))
{
	WHIT();
	createCancelPopup();
}

CtVcardToContacts::~CtVcardToContacts()
{
	WHIT();
	if (m_CancelPopup) {
		m_CancelPopup->destroy();
	}
}

int CtVcardToContacts::getImportedContactsCount()
{
	return m_ImportedContacts.size();
}

WPopup* CtVcardToContacts::getCancelPopup()
{
	return m_CancelPopup;
}

void CtVcardToContacts::createCancelPopup()
{
	m_CancelPopup = new WPopup();
	m_CancelPopup->setTextTranslatable(PACKAGE);
	m_CancelPopup->setTitle("IDS_PB_HEADER_CANCEL_IMPORTING_ABB");

	auto continueFunction = std::bind(&CtVcardToContacts::continueImporting, this, std::placeholders::_1);
	m_CancelPopup->addButton("IDS_PB_BUTTON_CANCEL", continueFunction);
	m_CancelPopup->setOnBack(continueFunction);

	m_CancelPopup->addButton("IDS_PB_BUTTON_OK_ABB2", std::bind(&CtVcardToContacts::cancelImporting, this, std::placeholders::_1));
}

void CtVcardToContacts::continueImporting(bool *destroyPopup)
{
	WHIT();
	evas_object_hide(m_CancelPopup->getEvasObj());
	m_Pause = false;
	m_ContinueCondition.notify_one();
	*destroyPopup = false;
}

void CtVcardToContacts::cancelImporting(bool *destroyPopup)
{
	WHIT();
	cancel();
	m_Pause = false;
	m_ContinueCondition.notify_one();
	m_CancelPopup = NULL;
}

void CtVcardToContacts::onStart(Ecore_Thread *thread)
{
	WHIT();
	m_Thread = thread;
	for (auto &&vcard : m_Vcards) {
		m_CurrentImportingVcard = vcard;
		int err = contacts_vcard_parse_to_contact_foreach(vcard.c_str(), vcardParseCb, this);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_vcard_parse_to_contact_foreach failed: %d", err);
		}
	}
}

void CtVcardToContacts::onCanceled()
{
	WHIT();
	contacts_db_delete_records(_contacts_contact._uri, m_ImportedContacts.data(), m_ImportedContacts.size());

	int err = notification_status_message_post(V_("IDS_PB_SBODY_IMPORTING_CANCELLED_M_STATUS_ABB"));
	if (err != NOTIFICATION_ERROR_NONE) {
		WERROR("notification_status_message_post failed: %d", err);
	}
}

bool CtVcardToContacts::onCancel()
{
	WHIT();
	char content[CT_TEXT_MAX_LEN] = {0};
	snprintf(content, CT_TEXT_MAX_LEN, V_("IDS_PB_POP_CONTACTS_WILL_STOP_BEING_IMPORTED_FROM_PS"), m_CurrentImportingVcard.c_str());
	elm_object_text_set(m_CancelPopup->getEvasObj(), content);

	evas_object_show(m_CancelPopup->getEvasObj());
	m_Pause = true;

	return false;
}

bool CtVcardToContacts::vcardParseCb(contacts_record_h record, void *user_data)
{
	CtVcardToContacts *importer = (CtVcardToContacts *)user_data;

	if (record) {
		int id = 0;

		int err = contacts_db_insert_record(record, &id);
		if ((CONTACTS_ERROR_NONE != err) || (id <= 0)) {
			WERROR("contacts_db_insert_record failed: err = %d, id = %d", err, id);
		}

		importer->m_ImportedContacts.push_back(id);
		if (!importer->onProgress(importer->m_ImportedContacts.size())) {
			WDEBUG("Creating vcard canceled");
			ecore_thread_cancel(importer->m_Thread);
			return false;
		}

		std::mutex condVariableMutex;
		std::unique_lock<std::mutex> locker(condVariableMutex);
		importer->m_ContinueCondition.wait(locker, [importer]{
			return !importer->m_Pause;
		});
	}

	// Return true to continue to scan next contact, according to contacts_vcard_parse_cb specification.
	return true;
}
