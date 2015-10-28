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

#ifndef _CONTACTS_UTILS_H_
#define _CONTACTS_UTILS_H_

#include <contacts.h>

#define CONTACTS_LIST_FOREACH(list, record) \
	bool success = (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE); \
	for( ; success; \
		success = ((contacts_list_next(list) == CONTACTS_ERROR_NONE) \
		&& (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE)) \
		)

#endif /* _CONTACTS_UTILS_H_ */
