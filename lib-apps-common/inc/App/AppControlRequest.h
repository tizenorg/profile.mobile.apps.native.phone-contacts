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

#ifndef APP_APP_CONTROL_REQUEST_H
#define APP_APP_CONTROL_REQUEST_H

#include "App/AppControl.h"
#include <string>
#include <vector>

#define APP_CONTROL_OPERATION_SETTING_CALL "http://tizen.org/appcontrol/operation/setting/call"

#define APP_CONTROL_MIME_CONTACT "application/vnd.tizen.contact"

#define APP_CONTROL_SELECT_SINGLE   "single"
#define APP_CONTROL_SELECT_MULTIPLE "multiple"

#define APP_CONTROL_RESULT_ID       "id"
#define APP_CONTROL_RESULT_PHONE    "phone"
#define APP_CONTROL_RESULT_EMAIL    "email"
#define APP_CONTROL_RESULT_VCARD    "vcf"
#define APP_CONTROL_RESULT_ACTION   "action"
#define APP_CONTROL_RESULT_CALL     "call"
#define APP_CONTROL_RESULT_MESSAGE  "message"

namespace App
{
	/**
	 * @brief Request contact creation operation
	 * @param[in]   number  Phone number to save to the new contact
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestContactCreate(const char *number);

	/**
	 * @brief Request contact view operation
	 * @param[in]   personId    ID of person to view
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestContactDetails(int personId);

	/**
	 * @brief Request contact edit operation
	 * @param[in]   personId    ID of person to edit or 0 to launch pick first
	 * @param[in]   number      Phone number to add to the edited contact
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestContactEdit(int personId, const char *number);

	/**
	 * @brief Request contact pick operation
	 * @param[in]   selectionMode   One of APP_CONTROL_SELECT_* values
	 * @param[in]   resultType      One of APP_CONTROL_RESULT_* values
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestContactPick(const char *selectionMode, const char *resultType);

	/**
	 * @brief Request telephony call
	 * @param[in]   number  Number to call
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestTelephonyCall(const char *number);

	/**
	 * @brief Request launch call settings
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestCallSettings();

	/**
	 * @brief Request message composer
	 * @param[in]   scheme      URI scheme (e.g. sms:, mmsto: or mailto:)
	 * @param[in]   to          Message recipient
	 * @param[in]   subject     Message subject
	 * @param[in]   text        Message text
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestMessageComposer(const char *scheme, const char *to,
			const char *subject = nullptr, const char *text = nullptr);

	/**
	 * @brief Request camera image capture
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestCameraImage();

	/**
	 * @brief Request pick image from gallery
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestGalleryImage();

	/**
	 * @brief Request share contact via other application
	 * @param[in]   personId    Contact's person ID
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestShareContact(int personId);

	/**
	 * @brief Request share contact in text representation via other application
	 * @param[in]   text    All person information in text representation
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestShareContactAsText(std::string text);

	/**
	 * @brief Request share "My Profile" via other application
	 * @param[in]   recordId    "My Profile" record ID
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestShareMyProfile(int recordId);

	/**
	 * @brief Request share of multiple contacts via other application
	 * @param[in]   personIds   Contact's person IDs
	 * @param[in]   count       Person IDs count
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestMultiShareContacts(const char **personIds, int count);

	/**
	 * @brief Request pick vcard(s) from filesystem
	 * @param[in]   path    Path of storage from where files should be picked
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestPickVcard(const char *path);

	/**
	 * @brief Request pick ringtone from Settings
	 * @param[in]   selectedPath    Path of currently selected ringtone
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestPickRingtone(const char *selectedPath);

	/**
	 * @brief Get single first string from App Control array extra data.
	 * @param[in]   appControl  App Control handle
	 * @param[in]   key         Extra data array key
	 * @return First string from array on success, otherwise empty string.
	 */
	std::string EXPORT_API getSingleExtraData(app_control_h appControl, const char *key);

	/**
	 * @brief Get integer value from App Control array extra data.
	 * @param[in]   appControl  App Control handle
	 * @param[in]   key         Extra data array key
	 * @return Integer value.
	 */
	int EXPORT_API getIntExtraData(app_control_h appControl, const char *key);

	/**
	 * @brief Get vector of integer values from App Control array extra data.
	 * @param[in]   appControl  App Control handle
	 * @param[in]   key         Extra data array key
	 * @return Vector of integer values on success, otherwise empty vector.
	 */
	std::vector<int> EXPORT_API getIntExtraDataArray(app_control_h appControl, const char *key);
}

#endif /* APP_APP_CONTROL_REQUEST_H */
