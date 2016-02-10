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

namespace {
	const storage_type_e storageType[] = {
		/* [TargetStorageDevice] = */           STORAGE_TYPE_INTERNAL,
		/* [TargetStorageSdCard] = */           STORAGE_TYPE_EXTERNAL,
		/* [TargetStorageInternalOther] = */    STORAGE_TYPE_INTERNAL
	};
}

ExportController::ExportController(Evas_Object *parent, const char *title,
		std::vector<int> personIdList, TargetStorage vcardStorage)
	: ProgressController(parent, title, personIdList.size()),
	  m_PersonIdList(std::move(personIdList)), m_VcardStorage(vcardStorage)
{
	createDirectory();
	m_VcardPath = getVcardFilePath();
}

void ExportController::createDirectory()
{
	RETM_IF(((m_VcardStorage != TargetStorageDevice) && (m_VcardStorage != TargetStorageSdCard)),
			"Wrong vcard storage");

	std::string path = getDirectoryPath(storageType[m_VcardStorage], STORAGE_DIRECTORY_MAX);
	RETM_IF(path.empty(), "Export directory path is null");

	std::string command("mkdir -p ");
	command.append(path).append(RELATIVE_EXPORT_PATH);

	if (system(command.c_str()) == -1) {
		ERR("failed to create folder");
	}
}

std::string ExportController::getVcardDirectoryPath()
{
	std::string directoryPath = getDirectoryPath(storageType[m_VcardStorage], STORAGE_DIRECTORY_MAX);

	if ((m_VcardStorage == TargetStorageDevice || m_VcardStorage == TargetStorageSdCard)
			&& !directoryPath.empty()) {
		directoryPath.append(RELATIVE_EXPORT_PATH);
	}

	return directoryPath;
}

std::string ExportController::getVcardFilePath()
{
	std::string directoryPath = getVcardDirectoryPath();
	RETVM_IF(directoryPath.empty(), std::string(), "Cannot get directory path to storage");

	const int vcardMaxNumber = 999;

	for (int fileNumber = 1; fileNumber <= vcardMaxNumber; ++fileNumber) {
		char path[BUFFER_SIZE] = { 0, };

		if (fileNumber == 1) {
			snprintf(path, sizeof(path), "%s/Contacts.vcf", directoryPath.c_str());
		} else {
			snprintf(path, sizeof(path), "%s/Contacts_%03d.vcf", directoryPath.c_str(), fileNumber);
		}

		struct stat buf;
		if (stat(path, &buf) != 0) {
			std::string vcardPath = path;
			return vcardPath;
		}
	}

	ERR("Too many vcards. Delete some before creating new.");
	notification_status_message_post(_("IDS_LOGS_SBODY_FAILED_M_STATUS"));
	return std::string();
}

void ExportController::onStart(Ecore_Thread *thread)
{
	FILE *file = fopen(m_VcardPath.c_str(), "a");
	RETM_IF(!file, "fopen failed: %s", strerror(errno));
	size_t currentCount = 0;

	for (auto &&personId : m_PersonIdList) {

		contacts_record_h personRecord = nullptr;
		int err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		LOG_IF_ERR(err, continue, "contacts_db_get_record failed: ");

		char *vcardStream = nullptr;

		err = contacts_vcard_make_from_person(personRecord, &vcardStream);
		contacts_record_destroy(personRecord, true);
		LOG_IF_ERR(err, continue, "contacts_vcard_make_from_person failed: ");

		fputs(vcardStream, file);
		free(vcardStream);

		++currentCount;

		if (!onProgress(currentCount)) {
			ecore_thread_cancel(thread);
			break;
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
