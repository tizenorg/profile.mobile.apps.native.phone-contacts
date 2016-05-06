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

#include <dlfcn.h>
#include <Elementary.h>
#include <app.h>
#include <Eina.h>
#include <metadata_extractor.h>
#include <account.h>
#include <telephony/telephony.h>
#include <storage.h>
#include <string>
#include <fcntl.h>


#include <errno.h>
#include <system_settings.h>

#include "CtCommon.h"
#include "CtType.h"
#include "CtPath.h"
#include "ContactsDebug.h"
#include "ContactsAppControl.h"

#define FILE_ACCESS_MODE 0644
#define VCARD_NAME_SUFFIX_LEN 16

static bool __ctCommongetProviderCb(char* appId, char* label, char* locale, void* userData);

char* ctCommonMarkupToUtf8(const char *string)
{
	unsigned int i;
	char *ret = NULL;

	ret = elm_entry_markup_to_utf8(string);
	WPRET_VM(NULL == ret, NULL, "elm_entry_markup_to_utf8() return NULL");

	for (i = 0; i < strlen(ret); i++)
		if (' ' != ret[i]) return ret;

	ret[0] = '\0';
	return ret;
}

void ctCommonRemoveNewline(char *src)
{
	int i=0;

	if(!src)
		return;

	while(src[i])
	{
		if (src[i] == '\n')
			src[i] = ' ';
		i++;
	}
}

Eina_List* ctCommonGetContactsList(int personId)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h contactRecord = NULL;
	contacts_record_h contactRecordClone = NULL;
	contacts_list_h contactList = NULL;
	Eina_List *contactRcordList = NULL;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;

	err = contacts_filter_create(_contacts_contact._uri, &filter);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contactcs_filter_create() Failed(%d)", err);
	err = contacts_filter_add_int(filter, _contacts_contact.person_id, CONTACTS_MATCH_EQUAL, personId);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_filter_add_int() Failed(%d)", err);
		contacts_filter_destroy(filter);
		return NULL;
	}
	err = contacts_query_create(_contacts_contact._uri, &query);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_query_create() Failed(%d)", err);
		contacts_filter_destroy(filter);
		return NULL;
	}
	err = contacts_query_set_filter(query, filter);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_query_set_filter() Failed(%d)", err);
		contacts_filter_destroy(filter);
		contacts_query_destroy(query);
		return NULL;
	}
	err = contacts_db_get_records_with_query(query, 0, 0, &contactList);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_db_get_records_with_query() Failed(%d)", err);
		contacts_list_destroy(contactList, true);
		contacts_filter_destroy(filter);
		contacts_query_destroy(query);
		return NULL;
	}

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);

	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(contactList, &contactRecord)) {
		err = contacts_record_clone(contactRecord, &contactRecordClone);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_clone() Failed(%d)", err);
			break;
		}
		contactRcordList = eina_list_append(contactRcordList, contactRecordClone);
		contacts_list_next(contactList);
	}
	contacts_list_destroy(contactList, true);
	return contactRcordList;
}

Eina_List* ctCommonGetWritableContactList(int personId)
{
	int addressbookId;
	int addressbookMode;
	Eina_List *contactList = NULL;

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h addressbookRecord = NULL;
	contacts_record_h contactRecordClone = NULL;

	WPRET_VM(personId <= 0, NULL, "Invalid parameter(%d)", personId);

	Eina_List* list = ctCommonGetContactsList(personId);
	WPRET_VM(list == NULL, NULL, "contact list is null");

	void *data = NULL;
	Eina_List *cursor = NULL;
	EINA_LIST_FOREACH(list, cursor, data) {
		contacts_record_h contactRecord = (contacts_record_h)data;

		err = contacts_record_get_int(contactRecord, _contacts_contact.address_book_id, &addressbookId);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			break;
		}

		err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &addressbookRecord);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_db_get_record() Failed(%d)", err);
			break;
		}

		err = contacts_record_get_int(addressbookRecord, _contacts_address_book.mode, &addressbookMode);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			err = contacts_record_destroy(addressbookRecord, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
			break;
		}

		char *name = NULL;
		err = contacts_record_get_str_p(addressbookRecord, _contacts_address_book.name, &name);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			err = contacts_record_destroy(addressbookRecord, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
			break;
		}

		if (addressbookMode & CONTACTS_ADDRESS_BOOK_MODE_READONLY) {
			err = contacts_record_destroy(addressbookRecord, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
			continue;
		}

		err = contacts_record_destroy(addressbookRecord, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);

		err = contacts_record_clone(contactRecord, &contactRecordClone);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_clone() Failed(%d)", err);
			break;
		}

		contactList = eina_list_append(contactList, contactRecordClone);
	}

	ctCommonDestroyRecordList(list);

	return contactList;
}

void ctCommonDestroyRecordList(Eina_List *list)
{
	int err = CONTACTS_ERROR_NONE;
	void *data = NULL;
	EINA_LIST_FREE(list, data) {
		contacts_record_h record = (contacts_record_h)data;
		err = contacts_record_destroy(record, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
	}
}

bool __ctCommonCheckDirtyNumber(char digit)
{
	switch (digit)
	{
	case '/':
	case 'N':
	case '.':
	case '0' ... '9':
	//case 'p':
	//case 'w':
	//case 'P':
	//case 'W':
	case '#':
	case '*':
	case '(':
	case ')':
	case ',':
	case ';':
	case '+':
	case '-':
	//case ' ':
		return false;
	default:
		return true;
	}
}

bool __ctCommongetProviderCb(char* appId, char* label, char* locale, void* userData)
{
	char ** name = (char **)userData;
	if(strcmp(locale,"default") == 0) {
		*name = SAFE_STRDUP(label);
	}

	return true;
}

void ctCommonGetProviderNameByAddressbookId(contacts_record_h recordAddressBook, int addressbookId ,char** providerName, char**userName)
{
	WHIT();
	int ret;
	int accountId;
	char *appId = NULL;
	char *addressbookName = NULL;
	char *addressbookUsername = NULL;
	account_h account = NULL;
	int err = CONTACTS_ERROR_NONE;
//	contacts_record_h record = NULL;

	//WASSERT(NULL == providerName);
	SAFE_FREE(*providerName);

	if (addressbookId <0){
		return;
	}

	err = contacts_record_get_str_p(recordAddressBook,_contacts_address_book.name,&addressbookName);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

	WDEBUG("addressbookName %s", addressbookName);

	if(!strcmp(addressbookName, "http://tizen.org/addressbook/phone"))
		*providerName = strdup(V_("IDS_PB_OPT_DEVICE"));
	else if(!strcmp(addressbookName, CT_SIM_ADDRESSBOOK_NAME"/0")) {
		*providerName = strdup("SIM1");
	}
	else if(!strcmp(addressbookName, CT_SIM_ADDRESSBOOK_NAME"/1")) {
		*providerName = strdup("SIM2");
	}
	else if(!strcmp(addressbookName, CT_SIM_ADDRESSBOOK_NAME)) { //for one sim device
		*providerName = strdup("SIM");
	}

	err = contacts_record_get_int(recordAddressBook, _contacts_address_book.account_id, &accountId);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_get_int() Failed(%d)", err);
		return;
	}

	if (0 == accountId)
		return;

	account = NULL;
	ret = account_create(&account);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_create() Failed(%d)", ret);
		return;
	}

	ret = account_query_account_by_account_id(accountId, &account);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_query_account_by_account_id(%d) Failed(%d)", accountId, ret);
		account_destroy(account);
		return;
	}

	if(userName) {
		ret = account_get_user_name(account, &addressbookUsername);
		if (ACCOUNT_ERROR_NONE != ret) {
			WERROR("account_get_display_name() Failed(%d)", ret);
			account_destroy(account);
			return;
		}
		WDEBUG("user name:%s", addressbookUsername);
		if(addressbookUsername)
			*userName = addressbookUsername;
		else
			*userName = NULL;
	}

	ret = account_get_package_name(account, &appId);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_get_display_name() Failed(%d)", ret);
		account_destroy(account);
		return;
	}

	WERROR("package_name = %s", appId);

	ret = account_type_query_label_by_app_id(__ctCommongetProviderCb, appId, providerName);
	WPWARN(ACCOUNT_ERROR_NONE != ret, "account_type_query_label_by_app_id() Failed(%d)", ret);
	if(*providerName == NULL){
		WERROR("provider_name is null");
		ret = account_get_domain_name(account, providerName);
		WPWARN(ACCOUNT_ERROR_NONE != ret, "account_get_domain_name() is Failed(%d)", ret);
	}
	account_destroy(account);
	free(appId);

	WDEBUG("providerName %s addressbookName %s", *providerName, addressbookName);
	if(*providerName && !strcmp(*providerName, CT_EAS_ADDRESSBOOK_NAME)) {
		free(*providerName);
		*providerName = strdup(CT_EAS_ADDRESSBOOK_DISPLAY_NAME);
		if(userName) {
			free(*userName);
			*userName = strdup(addressbookName);
		}
	}
	return;
}

bool ctCommonIsValidPerson(const int personId)
{
	int err = CONTACTS_ERROR_NONE;
	bool isValidPerson = false;
	contacts_record_h tempRecord = NULL;

	err = contacts_db_get_record(_contacts_person._uri, personId, &tempRecord);
	if (CONTACTS_ERROR_NONE == err) {
		isValidPerson = true;
	}

	contacts_record_destroy(tempRecord, true);

	return isValidPerson;
}

int ctCommonGetEasDefaultGroupId(int addressbook_id)
{
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	contacts_list_h list = NULL;
	int ret = 0;
	contacts_record_h group = NULL;
	contacts_record_h addressbook = NULL;
	int eas_default_group_id =0;
	int account_id = 0;
	account_h account = NULL;
	char *domain_name = NULL;

	ret = contacts_db_get_record(_contacts_address_book._uri, addressbook_id, &addressbook);
	WPRET_VM(CONTACTS_ERROR_NONE != ret, -1, "contacts_db_get_record() Failed(%d)", ret);

	ret = contacts_record_get_int(addressbook, _contacts_address_book.account_id, &account_id);
	if(CONTACTS_ERROR_NONE != ret) {
		WERROR("contacts_record_get_int() Failed(%d)", ret);

		contacts_record_destroy(addressbook, true);
		return -1;
	}

	ret = contacts_record_destroy(addressbook, true);
	if(CONTACTS_ERROR_NONE != ret) {
		WERROR("contacts_record_destroy() Failed(%d)", ret);
		return -1;
	}

	if (0 == account_id) {
		return -1;
	}

	ret = account_create(&account);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_create() Failed(%d)", ret);

		return -1;
	}

	ret = account_query_account_by_account_id(account_id, &account);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_query_account_by_account_id() Failed(%d)", ret);
		account_destroy(account);
		return -1;
	}

	ret = account_get_domain_name(account, &domain_name);
	if (ACCOUNT_ERROR_NONE != ret) {
		WERROR("account_get_display_name() Failed(%d)", ret);
		if(domain_name)
			free(domain_name);
		account_destroy(account);
		return -1;
	}
	if(domain_name && strcmp(domain_name, "Exchange") != 0){
		WERROR("domain name: %s", domain_name);
		free(domain_name);
		account_destroy(account);
		return -1;
	}
	free(domain_name);
	account_destroy(account);


	ret = contacts_filter_create(_contacts_group._uri, &filter);
	WPRET_VM(CONTACTS_ERROR_NONE != ret, -1, "contacts_filter_create() Failed(%d)", ret);

	ret = contacts_filter_add_int(filter, _contacts_group.address_book_id, CONTACTS_MATCH_EQUAL, addressbook_id);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_filter_add_int() Failed(%d)", ret);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_filter_add_operator() Failed(%d)", ret);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_filter_add_str(filter, _contacts_group.name, CONTACTS_MATCH_EXACTLY, "Contacts");
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_filter_add_str() Failed(%d)", ret);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_query_create(_contacts_group._uri, &query);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_query_create() Failed(%d)", ret);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_query_set_filter(query, filter);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_query_create() Failed(%d)", ret);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_db_get_records_with_query(query, 0, 0, &list);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_db_get_records_with_query() Failed(%d)", ret);
		contacts_list_destroy(list, true);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_list_get_current_record_p(list, &group);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_list_get_current_record_p() Failed(%d)", ret);
		contacts_list_destroy(list, true);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_record_get_int(group, _contacts_group.id, &eas_default_group_id);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_record_get_int() Failed(%d)", ret);
		contacts_list_destroy(list, true);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_list_destroy(list, true);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_record_get_int() Failed(%d)", ret);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_query_destroy(query);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_query_destroy() Failed(%d)", ret);
		contacts_filter_destroy(filter);
		return -1;
	}

	ret = contacts_filter_destroy(filter);
	if(CONTACTS_ERROR_NONE != ret)  {
		WERROR("contacts_record_get_int() Failed(%d)", ret);
		return -1;
	}

	return eas_default_group_id;
}

void* ctGetDllHandle(const char* dllName)
{
	if (strcmp(dllName, "libct-setting.so") == 0) {
		static void* settingDllHandle = NULL;
		if (settingDllHandle == NULL) {
			settingDllHandle = dlopen("libct-setting.so", RTLD_LAZY);
			if (!settingDllHandle) {
				WERROR("%s", dlerror());
			}
		}

		return settingDllHandle;
	}

	return NULL;
}

int ctCommonGetAddressBookId(char* addressbookName)
{
	int err = CONTACTS_ERROR_NONE;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	contacts_list_h list = NULL;
	contacts_record_h record = NULL;
	int addressbookId = -1;//0 is phone addressbook id
	WPRET_VM(NULL == addressbookName, -1, "addressbookName is null");

	err = contacts_filter_create(_contacts_address_book._uri, &filter);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
	err = contacts_filter_add_str(filter,_contacts_address_book.name , CONTACTS_MATCH_FULLSTRING, addressbookName);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_str() Failed(%d)", err);
	err = contacts_query_create(_contacts_address_book._uri, &query);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_create() Failed(%d)", err);
	err = contacts_query_set_filter(query, filter);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_filter() Failed(%d)", err);
	err = contacts_db_get_records_with_query(query, 0, 0, &list);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_records_with_query() Failed(%d)", err);
	err = contacts_list_get_current_record_p(list, &record);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);
	if (record){
		err = contacts_record_get_int(record,_contacts_address_book.id, &addressbookId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
	}

	contacts_list_destroy(list, true);
	contacts_filter_destroy(filter);
	contacts_query_destroy(query);

	return addressbookId;
}

char* ctCommonGetAddressBookName(int addressbookId)
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	char* val = NULL;
	char* name = NULL;

	err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &record);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_record() Failed(%d)", err);
	err = contacts_record_get_str_p(record, _contacts_address_book.name, &val);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

	name = SAFE_STRDUP(val);
	err = contacts_record_destroy(record, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);

	return name;
}

Eina_List* ctCommonGetChekedDuplicatedNumberList(Eina_List *sourceList)
{
	WENTER();
	bool bAlready = false;
	void *info = NULL;
	Eina_List *list = NULL;
	Eina_List *checkedDuplicatedList = NULL;

	if(NULL == sourceList)
		return NULL;

	EINA_LIST_FOREACH(sourceList, list, info) {
		contacts_record_h recordClone = (contacts_record_h)info;
		void *checked_info = NULL;
		Eina_List *checked_list = NULL;
		char *value = NULL;
		int err = CONTACTS_ERROR_NONE;
		err = contacts_record_get_str_p(recordClone, _contacts_number.number, &value);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		EINA_LIST_FOREACH(checkedDuplicatedList, checked_list, checked_info) {
			contacts_record_h checked_recordClone = (contacts_record_h)checked_info;
			char *checked_value = NULL;
			err = contacts_record_get_str_p(checked_recordClone, _contacts_number.number, &checked_value);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
			if (strcmp(value, checked_value) == 0) {
				bAlready = true;
				err = contacts_record_destroy(recordClone, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
				break;
			}
		}
		WDEBUG("bAlready %d %s", bAlready, value);
		if(false == bAlready)
			checkedDuplicatedList = eina_list_append(checkedDuplicatedList, info);
	}

	eina_list_free(sourceList);

	return checkedDuplicatedList;
}

Eina_List* ctCommonGetChekedDuplicatedEmailList(Eina_List *sourceList)
{
	WENTER();
	bool bAlready = false;
	void *info = NULL;
	Eina_List *list = NULL;
	Eina_List *checkedDuplicatedList = NULL;

	if(NULL == sourceList)
		return NULL;

	EINA_LIST_FOREACH(sourceList, list, info) {
		contacts_record_h recordClone = (contacts_record_h)info;
		void *checked_info = NULL;
		Eina_List *checked_list = NULL;
		char *value = NULL;
		int err = CONTACTS_ERROR_NONE;
		err = contacts_record_get_str_p(recordClone, _contacts_email.email, &value);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
		EINA_LIST_FOREACH(checkedDuplicatedList, checked_list, checked_info) {
			contacts_record_h checked_recordClone = (contacts_record_h)checked_info;
			char *checked_value = NULL;
			err = contacts_record_get_str_p(checked_recordClone, _contacts_email.email, &checked_value);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
			if (strcmp(value, checked_value) == 0) {
				bAlready = true;
				err = contacts_record_destroy(recordClone, true);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
				break;
			}
		}
		WDEBUG("bAlready %d %s", bAlready, value);
		if(false == bAlready)
			checkedDuplicatedList = eina_list_append(checkedDuplicatedList, info);
	}

	eina_list_free(sourceList);

	return checkedDuplicatedList;
}

static bool __isUsableVcardChar(char c)
{
	switch (c) {
	case '\\':
	case '/':
	case ':':
	case '*':
	case '?':
	case '\"':
	case '<':
	case '>':
	case '|':
	case ';':
	case ' ':
		return false;
	default:
		return true;
	}
}

static bool check_blank_string(const char *display)
{
	for (size_t i = 0; display[i]; ++i) {
		if (display[i] != ' ') {
			return false;
		}
	}

	return true;
}

struct StorageData
{
	storage_type_e m_Type;
	int m_StorageId;
};

static bool __storageCb(int storageId, storage_type_e type, storage_state_e state, const char *path, void *userData)
{
	WHIT();
	WPRET_VM(!userData, false, "Empty user data");
	StorageData *storageData = static_cast<StorageData *>(userData);

	if (type == storageData->m_Type && STORAGE_STATE_MOUNTED == state) {
		storageData->m_StorageId = storageId;
		return false;
	}
	return true;
}

static int __getStorageId(storage_type_e storageType)
{
	WHIT();
	StorageData storageData {storageType, -1};
	int error = storage_foreach_device_supported(__storageCb, &storageData);
	WPRET_VM(error != STORAGE_ERROR_NONE, -1, "storage_foreach_device_supported() failed");

	return storageData.m_StorageId;
}

std::string getRootDirectoryPath(storage_type_e storageType)
{
	WHIT();
	char *dirPath = nullptr;

	int error = storage_get_root_directory(__getStorageId(storageType), &dirPath);
	if(STORAGE_ERROR_NONE == error && dirPath) {
		std::string res = dirPath;
		free(dirPath);
		return res;
	} else {
		WERROR("storage_foreach_device_supported() failed");
	}

	return std::string();
}

std::string getDirectoryPath(storage_type_e storageType, storage_directory_e dirType)
{
	WHIT();
	char *dirPath = nullptr;

	int error = storage_get_directory(__getStorageId(storageType), dirType, &dirPath);
	if(STORAGE_ERROR_NONE == error && dirPath) {
		std::string res = dirPath;
		free(dirPath);
		return res;
	} else {
		WERROR("storage_foreach_device_supported() failed");
	}

	return std::string();
}

static std::string __makeVcardName(const char* displayName)
{
	WHIT();
	std::string filePathStr = getDirectoryPath(STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_OTHERS);
	WPRET_VM(filePathStr.empty(), std::string(), "directory path is empty");

	struct stat buf;
	std::string  firstFilePath;
	filePathStr += '/';

	size_t nameLength = strlen(displayName);

	if (false == check_blank_string(displayName)) {
		size_t dirPathLength = filePathStr.length();

		for (size_t i = 0; i < nameLength; ++i) {
			if ((filePathStr.length() - dirPathLength + 1) > (NAME_MAX - VCARD_NAME_SUFFIX_LEN)) {
				WDEBUG("File name is too long. It will be cut.");
				break;
			}
			if (__isUsableVcardChar(displayName[i])) {
				filePathStr += displayName[i];
			} else if (iscntrl(displayName[i])) {
				filePathStr += ' ';
			}
		}
	} else {
		const char *unknown = V_("IDS_LOGS_MBODY_UNKNOWN");

		filePathStr += unknown;
	}

	firstFilePath = filePathStr;
	firstFilePath += ".vcf";

	if (stat(firstFilePath.c_str(), &buf) != 0) {
		filePathStr = firstFilePath;
	} else {
		int i = 1;

		do {
			char filePath[BUFSIZ] = { '\0' };
			snprintf(filePath, BUFSIZ - 1, "%s_%d.vcf", filePathStr.c_str(), i++);
			if(stat(filePath, &buf) != 0) {
				filePathStr = filePath;
				break;
			}
		} while (true);
	}

	return filePathStr;
}

std::string createVcardFile(contacts_record_h record)
{
	WHIT();
	const char *uri = NULL;
	char *displayName = NULL;
	char *vcard = NULL;
	int fd = -1;
	std::string vcardPath;

	contacts_record_get_uri_p(record, &uri);
	if (0 == strcmp(_contacts_person._uri, uri)) {
		contacts_record_get_str_p(record, _contacts_person.display_name, &displayName);
		contacts_vcard_make_from_person(record, &vcard);
	} else if (0 == strcmp(_contacts_contact._uri, uri)) {
		contacts_record_get_str_p(record, _contacts_contact.display_name, &displayName);
		contacts_vcard_make_from_contact(record, &vcard);
	} else if (0 == strcmp(_contacts_my_profile._uri, uri)) {
		contacts_record_get_str_p(record, _contacts_my_profile.display_name, &displayName);
		contacts_vcard_make_from_my_profile(record, &vcard);
	}

	bool failed_occured = false;
	do {

		if (!vcard) {
			WERROR("vcard is NULL");
			failed_occured = true;
			break;
		}

		if (!displayName) {
			WERROR("displayName is NULL");
			failed_occured = true;
			break;
		}

		vcardPath = __makeVcardName(displayName);
		if (vcardPath.empty()) {
			WERROR("__makeVcardName() failed");
			failed_occured = true;
			break;
		}

		fd = open(vcardPath.c_str(), O_WRONLY|O_CREAT|O_TRUNC, FILE_ACCESS_MODE);
		if (fd < 0) {
			WERROR("open(%s) Failed(%s)", vcardPath.c_str(), strerror(errno));
			failed_occured = true;
			break;
		}

		int ret = write(fd, vcard, strlen(vcard));
		if (ret == -1) {
			WERROR("write() Failed(%s)", strerror(errno));
		}

		close(fd);
	} while (false);

	free(vcard);

	if(!failed_occured) {
		return vcardPath;
	}

	return "";
}

void sharePerson(int personId, bool isMyProfile)
{
	char buf[CT_TEXT_SHORT_LEN] = { 0, };
	snprintf(buf, sizeof(buf), "%d", personId);

	int err = launchShareContact(buf, isMyProfile);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "launchShareContact() failed(%d)", err);
}
