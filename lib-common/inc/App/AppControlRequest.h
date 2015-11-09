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

#define APP_CONTROL_MIME_CONTACT "application/vnd.tizen.contact"

#define APP_CONTROL_SELECT_SINGLE "single"
#define APP_CONTROL_SELECT_MULTIPLE "multiple"

#define APP_CONTROL_RESULT_ID "id"
#define APP_CONTROL_RESULT_PHONE "phone"
#define APP_CONTROL_RESULT_EMAIL "email"
#define APP_CONTROL_RESULT_VCARD "vcf"

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
	 * @brief Request message composer
	 * @param[in]   scheme      URI scheme (e.g. sms:, mmsto: or mailto:)
	 * @param[in]   to          Message recipient
	 * @param[in]   subject     Message subject
	 * @param[in]   text        Message text
	 * @return AppControl wrapper
	 */
	AppControl EXPORT_API requestMessageComposer(const char *scheme, const char *to,
			const char *subject = nullptr, const char *text = nullptr);
}

#endif /* APP_APP_CONTROL_REQUEST_H */
