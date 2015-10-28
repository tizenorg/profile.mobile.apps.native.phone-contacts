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

#ifndef _PH_LOG_DEFINES_H
#define _PH_LOG_DEFINES_H

#define PH_TEXT_SHORT_LEN 256

#define _EDJ(obj) (Evas_Object *)elm_layout_edje_get(obj)


enum PHUI_UG_LIST_TYPES{
	PH_UG_REQUEST_SELECT = 11,/**< . */
	PH_UG_REQUEST_SELECT_NUMBER = 12,/**< . */
	PH_UG_REQUEST_SELECT_EMAIL = 13,/**< . */
	PH_UG_REQUEST_SELECT_NUMBER_OR_EMAIL = 17,/**< . */

	PH_UG_REQUEST_SET_WITH_NUM = 21,/**< . */
	PH_UG_REQUEST_SET_WITH_EMAIL = 22,/**< . */
	PH_UG_REQUEST_SET_WITH_WEB = 23,/**< . */
	PH_UG_REQUEST_SET_WITH_IMG = 24,/**< . */
	PH_UG_REQUEST_SET_WITH_RINGTONE = 25,/**< . */

	PH_UG_REQUEST_CHECK = 31,/**< . */
	PH_UG_REQUEST_CHECK_FOR_NUMBER = 32,/**< . */
	PH_UG_REQUEST_CHECK_FOR_EMAIL = 33,/**< . */
	PH_UG_REQUEST_CHECK_FOR_NUMBER_OR_EMAIL = 34,/**< . */

	PH_UG_REQUEST_LAUNCH_VOICECALL = 51,/**< . */

	PH_UG_REQUEST_LAUNCH_HELP_MAKECALLS = 52,/**< . */
	PH_UG_REQUEST_LAUNCH_HELP_SAVECONTACTS = 53,/**< . */
};

enum PH_UG_LOG_TYPES{
	PH_UG_REQUEST_LOG_CHECK = 11,
	PH_UG_REQUEST_LOG_SELECT = 12,
	PH_UG_REQUEST_LOG_DETAIL = 13,
};

/**
 * The key of request bundle for type.
 * \n Value : convert id to string by using \%d.
 */
#define PH_UG_BUNDLE_TYPE "type"

/**
 * The key of result bundle for phonelog index
 * \n The contact index on #PH_UG_REQUEST_LOG_SELECT
 * \n Value : convert id to integer by using atoi()
 */
#define PH_UG_BUNDLE_RESULT_PLOG_ID "plog_id"

/**
 * The key of result bundle for a list of phonelog index
 * \n The phonelog index on #PH_UG_REQUEST_LOG_CHECK
 * \n Value : convert string to a integer array by using g_base64_decode()
 * @par example
 * @code
 void phonelog_result_cb(struct ui_gadget *ug, bundle *result, void *data)
 {
    if(NULL == ug || NULL == data)
       return;

    if(result) {
       const char *val;
       int *list, len, i;

       val = bundle_get_val(result, PH_UG_BUNDLE_RESULT_PLOG_ID_LIST);
       list = (int *)g_base64_decode(val, &len);

       for(i=0;i<len/sizeof(int);i++)
          // handle operation for each list[i]

       g_free(list);
    }
 }
 *
 * @endcode
 */
#define PH_UG_BUNDLE_RESULT_PLOG_ID_LIST "get_select_list"        // TODO : change to "plog_id_list"

/**
 * @}
 */

#define LOG_LIST_CALL_TYPE_SIZE						45
#define LOG_LIST_INFO_BTN_SIZE						50

#endif //_PH_LOG_DEFINES_H
