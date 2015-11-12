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

#ifndef DIALER_LAYOUT_H
#define DIALER_LAYOUT_H

#include "DialerPath.h"

#define GROUP_DIALER                    "dialer"
#define PART_SWALLOW_ENTRY              "swallow.entry"
#define PART_SWALLOW_PREDICTIVE         "swallow.predictive"
#define PART_SWALLOW_KEYPAD             "swallow.keypad"
#define PART_SWALLOW_CALL               "swallow.call"
#define PART_SWALLOW_BACKSPACE          "swallow.backspace"

#define GROUP_PREDICTIVE_NO_RESULTS     "predictive_noresults"
#define PART_TEXT_ADD                   "text.add"

#define GROUP_PREDICTIVE                "predictive"
#define PART_SWALLOW_THUMBNAIL          "swallow.thumbnail"
#define PART_SWALLOW_SPEEDDIAL          "swallow.speeddial"
#define PART_TEXT_NAME                  "text.name"
#define PART_TEXT_NUMBER                "text.number"
#define PART_TEXT_1_LINE                "text.1line"
#define PART_SWALLOW_RESULTS            "swallow.results"

#define GROUP_SPEEDDIAL_NUMBER          "speeddial_number"

#define GROUP_PREDICTIVE_RES_COUNT      "predictive_res_count"
#define PART_TEXT_COUNT                 "text.count"
#define PART_SWALLOW_ARROW              "swallow.arrow"

#define GROUP_BUTTON_CALL               "button_call"
#define GROUP_BUTTON_BACKSPACE          "button_backspace"

#define GROUP_BUTTON_1                  "keypad_button_1"
#define GROUP_BUTTON_2                  "keypad_button_2"
#define GROUP_BUTTON_3                  "keypad_button_3"
#define GROUP_BUTTON_4                  "keypad_button_4"
#define GROUP_BUTTON_5                  "keypad_button_5"
#define GROUP_BUTTON_6                  "keypad_button_6"
#define GROUP_BUTTON_7                  "keypad_button_7"
#define GROUP_BUTTON_8                  "keypad_button_8"
#define GROUP_BUTTON_9                  "keypad_button_9"
#define GROUP_BUTTON_ASTERISK           "keypad_button_asterisk"
#define GROUP_BUTTON_0                  "keypad_button_0"
#define GROUP_BUTTON_SHARP              "keypad_button_sharp"

#define BUTTON_CALL_NORMAL              89, 176, 58, 255
#define BUTTON_CALL_PRESSED             127, 184, 106, 255

#endif /* DIALER_LAYOUT_H */
