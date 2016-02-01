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

#include <fstream>
#include <notification.h>

#include "ContactsDebug.h"
#include "CtCommon.h"
#include "CtContactsToVcard.h"
#include "CtString.h"
#include "CtType.h"

#define CT_SETTING_RELATIVE_EXPORT_PATH "/Device/Contacts/Exported"


CtContactsToVcard::CtContactsToVcard(const char *title, TargetStorage vcardStorage, std::vector<int> personIdList)
: CtProgressController(title, personIdList.size())
, m_PersonIdList (std::move(personIdList))
, m_VcardStorage(vcardStorage)
{
	WHIT();
	createDirectory();

	vcardFilePathGet();
	if (m_VcardPath.empty()) {
		WWARNING("vcard path is NULL");
		return;
	}
}

std::string CtContactsToVcard::getVcardPath()
{
	return m_VcardPath;
}

int CtContactsToVcard::getTotalCount()
{
	return m_PersonIdList.size();
}

void CtContactsToVcard::createDirectory()
{
	std::string path;
	if (TargetStorage::DEVICE == m_VcardStorage) {
		path = getRootDirectoryPath(STORAGE_TYPE_INTERNAL);
	} else if (TargetStorage::SD_CARD == m_VcardStorage) {
		path = getRootDirectoryPath(STORAGE_TYPE_EXTERNAL);
	}

	if (!path.empty()) {
		std::string command("mkdir -p ");
		command.append(path).append(CT_SETTING_RELATIVE_EXPORT_PATH);
		WINFO("Command: %s", command.c_str());

		int ret = system(command.c_str());
		WPRET_M(ret == -1, "failed to create folder");
	} else {
		WERROR("Export directory path is null");
	}
}

void CtContactsToVcard::vcardFilePathGet()
{
	const int vcardMaxNumber = 999;
	char path[CT_TEXT_MAX_LEN] = {0, };
	int fileNumber = 1;
	std::string directoryPath;

	if (m_VcardStorage == TargetStorage::DEVICE) {
		directoryPath = getRootDirectoryPath(STORAGE_TYPE_INTERNAL);
		if(!directoryPath.empty()) {
			directoryPath.append(CT_SETTING_RELATIVE_EXPORT_PATH);
		}
	} else if (m_VcardStorage == TargetStorage::SD_CARD) {
		directoryPath = getRootDirectoryPath(STORAGE_TYPE_EXTERNAL);
		if(!directoryPath.empty()) {
			directoryPath.append(CT_SETTING_RELATIVE_EXPORT_PATH);
		}
	} else if (m_VcardStorage == TargetStorage::INTERNAL_OTHER){
		directoryPath = getDirectoryPath(STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_OTHERS);
	}

	WPRET_M(directoryPath.empty(), "Cannot get directory path to storage");

	while (fileNumber <= vcardMaxNumber) {
		struct stat buf;
		if (fileNumber == 1) {
			snprintf(path, sizeof(path), "%s/Contacts.vcf", directoryPath.c_str());
		} else {
			snprintf(path, sizeof(path), "%s/Contacts_%03d.vcf", directoryPath.c_str(), fileNumber);
		}

		if (stat(path, &buf) == 0) {
			++fileNumber;
		} else {
			m_VcardPath = path;
			WDEBUG("path = %s", path);
			return;
		}
	}

	if (fileNumber > vcardMaxNumber) {
		WERROR("Too many vcards. Delete some before creating new.");
		notification_status_message_post(V_("IDS_PB_POP_FAILED"));
	}
	m_VcardPath.clear();
}

void CtContactsToVcard::onStart(Ecore_Thread *thread)
{
	WHIT();
	FILE *file = fopen(m_VcardPath.c_str(), "a");
	WPRET_M(!file, "fopen failed: %s", strerror(errno));
	int currentCount = 0;

	for (auto &&personId : m_PersonIdList) {

		contacts_record_h personRecord = NULL;
		int err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_db_get_record failed: %d", err);
			continue;
		}

		if (personRecord) {
			char *vcardStream = NULL;

			err = contacts_vcard_make_from_person(personRecord, &vcardStream);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_vcard_make_from_person failed: %d", err);
				contacts_record_destroy(personRecord, true);
				continue;
			}

			if (vcardStream) {
				fputs(vcardStream, file);
				free(vcardStream);

				++currentCount;
				if (!onProgress(currentCount)) {
					WDEBUG("Creating vcard canceled");
					ecore_thread_cancel(thread);
					contacts_record_destroy(personRecord, true);
					fclose(file);
					return;
				}
			}
			contacts_record_destroy(personRecord, true);
		}
	}
	fclose(file);
}

void CtContactsToVcard::onCanceled()
{
	WHIT();
	if (!m_VcardPath.empty()) {
		remove(m_VcardPath.c_str());
	}

	notification_status_message_post(V_("IDS_PB_SBODY_EXPORTING_CANCELLED_M_STATUS_ABB"));
}
