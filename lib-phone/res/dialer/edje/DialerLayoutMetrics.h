/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef DIALER_LAYOUT_METRICS_H
#define DIALER_LAYOUT_METRICS_H

#define TOTAL_W                             720
#define TOTAL_H                             1120

#define ENTRY_H                             296
#define PREDICTIVE_H                        144
#define KEYPAD_H                            528
#define ACTION_PANEL_H                      152

#define BUTTON_NUMBER_W                     238
#define BUTTON_NUMBER_H                     130
#define DIVIDER_SIZE                        2

#define BUTTON_PART_W                       108
#define BUTTON_PART_NUMBER_H                71
#define BUTTON_PART_LETTERS_H               36

#define OFFSET_BUTTON_PART_X                65
#define OFFSET_BUTTON_PART_Y                12

#define ACTION_BUTTONS_H                    152

#define BUTTON_CALL_SIZE                    104
#define BUTTON_CALL_X                       ((TOTAL_W-BUTTON_CALL_SIZE)/2)
#define BUTTON_CALL_Y                       ((ACTION_BUTTONS_H-BUTTON_CALL_SIZE)/2)

#define BUTTON_BACKSPACE_SIZE               90
#define BUTTON_BACKSPACE_X                  82
#define BUTTON_BACKSPACE_Y                  ((ACTION_BUTTONS_H-BUTTON_BACKSPACE_SIZE)/2)

#define PREDICTIVE_PICTURE_BG_W             162
#define PREDICTIVE_PICTURE_SIZE             98
#define PREDICTIVE_MAIN_TEXT_H              57
#define PREDICTIVE_SUB_TEXT_H               45
#define PREDICTIVE_TEXT_W                   446
#define PREDICTIVE_PARTS_H                  48
#define PREDICTIVE_PARTS_W                  48
#define PREDICTIVE_RESULTS_BG_W             112
#define PREDICTIVE_ADD_SIZE                 80

#define OFFSET_PREDICTIVE_Y                 24
#define OFFSET_PREDICTIVE_TEXT_Y            ((PREDICTIVE_H-PREDICTIVE_MAIN_TEXT_H-PREDICTIVE_SUB_TEXT_H)/2)
#define OFFSET_PREDICTIVE_1_LINE_TEXT_Y     ((PREDICTIVE_H-PREDICTIVE_MAIN_TEXT_H)/2)
#define SPACER_PREDICTIVE_Y                 1
#define SPACER_PREDICTIVE_X                 32
#define OFFSET_PREDICTIVE_ADD               32

#define SPEEDDIAL_AREA_W                    76
#define SPEEDDIAL_AREA_H                    45
#define SPEEDDIAL_IC_SIZE                   28
#define SPEEDIAL_TEXT_W                     18

#define SPEEDDIAL_SPACER                    12
#define OFFSET_INNER_SPEEDDIAL_AREA_H       ((SPEEDDIAL_AREA_W-SPEEDDIAL_IC_SIZE-SPEEDIAL_TEXT_W)/2)
#define OFFSET_T_SPEEDDIAL_IC               9
#define OFFSET_B_SPEEDDIAL_IC               8

#endif /* DIALER_LAYOUT_METRICS_H */
