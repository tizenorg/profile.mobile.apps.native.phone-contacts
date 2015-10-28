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

#ifndef _CONTACTS_NUMBER_ITEM_ICONS_H_
#define _CONTACTS_NUMBER_ITEM_ICONS_H_

#include "WDefine.h"

/**
 * @brief Create layout with "call" and "message" icons.
 * @param[in]   parent   parent object
 * @param[in]   number   number for call or message
 * @return The pointer to created evas object
 */
WAPP_ASSIST_EXPORT Evas_Object *createNumberItemIconsLayout(Evas_Object *parent, const char *number = nullptr);

#endif /* _CONTACTS_NUMBER_ITEM_ICONS_H_ */
