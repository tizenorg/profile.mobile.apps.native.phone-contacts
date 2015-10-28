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

#ifndef _WIDGET_ITEM_LAYOUT_METRICS_H_
#define _WIDGET_ITEM_LAYOUT_METRICS_H_

#define ITEM_PAD_X 4
#define ITEM_PAD_Y 14

#define ITEM_WIDTH (ITEM_PAD_X+174)
#define ITEM_HEIGHT (ITEM_PAD_Y+206)

#define THUMBNAIL_SIZE 118
#define THUMBNAIL_OFFSET_X (ITEM_WIDTH-THUMBNAIL_SIZE)/2
#define THUMBNAIL_OFFSET_Y ITEM_PAD_Y

#define BUTTON_DELETE_SIZE 62
#define BUTTON_DELETE_OFFSET_X 8
#define BUTTON_DELETE_OFFSET_Y 8

#define ICON_TYPE_SIZE 54
#define ICON_TYPE_OFFSET_X 20

#define NAME_HEIGHT 80
#define NAME_OFFSET_X ITEM_PAD_X/2
#define NAME_OFFSET_Y (THUMBNAIL_OFFSET_Y+THUMBNAIL_SIZE+6)
#define NAME_TEXT_SIZE 28

#endif /* _WIDGET_ITEM_LAYOUT_METRICS_H_ */
