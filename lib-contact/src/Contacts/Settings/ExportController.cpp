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

#include "Contacts/Common/Utils.h"
#include "Contacts/Settings/ExportController.h"
#include "Utils/Logger.h"

#include <contacts.h>
#include <notification.h>

using namespace Contacts::Common;
using namespace Contacts::Settings;

#define BUFFER_SIZE        64

#define RELATIVE_EXPORT_PATH   "/Device/Contacts/Exported"

ExportController::ExportController(Evas_Object *parent, const char *title,
		std::vector<int> personIdList, TargetStorage vcardStorage)
	: ProgressController(parent, title, personIdList.size()),
	  m_PersonIdList(personIdList), m_VcardStorage(vcardStorage)
{
	createDirectory();

	getVcardFilePath();
	RETM_IF_ERR(m_VcardPath.empty(), "vcard path is NULL");
}

void ExportController::createDirectory()
{
	std::string path;
	if (TargetStorage::TargetStorageDevice == m_VcardStorage) {
		path = getRootDirectoryPath(STORAGE_TYPE_INTERNAL);
	} else if (TargetStorage::TargetStorageSdCard == m_VcardStorage) {
		path = getRootDirectoryPath(STORAGE_TYPE_EXTERNAL);
	}

	if (!path.empty()) {
		std::string command("mkdir -p ");
		command.append(path).append(RELATIVE_EXPORT_PATH);

		int ret = system(command.c_str());
		RETM_IF(ret == -1, "failed to create folder");
	} else {
		ERR("Export directory path is null");
	}
}

void ExportController::getVcardFilePath()
{
	std::string directoryPath;

	if (m_VcardStorage == TargetStorage::TargetStorageDevice) {
		directoryPath = getRootDirectoryPath(STORAGE_TYPE_INTERNAL);
		if(!directoryPath.empty()) {
			directoryPath.append(RELATIVE_EXPORT_PATH);
		}
	} else if (m_VcardStorage == TargetStorage::TargetStorageSdCard) {
		directoryPath = getRootDirectoryPath(STORAGE_TYPE_EXTERNAL);
		if(!directoryPath.empty()) {
			directoryPath.append(RELATIVE_EXPORT_PATH);
		}
	} else if (m_VcardStorage == TargetStorage::TargetStorageInternalOther){
		directoryPath = getDirectoryPath(STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_OTHERS);
	}

	RETM_IF(directoryPath.empty(), "Cannot get directory path to storage");

	const int vcardMaxNumber = 999;
	char path[BUFFER_SIZE] = { 0, };
	int fileNumber = 1;
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
			return;
		}
	}

	if (fileNumber > vcardMaxNumber) {
		ERR("Too many vcards. Delete some before creating new.");
		notification_status_message_post("Failed");
	}
	m_VcardPath.clear();
}

void ExportController::onStart(Ecore_Thread *thread)
{
	FILE *file = fopen(m_VcardPath.c_str(), "a");
	RETM_IF(!file, "fopen failed: %s", strerror(errno));
	size_t currentCount = 0;

	for (auto &&personId : m_PersonIdList) {

		contacts_record_h personRecord = NULL;
		int err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		if (CONTACTS_ERROR_NONE != err) {
			ERR("contacts_db_get_record failed: %d", err);
			continue;
		}

		if (personRecord) {
			char *vcardStream = NULL;

			err = contacts_vcard_make_from_person(personRecord, &vcardStream);
			if (CONTACTS_ERROR_NONE != err) {
				ERR("contacts_vcard_make_from_person failed: %d", err);
				contacts_record_destroy(personRecord, true);
				continue;
			}

			if (vcardStream) {
				fputs(vcardStream, file);
				free(vcardStream);

				++currentCount;

				if (!onProgress(currentCount)) {
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

void ExportController::onCanceled()
{
	if (!m_VcardPath.empty()) {
		remove(m_VcardPath.c_str());
	}

	notification_status_message_post(_("IDS_PB_SBODY_EXPORTING_CANCELLED_M_STATUS_ABB"));
}
