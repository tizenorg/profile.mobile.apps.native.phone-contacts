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

#ifndef __CT_STRING_H__
#define __CT_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <libintl.h>

#define V_(str) dgettext("contacts", str)

/**
 * @brief Localizes string by Message Id
 * @param[in]   did Message Id
 * @return localized string
 */
const char* ctGetContactsString(const char* did);

#ifdef __cplusplus
}
#endif

#endif // __CT_STRING_H__

