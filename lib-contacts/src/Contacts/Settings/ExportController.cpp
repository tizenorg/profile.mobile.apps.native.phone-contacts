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
#include "Ui/Popup.h"
#include "Utils/Logger.h"

#include <contacts.h>
#include <notification.h>

using namespace Contacts::Common;
using namespace Contacts::Settings;

#define BUFFER_SIZE        1024

#define RELATIVE_EXPORT_PATH   "/Device/Contacts/Exported"

namespace
{
	struct {
		storage_type_e type;
		storage_directory_e directory;
	} storageParam[] = {
		/* [StorageDevice]        = */    { STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_MAX },
		/* [StorageSdCard]        = */    { STORAGE_TYPE_EXTERNAL, STORAGE_DIRECTORY_MAX },
		/* [StorageInternalOther] = */    { STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_OTHERS }
	};
}

ExportController::ExportController(Evas_Object *parent, const char *title,
		std::vector<int> personIdList, StorageType vcardStorage)
	: ProgressController(parent, title, personIdList.size()), m_Parent(parent),
	  m_PersonIdList(std::move(personIdList)), m_VcardStorage(vcardStorage)
{
	m_VcardPath = getVcardFilePath();
}

const char *ExportController::getVcardRelativePath() const
{
	std::string deviceDir = getDirectoryPath(storageParam[m_VcardStorage].type,
			STORAGE_DIRECTORY_MAX);

	size_t endPosition = m_VcardPath.find(deviceDir);
	RETVM_IF(endPosition == std::string::npos, nullptr, "No matches were found.");
	endPosition += deviceDir.length() + 1;

	return &m_VcardPath[endPosition];
}

const std::string &ExportController::getVcardPath() const
{
	return m_VcardPath;
}

size_t ExportController::getTotalCount() const
{
	return m_PersonIdList.size();
}

void ExportController::createDirectory(const std::string &directoryPath)
{
	std::string command("mkdir -p ");
	command.append(directoryPath);

	WARN_IF(system(command.c_str()) == -1, "failed to create folder: %s", strerror(errno));
}

std::string ExportController::getVcardDirectoryPath()
{
	std::string directoryPath = getDirectoryPath(storageParam[m_VcardStorage].type, storageParam[m_VcardStorage].directory);

	if ((storageParam[m_VcardStorage].directory == STORAGE_DIRECTORY_MAX)
			&& !directoryPath.empty()) {
		directoryPath.append(RELATIVE_EXPORT_PATH);
		createDirectory(directoryPath);
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
			return path;
		}
	}

	ERR("Too many vcards. Delete some before creating new.");
	notification_status_message_post(_("IDS_PB_POP_FAILED"));
	return std::string();
}

void ExportController::onStart()
{
	FILE *file = fopen(m_VcardPath.c_str(), "a");
	RETM_IF(!file, "fopen failed: %s", strerror(errno));
	contacts_connect_on_thread();
	size_t currentCount = 0;

	for (auto &&personId : m_PersonIdList) {
		contacts_record_h personRecord = nullptr;
		int err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		LOG_IF_ERR(err, continue, "contacts_db_get_record() failed.");

		char *vcardStream = nullptr;
		err = contacts_vcard_make_from_person(personRecord, &vcardStream);
		contacts_record_destroy(personRecord, true);
		LOG_IF_ERR(err, continue, "contacts_vcard_make_from_person() failed.");

		fputs(vcardStream, file);
		free(vcardStream);

		if (!onProgress(++currentCount)) {
			break;
		}
	}
	fclose(file);
	contacts_disconnect_on_thread();
}

void ExportController::onCanceled()
{
	if (!m_VcardPath.empty()) {
		remove(m_VcardPath.c_str());
	}

	notification_status_message_post(_("IDS_PB_SBODY_EXPORTING_CANCELLED_M_STATUS_ABB"));
}
