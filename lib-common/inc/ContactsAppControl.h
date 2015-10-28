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

#ifndef _CONTACTS_APP_CONTROL_H_
#define _CONTACTS_APP_CONTROL_H_

#include <WDefine.h>
#include <app_control.h>

#define APP_CONTROL_OPERATION_UPDATE "http://tizen.org/appcontrol/operation/update"
#define APP_CONTROL_MIME_CONTACT "application/vnd.tizen.contact"

#define APP_CONTROL_DATA_NAME "http://tizen.org/appcontrol/data/name"

#define APP_CONTROL_DATA_SELECTION_MODE_SINGLE "single"
#define APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE "multiple"

#define APP_CONTROL_DATA_TYPE_ID "id"
#define APP_CONTROL_DATA_TYPE_PHONE "phone"
#define APP_CONTROL_DATA_TYPE_EMAIL "email"
#define APP_CONTROL_DATA_TYPE_PHONE_OR_EMAIL "phone_or_email"
#define APP_CONTROL_DATA_TYPE_VCARD "vcf"

#define APP_CONTROL_DATA_TYPE_PHONE_CALL "phone_call"
#define APP_CONTROL_DATA_TYPE_PHONE_MESSAGE "phone_message"

#define APP_CONTROL_DATA_ID "http://tizen.org/appcontrol/data/id"
#define APP_CONTROL_DATA_TYPE "http://tizen.org/appcontrol/data/type"
#define APP_CONTROL_DATA_PHONE "http://tizen.org/appcontrol/data/phone"
#define APP_CONTROL_OPERATION_ADD "http://tizen.org/appcontrol/operation/add"
#define APP_CONTROL_DATA_TOTAL_COUNT  "http://tizen.org/appcontrol/data/total_count"
#define APP_CONTROL_DATA_EMAIL "http://tizen.org/appcontrol/data/email"
#define APP_CONTROL_DATA_URL  "http://tizen.org/appcontrol/data/url"

/**
 * @brief Get integer extra data from app_control
 * @param[in]   appControl  app_control handle
 * @param[in]   key         Extra data key
 * @return Integer representation of string extra data or 0 on failure
 */
WAPP_ASSIST_EXPORT int appControlGetInt(app_control_h appControl, const char *key);

/**
 * @brief Request contact creation operation
 * @param[in]   number  Phone number to save to the new contact
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchContactCreate(const char *number);

/**
 * @brief Request contact view operation
 * @param[in]   personId    ID of person to view
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchContactDetails(int personId);

/**
 * @brief Request contact edit operation
 * @param[in]   personId    ID of person to edit or 0 to launch pick first
 * @param[in]   number      Phone number to add to the edited contact
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchContactEdit(int personId, const char *number);

/**
 * @brief Request contact pick operation
 * @param[in]   selectionMode   One of APP_CONTROL_DATA_SELECTION_MODE_* values
 * @param[in]   resultType      One of APP_CONTROL_DATA_TYPE_* values
 * @param[in]   replyCallback   Callback to receive selection result
 * @param[in]   userData        Data passed to @a replyCallback
 * @param[in]   groupMode       Whether to group launched application with current
 * @param[out]  appControl      Created App Control handle if not nullptr
 * @remark If appControl is not nullptr it should be destroyed by the caller
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchContactPick(const char *selectionMode, const char *resultType,
		app_control_reply_cb replyCallback, void *userData,
		bool groupMode = true, app_control_h *appControl = nullptr);

/**
 * @brief Request telephony call
 * @param[in]   number  Number to call
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchCall(const char *number);

/**
 * @brief Request compose message
 * @param[in]   scheme      URI scheme (e.g. sms:, mmsto: or mailto:)
 * @param[in]   to          Message recipient
 * @param[in]   subject     Message subject
 * @param[in]   text        Message text
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchMessageComposer(const char *scheme, const char *to, const char *subject = nullptr, const char *text = nullptr,
		bool groupMode = true);

/**
 * @brief Request single share of contact
 * @param[in]   personId    person id
 * @param[in]   isMyProfile true if person id is My profile id
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchShareContact(const char *personId, bool isMyProfile);

/**
 * @brief Request multi share of contacts
 * @param[in]   personIdArray   array of person id
 * @param[in]   count           person id count
 * @return App Control launch result
 * @see app_control_send_launch_request()
 */
WAPP_ASSIST_EXPORT int launchMultiShareContacts(const char **personIdArray, int count);

#endif /* _CONTACTS_APP_CONTROL_H_ */
