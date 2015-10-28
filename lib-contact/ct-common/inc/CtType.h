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

#ifndef _CT_TYPE_H_
#define _CT_TYPE_H_

#include <app_control.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CT_EDITFIELD_MAX_CHAR 1000
#define CT_MAIN_NAME_EDITFIELD_MAX_CHAR 5005
#define CT_SIMNAME_EDITFIELD_MAX_CHAR 14
#define CT_SIMNUMBER_EDITFIELD_MAX_CHAR 40


#define CT_TEXT_MAX_LEN 1024
#define CT_HELP_TEXT_MAX_LEN 4096
#define CT_TEXT_SHORT_LEN 256
#define CT_EDITFIELD_MAX_BUFFER 1000*3
#define CT_SEARCHBAR_MAX_CHAR 100

#define CT_KEY_APPSVC_TIMER "appsvc_timer"
#define CT_KEY_SERVICE "app_control_h"
#define CT_KEY_LIST_MFC_PERSON_IDS "db/private/org.tizen.contacts/list_mfc_person_ids"
#define CT_KEY_LIST_DISPLAY_ADDRESSBOOK "db/private/org.tizen.contacts/list_display_addressbook"
#define CT_KEY_INPUT_BASE_ADDRESSBOOK "db/private/org.tizen.contacts/input_base_addressbook"
#define CT_KEY_SETTING_CONTACT_SHARING_SETTINGS "db/private/org.tizen.contacts/contact_sharing_settings"
#define CT_KEY_CONTACT_CREATE_FIRST "db/private/org.tizen.contacts/contact_create_first"
#define CT_KEY_MYPROFILE_CREATE_FIRST "db/private/org.tizen.contacts/myprofile_create_first"
#define CT_KEY_DISPLAY_WITH_NUMBER "db/private/org.tizen.contacts/display_contacts_with_number"
#define CT_KEY_LAST_SELECTED_TAB_INDEX_FOR_PHONE "db/private/org.tizen.contacts/last_selected_tab_index_for_phone"
#define CT_KEY_SHOW_LINK_CONTACTS_TOAST_POPUP "db/private/org.tizen.contacts/show_link_contacts_toast_popup"
#define CT_KEY_NUM_OF_LINKABLE_CONTACTS "db/private/org.tizen.contacts/num_of_linkable_contacts"
#define CT_SIM_ADDRESSBOOK_NAME "http://tizen.samsung.com/addressbook/sim"
#define CT_EAS_ADDRESSBOOK_NAME "Microsoft Exchange ActiveSync"
#define CT_EAS_ADDRESSBOOK_DISPLAY_NAME "Exchange ActiveSync"
#define CT_FACEBOOK_ADDRESSBOOK_NAME "Sync apps with Facebook"

#define CT_KEY_SIM_CARD_NUMBER "sim_card_number"

typedef enum {
	LIST_DATA_DEFAULT, /**< List data default */
	LIST_DATA_ALL_PERSON, /**< List data of all person */
	LIST_DATA_ALL_FAVORITES, /**< List data of all favorites */
	LIST_DATA_PERSON_EXCEPT_SIM, /**< List data of persons without sim */
	LIST_DATA_PERSON_WITH_NUMBER, /**< List data of persons with number */
	LIST_DATA_PERSON_EXCEPT_FACEBOOK, /**< List data of persons except facebook */
} CtListDataType;

typedef enum {
	LIST_CTRL_DEFAULT, /**< Default control. Launches detail */
	LIST_CTRL_DELETE_CONTACTS, /**< Control delete contacts */
	LIST_CTRL_SHARE_CONTACTS, /**< Control share contacts */
	LIST_CTRL_ADD_FAVORITES, /**< Control add favorites */
	LIST_CTRL_REMOVE_FAVORITES, /**< Control remove favorites */
	LIST_CTRL_REORDER_FAVORITES, /**< Control reorder favorites */
	LIST_CTRL_LINK_CONTACT, /**< Control link contact */

	LIST_CTRL_REQUEST_CHECK_FOR_PERSON, /**< Control request check for pick multiple */
	LIST_CTRL_REQUEST_CHECK_FOR_NUMBER, /**< Control request check for number */
	LIST_CTRL_REQUEST_CHECK_FOR_EMAIL, /**< Control request check for email */
	LIST_CTRL_REQUEST_CHECK_FOR_VCARD, /**< Control request check for Vcard */
	LIST_CTRL_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL, /**< Control request check for number or email */
	LIST_CTRL_REQUEST_CHECK_FOR_LINK_CONTACTS, /**< Control request check for link contacts */
} CtListCtrlType;

typedef enum {
	LIST_VIEW_DEFAULT, /**< List view default */
	LIST_VIEW_CHECK, /**< List view check */
	LIST_VIEW_REORDER, /**< List view reorder */
} CtListViewType;

typedef enum {
	CHECK_NONE = -1, /**< Check NONE */
	CHECK_OFF, /**< Check OFF */
	CHECK_ON, /**< Check ON */
} CtCheckState;

typedef enum {
	RESULT_FAILED = APP_CONTROL_RESULT_FAILED, /**< Operation failed by the callee */
	RESULT_SUCCESS = APP_CONTROL_RESULT_SUCCEEDED, /**< Operation succeeded */
	RESULT_CANCELLED = APP_CONTROL_RESULT_CANCELED /**< Operation canceled by the framework */
} CtResultType;

#ifdef __cplusplus
}
#endif

#endif
