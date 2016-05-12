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

#ifndef _CT_COMMON_H_
#define _CT_COMMON_H_

#include <app.h>
#include <Elementary.h>
#include <contacts.h>
#include <string>
#include <storage/storage.h>

#include "CtString.h"

#define _EDJ(obj) (Evas_Object *)elm_layout_edje_get(obj)

/**
 * @brief Converts a markup (HTML-like) string into UTF-8.
 * @param[in]   string The string (in markup) to be converted
 * @remark The returned string should be released.
 *
 * @return The converted string (in UTF-8)
 */
char* ctCommonMarkupToUtf8(const char *string);

/**
 * @brief Removes '\n' symbols.
 * @param[in/out]   src The string to be formatted
 */
void ctCommonRemoveNewline(char *src);

/**
 * @brief Gets contacts list filtered by person Id
 * @param[in]   personId Person Id to be filtered
 * @remark The returned list should be released.
 *
 * @return Filtered contact list
 */
Eina_List* ctCommonGetContactsList(int personId);

/**
 * @brief Gets writable contacts list filtered by person Id
 * @param[in]   personId Person Id to be filtered
 * @remark The returned list should be released.
 *
 * @return Filtered contact list
 */
Eina_List* ctCommonGetWritableContactList(int personId);

/**
 * @brief Destroys all records in the list.
 * @param[in]   list Records list
 */
void ctCommonDestroyRecordList(Eina_List *list);

/**
 * @brief Destroys all records in the list.
 * @param[in]    recordAddressBook The record handle
 * @param[in]    addressbookId     Address book Id
 * @param[out]   providerName      Provider name
 * @param[out]   userName          User name
 * @remark The out values should be released.
 */
void ctCommonGetProviderNameByAddressbookId(contacts_record_h recordAddressBook, int addressbookId ,char** providerName, char**userName);

/**
 * @brief Validates person Id
 * @param[in]   personId Person Id
 * @return true when person Id is valid, otherwise false
 */
bool ctCommonIsValidPerson(const int personId);

/**
 * @brief Gets Eas default group Id
 * @param[in]   addressbook_id Address book Id
 * @return Default group Id on success, otherwise -1
 */
int ctCommonGetEasDefaultGroupId(int addressbook_id);

/**
 * @brief Gets handle for the shared object path.
 * @param[in]   dllName Shared object path
 * @return A handle that can be passed to `dlsym' to get symbol values from it
 */
void* ctGetDllHandle(const char* dllName);

/**
 * @brief Gets address book Id
 * @param[in]   addressbookName Address book name
 * @return Address book Id on success, otherwise -1
 */
int ctCommonGetAddressBookId(char* addressbookName);

/**
 * @brief Gets address book name
 * @param[in]   addressbookId Address book Id
 * @remark The returned value should be released.
 *
 * @return Address book name on success, otherwise NULL
 */
char* ctCommonGetAddressBookName(int addressbookId);

/**
 * @brief Gets checked duplicated number list
 * @param[in]   sourceList Number List
 * @remark The returned list should be released.
 *
 * @return Checked duplicated number list
 */
Eina_List* ctCommonGetChekedDuplicatedNumberList(Eina_List *sourceList);

/**
 * @brief Gets checked duplicated email list
 * @param[in]   sourceList Email List
 * @remark The returned list should be released.
 *
 * @return Checked duplicated email list
 */
Eina_List* ctCommonGetChekedDuplicatedEmailList(Eina_List *sourceList);

/**
 * @brief Gets root storage directory path
 * @param[in]   storageType The storage Type
 *
 * @return The directory path
 */
std::string getRootDirectoryPath(storage_type_e storageType);

/**
 * @brief Gets certain storage directory path
 * @param[in]   storageType The storage Type
 * @param[in]   dirType The directory type
 *
 * @return The directory path
 */
std::string getDirectoryPath(storage_type_e storageType, storage_directory_e dirType);

/**
 * @brief Creates Vcard file
 * @param[in]   record The record handle
 * @return Vcard path
 */
std::string createVcardFile(contacts_record_h record);

/**
 * @brief Launches contact application with operation share
 * @param[in]   personId Person Id
 * @param[in]   isMyProfile True is share of my profile
 */
void sharePerson(int personId, bool isMyProfile);

#endif // _CT_COMMON_H_
