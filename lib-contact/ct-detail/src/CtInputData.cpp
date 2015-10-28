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

#include <vector>
#include <app_preference.h>

#include "ContactsDebug.h"
#include "CtInputData.h"
#include "CtCommon.h"
#include "CtString.h"

CtInputData::CtInputData(contacts_record_h contactRecord)
{
	WHIT();
	__contactRecord = contactRecord;
	__contactId = 0;
	__baseAddressbookId = 0;
	__baseAddressbookName = NULL;
	__userName = NULL;
	__personId = 0;
	__favorite = false;
	__init();
}

CtInputData::CtInputData(int personId, const char *number, const char *email, const char *url)
{
	WHIT();
	__contactRecord = NULL;
	__contactId = 0;
	__baseAddressbookId = 0;
	__baseAddressbookName = NULL;
	__userName = NULL;
	__contactRecord = __getInitContact(personId, number, email, url);
	__personId = 0;
	__favorite = false;
	__init();
}

CtInputData::CtInputData(const char *name, const char *number, const char *email, const char *url)
{
	WHIT();
	__contactRecord = NULL;
	__contactId = 0;
	__baseAddressbookId = 0;
	__baseAddressbookName = NULL;
	__userName = NULL;
	__contactRecord = NULL;
	__personId = 0;
	__favorite = false;
	__init();

	if (name) {
		setFullName(name);
	}
	if (number) {
		addNumber(number);
	}
	if (email) {
		addEmail(email);
	}
	if (url) {
		addWebsite(url);
	}
}

CtInputData::CtInputData()
{
	WHIT();
	__init();
}

CtInputData::~CtInputData()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	free(__baseAddressbookName);
	free(__userName);
	err = contacts_db_remove_changed_cb(_contacts_contact._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);

	err = contacts_record_destroy(__contactRecord, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
}

void CtInputData::__init()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
//	contacts_record_h addressbookRecord;
	__currentVersion = 0;
	__listener = NULL;
	__inputAddressbook = false;
	__favorite = false;

	if (__contactRecord == NULL) {
		if(__contactId > 0) {
			WDEBUG("get contact %d", __contactId);
//			contacts_record_h personRecord = NULL;
			int err = CONTACTS_ERROR_NONE;
			err = contacts_db_get_record(_contacts_contact._uri, __contactId, &__contactRecord);
			WPRET_M(CONTACTS_ERROR_NONE != err,  "contacts_db_get_record() Failed(%d)", err);

		}
		else {
			WDEBUG("create contact");
			err = contacts_record_create(_contacts_contact._uri, &__contactRecord);
			WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d))", err);

			__baseAddressbookId = __getBaseAddressbookId();
		}
	} else {
		err = contacts_record_get_int(__contactRecord, _contacts_contact.id, &__contactId);
		WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		WDEBUG("get contact id %d", __contactId);
		if(__contactId > 0) {
			err = contacts_record_get_int(__contactRecord, _contacts_contact.address_book_id, &(__baseAddressbookId));
			WPRET_M(CONTACTS_ERROR_NONE != err,  "contacts_record_get_int() Failed(%d)", err);
		}
		else {
			__baseAddressbookId = __getBaseAddressbookId();
		}

		err = contacts_record_get_int(__contactRecord, _contacts_contact.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_bool(__contactRecord, _contacts_contact.is_favorite, &__favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str is_favorite %d", err);
	}

	err  = contacts_db_get_current_version(&__currentVersion);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_current_version() Failed(%d)", err);

	err = contacts_db_add_changed_cb(_contacts_contact._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() Failed(%d)", err);

	setAddressbookInfo(__baseAddressbookId);
}

void CtInputData::__onPersonChanged(const char *viewUri, void *userData)
{
	WHIT();
	WPRET_M(NULL == userData, "userData is NULL");
	CtInputData* data = (CtInputData*)userData;

	WPRET_M(0 == data->__contactId, "no need to update");

	int err = CONTACTS_ERROR_NONE;
	int version = 0;
	bool matched = false;
	contacts_list_h chagnedList = NULL;
	contacts_record_h changedRecord = NULL;

	err = contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, data->__baseAddressbookId, data->__currentVersion, &chagnedList, &version);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_changes_by_version() Failed(%d)", err);

	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(chagnedList, &changedRecord)) {
		int changedId = 0;
		err = contacts_record_get_int(changedRecord, _contacts_contact_updated_info.contact_id, &changedId);
		if(data->__contactId == changedId) {
			WDEBUG("update matched");
			matched = true;
			break;
		}
		contacts_list_next(chagnedList);
	}

	err = contacts_list_destroy(chagnedList, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy(out_change_list) Failed(%d)", err);

	if(version > 0)
		data->__currentVersion = version;

	if(matched && data->__listener)
		data->__listener->onDbChanged(viewUri);
}

bool CtInputData::checkChangedTypeIsDeleted()
{
	WENTER();
	contacts_list_h list = NULL;
	int contactId = 0;
	int currentVersion = 0;

	int type = -1;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h updatedRecord = NULL;

	err = contacts_record_get_int(__contactRecord, _contacts_contact.address_book_id, &__baseAddressbookId);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

	err = contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, __baseAddressbookId, __currentVersion, &list, &currentVersion);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_changes_by_version() Failed(%d)", err);

	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &updatedRecord)) {
		err = contacts_record_get_int(updatedRecord, _contacts_contact_updated_info.contact_id, &contactId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_next() Failed(%d)", err);
		if (CONTACTS_ERROR_NONE != err) {
			err = contacts_list_next(list);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_next() Failed(%d)", err);
			continue;
		}

		if (contactId == __contactId) {
			err = contacts_record_get_int(updatedRecord, _contacts_contact_updated_info.type, &type);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

			break;
		}
		contacts_list_next(list);
	}
	err = contacts_list_destroy(list, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destrot() Failed(%d)", err);

	if (currentVersion) {
		__currentVersion = currentVersion;
	}

	if (CONTACTS_CHANGE_DELETED == type) {
		return true;
	}
	else {
		return false;
	}
}

void CtInputData::setPreferenceCreateFirst(bool isFirst)
{
	WHIT();

	preference_set_boolean(CT_KEY_CONTACT_CREATE_FIRST, isFirst);
}

void CtInputData::__setContactInfo(int personId)
{
	WHIT();

	WDEBUG("personID %d", personId);

	int err = CONTACTS_ERROR_NONE;

	if(personId > 0) {
		__contactRecord = __getWritableContact(personId);
		err = contacts_record_get_int(__contactRecord, _contacts_contact.id, &__contactId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_int(__contactRecord, _contacts_contact.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_bool(__contactRecord, _contacts_contact.is_favorite, &__favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str is_favorite %d", err);
	}

	if (__contactRecord == NULL) {
		WDEBUG("create contact");
		err = contacts_record_create(_contacts_contact._uri, &__contactRecord);
		WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d))", err);

		__baseAddressbookId = __getBaseAddressbookId();
	}
}

contacts_record_h CtInputData::__getInitContact(int personId, const char *number, const char *email, const char *url)
{
	WHIT();

	__setContactInfo(personId);
	setAddressbookInfo(__baseAddressbookId);

	if (number) {
		addNumber(number);
	}
	if (email) {
		addEmail(email);
	}
	if (url) {
		addWebsite(url);
	}

	return __contactRecord;
}
/*
bool CtInputData::getMFC()
{
		int err = CONTACTS_ERROR_NONE;
		contacts_list_h personUsageList = NULL;
		contacts_query_h query= NULL;
		contacts_filter_h filter = NULL;


		err = contacts_query_create(_contacts_person_usage._uri, &query);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_create() Failed(%d)", err);
		err = contacts_filter_create(_contacts_person_usage._uri, &filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);
		do {
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_usage.times_used, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, 1))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_bool(filter, _contacts_person_usage.is_favorite, false))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter))) break;
		} while (0);

		err = contacts_query_set_sort(query, _contacts_person_usage.times_used, false);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_sort() Failed(%d)", err);

		err = contacts_db_get_records_with_query(query, 0, 5, &personUsageList);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_get_list() Failed(%d)", err);

		contacts_filter_destroy(filter);
		contacts_query_destroy(query);
		int mfcIndex = 0;
		contacts_record_h recordPerson = NULL;
		while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(personUsageList, &recordPerson)) {
			if(mfcIndex >5)
				break;
			int personId = 0;
			err = contacts_record_get_int(recordPerson, _contacts_person.id, &personId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
			if(__personId == personId){
				contacts_list_destroy(personUsageList, true);
				return true;
			}
			mfcIndex++;
			contacts_list_next(personUsageList);
		}
		contacts_list_destroy(personUsageList, true);
	return false;
}
*/
contacts_record_h CtInputData::__getWritableContact(int personId)
{
	WDEBUG("personId %d", personId);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h contactRecord = NULL;

	if(personId > 0) {
		Eina_List *contactList = ctCommonGetWritableContactList(personId);
		void *data = NULL;
		Eina_List *cursor = NULL;
		EINA_LIST_FOREACH(contactList, cursor, data) {
			contacts_record_h record = (contacts_record_h)data;
			err = contacts_record_clone(record, &contactRecord);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
			break;
		}

		ctCommonDestroyRecordList(contactList);
	}
	else {
		err = contacts_record_create(_contacts_contact._uri, &contactRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d)", err);
	}

	return contactRecord;
}


void CtInputData::setAddressbookInfo(int addressbookId)
{
	WDEBUG("addressbookId %d", addressbookId);

	__baseAddressbookId = addressbookId;

	contacts_list_h list = NULL;
	contacts_record_h record_address_book = NULL;
	int err = CONTACTS_ERROR_NONE;

	err = contacts_db_get_all_records(_contacts_address_book._uri, 0, 0, &list);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);
	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &record_address_book)) {
		int addressbook_id;
		char *addressbook_name = NULL;
		char *addressbook_username = NULL;

		err = contacts_record_get_int(record_address_book, _contacts_address_book.id, &addressbook_id);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		ctCommonGetProviderNameByAddressbookId(record_address_book, addressbook_id, &addressbook_name, &addressbook_username);

		if(addressbook_id == __baseAddressbookId) {
			FREEandSTRDUP(__baseAddressbookName, addressbook_name);
			FREEandSTRDUP(__userName, addressbook_username);
			WDEBUG("addressbook_id %d is %s", addressbook_id, addressbook_name);
			SAFE_FREE(addressbook_name);
			SAFE_FREE(addressbook_username);
			err = contacts_list_destroy(list, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);
			return;
		}
		SAFE_FREE(addressbook_name);
		SAFE_FREE(addressbook_username);
		contacts_list_next(list);
	}
	err = contacts_list_destroy(list, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);
	__baseAddressbookName = NULL;
	__userName = NULL;
	return;
}

bool CtInputData::__hasSuffix(const char *src)
{
	int index = strlen(src) - 1;
	bool delim = false;
	for (int i = index; i >= 0; i--) {
		if (src[i] == ' ' || src[i] == '.') {
			delim = true;
		} else if (src[i] == ',') {
			return true;
		} else if (delim) {
			return false;
		}
	}
	return false;
}

int CtInputData::separateFullName(contacts_record_h nameRecord, const char *full_name)
{
	WHIT();
	WPRET_VM(NULL == nameRecord, -1, "nameRecord is NULL");
	WPRET_VM(NULL == full_name, -1, "full_name is NULL");
	int err = CONTACTS_ERROR_NONE;
	char *tmp_name = NULL;
	char *tmp_str = NULL;
	char tmp[CT_TEXT_MAX_LEN] = {0};
	std::string tmp_first;
	snprintf(tmp, sizeof(tmp), "%s", full_name);
	std::vector<char*> fields;

	tmp_str = strtok_r(tmp, ",. ", &tmp_name);
	while(tmp_str && *tmp_str) {
		fields.push_back(tmp_str);
		tmp_str = strtok_r(NULL, ",. ", &tmp_name);
	}

	if ((fields.size() > 2) && __hasSuffix(full_name)) {
		err = contacts_record_set_str(nameRecord, _contacts_name.suffix, fields.back());
		fields.pop_back();
	} else {
		err = contacts_record_set_str(nameRecord, _contacts_name.suffix, NULL);
	}
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	if (fields.size() > 1) {
		err = contacts_record_set_str(nameRecord, _contacts_name.last, fields.back());
		fields.pop_back();
	} else {
		err = contacts_record_set_str(nameRecord, _contacts_name.last, NULL);
	}
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	if (fields.size() > 1) {
		err = contacts_record_set_str(nameRecord, _contacts_name.addition, fields.back());
		fields.pop_back();
	} else {
		err = contacts_record_set_str(nameRecord, _contacts_name.addition, NULL);
	}
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	if (fields.size() < 1) {
		err = contacts_record_set_str(nameRecord, _contacts_name.prefix, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.first, NULL);
	} else {
		for (auto &str : fields) {
			tmp_first.append(str);
			tmp_first.append(" ");
		}
		tmp_first.pop_back();
		err = contacts_record_set_str(nameRecord, _contacts_name.first, tmp_first.c_str());
	}
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::separatePhoneticFullName(contacts_record_h nameRecord, const char *full_name)
{
	WHIT();
	WPRET_VM(NULL == nameRecord, -1, "nameRecord is NULL");
	int err = CONTACTS_ERROR_NONE;
	char *temp_name = NULL;
	char *temp_first = NULL;
	char *temp_middle = NULL;
	char *temp_last = NULL;
	char tmp[CT_TEXT_MAX_LEN] = {0};

	if (NULL == full_name) {
		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
		return err;
	}

	snprintf(tmp, sizeof(tmp), "%s", full_name);

	temp_first = strtok_r(tmp, " ", &temp_name);
	if (temp_first) {
		temp_middle = strtok_r(NULL, " ", &temp_name);
		if (temp_middle) {
			temp_last = temp_name;
		}
	}

	WDEBUG("full_name %s, first %s, middle %s, last %s", full_name, temp_first, temp_middle, temp_last);

	if(temp_first && *temp_first) {
		if(temp_last && *temp_last) {
			err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, temp_first);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

			err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, temp_middle);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

			err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, temp_last);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
		}
		else {
			if(temp_middle && *temp_middle) {
				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, temp_first);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, temp_middle);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
			}
			else {
				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, temp_first);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
			}
		}
	}else {
		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::setPrefixName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.prefix, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setSuffixName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.suffix, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setPhoneticFullName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if(nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = separatePhoneticFullName(nameRecord, text);
	return err;
}

int CtInputData::setPhoneticFirstName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_first, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setPhoneticMiddleName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_middle, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}
int CtInputData::setPhoneticLastName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.phonetic_last, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setNickname(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.nickname, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_nickname._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.nickname, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_nickname.name, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setCompany(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.company, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_company._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.company, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_company.name, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setJobTitle(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.company, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_company._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.company, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_company.job_title, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setFullName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	//WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	if(nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = separateFullName(nameRecord, text);
	return err;
}

int CtInputData::setFirstName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.first, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setMiddleName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.addition, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

int CtInputData::setLastName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_contact.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = contacts_record_set_str(nameRecord, _contacts_name.last, text);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

	return err;
}

const char* CtInputData::getPrefixName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *prefix = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.prefix, &prefix);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(prefix);

	return prefix;
}

const char* CtInputData::getSuffixName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *suffix = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.suffix, &suffix);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(suffix);

	return suffix;
}

const char* CtInputData::getPhoneticFirstName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_first, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(phoneticName);

	return phoneticName;
}

const char* CtInputData::getPhoneticMiddleName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_middle, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(phoneticName);

	return phoneticName;
}

const char* CtInputData::getPhoneticLastName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_last, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(phoneticName);

	return phoneticName;
}

const char* CtInputData::getNickname()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *nickname = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.nickname, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_nickname.name, &nickname);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return nickname;
}

const char* CtInputData::getCompany()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *company = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.company, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_company.name, &company);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return company;
}

const char* CtInputData::getJobTitle()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *jobTitle = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.company, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_company.job_title, &jobTitle);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return jobTitle;
}

/**
  * @return must be freed by you
  */
void CtInputData::getFullName(std::string &Name)
{
	int strLen = 0;
	const char *firstName = NULL;
	const char *middleName = NULL;
	const char *lastName = NULL;
	const char *prefixName = NULL;
	const char *suffixName = NULL;
	char *fullName = NULL;

	firstName = getFirstName();
	middleName = getMiddleName();
	lastName = getLastName();
	prefixName = getPrefixName();
	suffixName = getSuffixName();

	strLen = SAFE_STRLEN(prefixName) + SAFE_STRLEN(firstName) + SAFE_STRLEN(middleName) + SAFE_STRLEN(lastName) + SAFE_STRLEN(suffixName) + 6;
	WDEBUG("strLen = %d", strLen);

	fullName = (char*)malloc(sizeof(char)*strLen);
	WPRET_M(NULL == fullName, "failed to malloc");
	memset(fullName, 0, sizeof(char)*strLen);

	if (prefixName && *prefixName) {
		strncat(fullName, prefixName, SAFE_STRLEN(prefixName));
	}
	if (firstName && *firstName) {
		if (fullName && *fullName) {
			strncat(fullName, " ", 1);
		}
		strncat(fullName, firstName, SAFE_STRLEN(firstName));
	}
	if (middleName && *middleName) {
		if (fullName && *fullName) {
			strncat(fullName, " ", 1);
		}
		strncat(fullName, middleName, SAFE_STRLEN(middleName));
	}
	if (lastName && *lastName) {
		if (fullName && *fullName) {
			strncat(fullName, " ", 1);
		}
		strncat(fullName, lastName, SAFE_STRLEN(lastName));
	}
	if (suffixName && *suffixName) {
		if (fullName && *fullName) {
			strncat(fullName, ", ", 2);
		}
		strncat(fullName, suffixName, SAFE_STRLEN(suffixName));
	}

	Name = fullName;
	free(fullName);
}

const char* CtInputData::getFirstName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *firstName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.first, &firstName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(firstName);

	result = firstName;
	return result;
}

const char* CtInputData::getMiddleName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *middleName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.addition, &middleName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(middleName);

	result = middleName;
	return result;
}

const char* CtInputData::getLastName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *lastName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.last, &lastName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	ctCommonRemoveNewline(lastName);

	result = lastName;
	return result;
}

int CtInputData::removeImage(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.image, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	err = contacts_record_destroy(record, true);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_destroy() Failed(%d)", err);

	return err;
}

const char* CtInputData::getImagePath()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h imageRecord = NULL;
	char *image = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.image, 0, &imageRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(imageRecord, _contacts_image.path, &image);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	result = image;
	return result;
}

contacts_record_h CtInputData::getImageRecord()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.image, 0, &record);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_child_record_at_p() Failed(%d)", err);
	if (NULL != record) {
		return record;
	}

	return record;
}

int CtInputData::setImage(contacts_record_h record, char *image, int type)
{
	WDEBUG("image %s", image);
	int err = CONTACTS_ERROR_NONE;

	if(image && *image) {
		if(record == NULL) {
			err = contacts_record_create(_contacts_image._uri, &record);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

			err = contacts_record_add_child_record(__contactRecord, _contacts_contact.image, record);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_add_child_record() Failed(%d)", err);
		}
		err = contacts_record_set_int(record, _contacts_image.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
		err = contacts_record_set_str(record, _contacts_image.path, image);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

Eina_List* CtInputData::getNumberList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.number);
}

contacts_record_h CtInputData::addNumber(const char *number)
{
	WDEBUG("number %s", number);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_number._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setNumberNumber(record, number);
	setNumberType(record, getNextNumberType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.number, record);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setNumberNumber(contacts_record_h record, const char *number)
{
	WDEBUG("number %s", number);
	int err = CONTACTS_ERROR_NONE;

	if(number) {
		err = contacts_record_set_str(record, _contacts_number.number, number);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::setNumberType(contacts_record_h record, int type, const char *customStr)
{
	WDEBUG("type %d", type);
	WDEBUG("customStr %s", customStr);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_number.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}
	if (customStr) {
		err = contacts_record_set_str(record, _contacts_number.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}
	return err;
}

int CtInputData::removeNumber(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.number, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

const char* CtInputData::getNumberNumber(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *number = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_number.number, &number);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = number;
	return result;
}

contacts_record_h CtInputData::getEmergencyNumber()
{
	WHIT();
	/*
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h numberRecord = NULL;
	contacts_record_h emergencyRecord = NULL;
	contacts_query_h query = NULL;
	contacts_filter_h filter = NULL;
	contacts_list_h emergencyList = NULL;
	int personId = 0;
	int numberId = 0;
	int index = 0;

	err = contacts_record_get_int(__contactRecord, _contacts_contact.person_id, &personId);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

	err = contacts_filter_create(_contacts_emergency._uri, &filter);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create() Failed(%d)", err);

	do {
		if (CONTACTS_ERROR_NONE != (err = contacts_filter_add_int(filter, _contacts_emergency.person_id, CONTACTS_MATCH_EQUAL, personId))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_query_create(_contacts_emergency._uri, &query))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_query_set_filter(query, filter))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_db_get_records_with_query(query, 0, 0, &emergencyList))) break;
	} while (0);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_get_list() Failed(%d)", err);

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);
	if (emergencyList && CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(emergencyList, &emergencyRecord)) {
		err = contacts_record_get_int(emergencyRecord,_contacts_emergency.number_data_id, &numberId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
	}
	contacts_list_destroy(emergencyList, true);

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.number, index++, &numberRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}
		int id = 0;
		err = contacts_record_get_int(numberRecord, _contacts_number.id, &id);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		if (id == numberId) {
			break;
		}
	}

	return numberRecord;
	*/
	return NULL;
}

contacts_record_h CtInputData::getDefaultNumber()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.number, 0, &record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_get_child_record_at_p() Failed(%d)", err);
	}
	return record;
}

int CtInputData::getNumberType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_number.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

Eina_List* CtInputData::getEmailList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.email);
}

const char* CtInputData::getDataTypeCustom(contacts_record_h record, int property_id)
{
	char* typeStr = NULL;
	const char* result = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, property_id, &typeStr);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	result = typeStr;
	return result;
}

void CtInputData::getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString)
{
	WHIT();
	const char* typeStr = NULL;
	char buf[128] = {0,};

	if (numberType & CONTACTS_NUMBER_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_number.label);

	}
	else if (numberType & CONTACTS_NUMBER_TYPE_CELL) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME) {
			snprintf(buf, sizeof(buf),"%s 2", V_("IDS_PB_OPT_HOME_ABB"));
			typeStr = buf;
		}
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK) {
			snprintf(buf, sizeof(buf),"%s 2", V_("IDS_PB_OPT_WORK"));
			typeStr = buf;
		}
		else {
			typeStr = V_("IDS_PB_OPT_MOBILE");
		}
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_VOICE) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME) {
			typeStr = V_("IDS_PB_OPT_HOME_ABB");
		}
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK) {
			typeStr = V_("IDS_PB_OPT_WORK");
		}
		else {
			typeStr = V_("IDS_PB_OPT_OTHER");
		}
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_FAX) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME) {
			typeStr = V_("IDS_PB_OPT_FAX_HHOME_ABB");
		}
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK) {
			typeStr = V_("IDS_PB_OPT_FAX_HWORK_ABB");
		}
		else {
			typeStr = V_("IDS_PB_BODY_FAX");
		}
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_MAIN) {
		typeStr = V_("IDS_PB_OPT_MAIN");
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_PAGER) {
		typeStr = V_("IDS_PB_OPT_PAGER");
	}
	else {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME) {
			typeStr = V_("IDS_PB_OPT_HOME_ABB");
		}
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK) {
			typeStr = V_("IDS_PB_OPT_WORK");
		}
		else {
			typeStr = V_("IDS_PB_OPT_OTHER");
		}
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNumberRetType(int indexType)
{
	int retType;

	switch (indexType) {
	case CTTEXT_TYPE_MOBILE:
		retType = CONTACTS_NUMBER_TYPE_CELL;
		break;
	case CTTEXT_TYPE_HOME:
		retType = CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_HOME;
		break;
	case CTTEXT_TYPE_WORK:
		retType = CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_WORK;
		break;
	case CTTEXT_TYPE_FAX_HOME:
		retType = CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_HOME;
		break;
	case CTTEXT_TYPE_FAX_WORK:
		retType = CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_WORK;
		break;
	case CTTEXT_TYPE_PAGER:
		retType = CONTACTS_NUMBER_TYPE_PAGER;
		break;
	case CTTEXT_TYPE_CUSTOM:
		retType = CONTACTS_NUMBER_TYPE_CUSTOM;
		break;
	case CTTEXT_TYPE_MAIN:
		retType = CONTACTS_NUMBER_TYPE_MAIN;
		break;
	case CTTEXT_TYPE_OTHER:
	default:
		retType = CONTACTS_NUMBER_TYPE_OTHER;
		break;
	}

	return retType;
}
int CtInputData::getNumberIndexType(int numberType)
{
	if (numberType & CONTACTS_NUMBER_TYPE_CUSTOM) {
		return (CTTEXT_TYPE_OTHER);
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_CELL) {
			return (CTTEXT_TYPE_MOBILE);
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_VOICE) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME) {
			return (CTTEXT_TYPE_HOME);
		}
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK) {
			return (CTTEXT_TYPE_WORK);
		}
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_FAX) {
		if (numberType & CONTACTS_NUMBER_TYPE_HOME)
			return (CTTEXT_TYPE_FAX_HOME);
		else if (numberType & CONTACTS_NUMBER_TYPE_WORK)
			return (CTTEXT_TYPE_FAX_WORK);
	}
	else if (numberType & CONTACTS_NUMBER_TYPE_PAGER)
		return (CTTEXT_TYPE_PAGER);
	else if (numberType & CONTACTS_NUMBER_TYPE_MAIN)
		return (CTTEXT_TYPE_MAIN);
	else {
		if (numberType == CONTACTS_NUMBER_TYPE_HOME)
			return (CTTEXT_TYPE_HOME);
		else if (numberType == CONTACTS_NUMBER_TYPE_WORK)
			return (CTTEXT_TYPE_WORK);
		else
			return (CTTEXT_TYPE_OTHER);
	}

	return CTTEXT_TYPE_MOBILE;
}

int CtInputData::getNextNumberType()
{
	int i;
	int numberType;
	int indexType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	int used[CTTEXT_TYPE_OTHER+1] = {0};
	contacts_record_h record = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.number, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_number.type, &numberType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		indexType = getNumberIndexType(numberType);
		used[indexType] = 1;
	}

	for (i=CTTEXT_TYPE_MOBILE;i<CTTEXT_TYPE_OTHER;i++)  {
		if (0 == used[i]) {

			return getNumberRetType(i);
		}
	}

	return CONTACTS_NUMBER_TYPE_OTHER;

}

contacts_record_h CtInputData::addEmail(const char *email)
{
	WDEBUG("email %s", email);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_email._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setEmailEmail(record, email);
	setEmailType(record, getNextEmailType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.email, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setEmailEmail(contacts_record_h record, const char *email)
{
	WDEBUG("email %s", email);
	int err = CONTACTS_ERROR_NONE;

	if (email) {
		err = contacts_record_set_str(record, _contacts_email.email, email);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::setEmailType(contacts_record_h record, int type, char *customStr)
{
	WDEBUG("type %d", type);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_email.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}
	if (customStr) {
		err = contacts_record_set_str(record, _contacts_email.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

const char* CtInputData::getEmailEmail(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *email = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_email.email, &email);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = email;
	return result;
}

contacts_record_h CtInputData::getDefaultEmail()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.email, 0, &record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_get_child_record_at_p() Failed(%d)", err);
	}
	return record;
}

int CtInputData::getEmailType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_email.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtInputData::getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString)
{
	WHIT();

	const char* typeStr = NULL;

	if (indexType & CONTACTS_EMAIL_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_email.label);
	}
	else if (indexType & CONTACTS_EMAIL_TYPE_HOME) {
		typeStr = V_("IDS_PB_OPT_HOME_ABB");
	}
	else if (indexType & CONTACTS_EMAIL_TYPE_WORK) {
		typeStr = V_("IDS_PB_OPT_WORK");
	}
	else {
		typeStr = V_("IDS_PB_OPT_OTHER");
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNextEmailType()
{
	int i;
	int used = 0;
	int emailType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.email, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_email.type, &emailType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		used |= emailType;
	}

	int maxType = CONTACTS_EMAIL_TYPE_WORK;
	for (i=1;(1<<i)<=maxType;i++) {
		if (!(used & (1<<i))) {
			return 1<<i;
		}
	}

	return CONTACTS_EMAIL_TYPE_OTHER;

}

int CtInputData::removeEmail(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.email, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

contacts_record_h CtInputData::getNoteRecord()
{
	WHIT();
	contacts_record_h record = NULL;
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.note, 0, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);
	return record;
}

Eina_List* CtInputData::getNoteList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.note);
}

contacts_record_h CtInputData::addNote(const char *note)
{
	WDEBUG("note %s", note);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_note._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setNote(record, note);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.note, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setNote(contacts_record_h record, const char *note)
{
	WDEBUG("note %s", note);
	int err = CONTACTS_ERROR_NONE;

	if (note) {
		err = contacts_record_set_str(record, _contacts_note.note, note);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

const char* CtInputData::getNote(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *note = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_note.note, &note);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = note;
	return result;
}

int CtInputData::removeNote(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.note, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtInputData::getAddressList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.address);
}

contacts_record_h CtInputData::addAddress(const char *address)
{
	WDEBUG("address %s", address);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_address._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setAddressAddress(record, address);
	setAddressType(record, getNextAddressType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.address, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setAddressAddress(contacts_record_h record, const char *address)
{
	WDEBUG("address %s", address);
	int ret = CONTACTS_ERROR_NONE;

	if (address) {
		do {
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.postbox, NULL))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.postal_code, NULL))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.region, NULL))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.locality, NULL))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.street, address))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.country, NULL))) break;
			if (CONTACTS_ERROR_NONE != (ret = contacts_record_set_str(record, _contacts_address.extended, NULL))) break;
		} while (0);
		WPRET_VM(CONTACTS_ERROR_NONE != ret, ret, "contacts_record_set_str() Failed(%d)", ret);
	}

	return ret;
}

int CtInputData::setAddressType(contacts_record_h record, int type, const char *customStr)
{
	WDEBUG("type %d", type);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_address.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	if (customStr) {
		err = contacts_record_set_str(record, _contacts_address.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::getAddressAddress(contacts_record_h record, char *buf, int length)
{
	WHIT();
	return __getFullAddress(record, buf, length);
}

void CtInputData::__appendStringWithLinebreak(const char *src, char *dest, int length)
{
	char temp[CT_TEXT_MAX_LEN];
	if (src && '\0' != src[0]) {
		if ('\0' == dest[0])
			snprintf(dest, length, "%s", src);
		else {
			snprintf(temp, sizeof(temp), "%s\n%s", src, dest);
			snprintf(dest, length, "%s", temp);
		}
	}
}

int CtInputData::__getFullAddress(contacts_record_h addresssRecord, char *buf, int length)
{
	WHIT();

	const char *code = NULL;
	const char *region = NULL;
	const char *locality = NULL;
	const char *street = NULL;
	const char *extended = NULL;
	const char *country = NULL;
	const char *pobox = NULL;
	int err = CONTACTS_ERROR_NONE;

	do {
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.postal_code , (char**)&code))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.extended , (char**)&extended))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.postbox , (char**)&pobox))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.street , (char**)&street))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.locality , (char**)&locality))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.region , (char**)&region))) break;
		if (CONTACTS_ERROR_NONE != (err = contacts_record_get_str_p(addresssRecord, _contacts_address.country ,(char**)&country))) break;
	} while (0);
	if(CONTACTS_ERROR_NONE != err)
	WERROR("contacts_record_get_str_p() Failed(%d)", err);

	__appendStringWithLinebreak(country, buf, length);
	__appendStringWithLinebreak(code, buf, length);
	__appendStringWithLinebreak(region, buf, length);
	__appendStringWithLinebreak(locality, buf, length);
	__appendStringWithLinebreak(street, buf, length);
	__appendStringWithLinebreak(extended, buf, length);
	__appendStringWithLinebreak(pobox, buf, length);

	return 0;
}

int CtInputData::getAddressType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_address.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtInputData::getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString)
{
	WHIT();
	const char* typeStr = NULL;

	if (indexType & CONTACTS_ADDRESS_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_address.label);
	}
	else if (indexType & CONTACTS_ADDRESS_TYPE_HOME) {
		typeStr = V_("IDS_PB_OPT_HOME_ABB");
	}
	else if (indexType & CONTACTS_ADDRESS_TYPE_WORK) {
		typeStr = V_("IDS_PB_OPT_WORK");
	}
	else {
		typeStr = V_("IDS_PB_OPT_OTHER");
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNextAddressType()
{
	int i;
	int used = 0;
	int addressType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.address, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_address.type, &addressType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		used |= addressType;
	}

	int maxType = CONTACTS_ADDRESS_TYPE_WORK;
	for (i=1;(1<<i)<=maxType;i++) {
		if (!(used & (1<<i))) {
			return 1<<i;
		}
	}

	return CONTACTS_ADDRESS_TYPE_OTHER;

}

int CtInputData::removeAddress(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.address, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtInputData::getMessengerList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.messenger);
}

contacts_record_h CtInputData::addMessenger(const char *messenger)
{
	WDEBUG("messenger %s", messenger);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_messenger._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setMessengerMessenger(record, messenger);
	setMessengerType(record, getNextMessengerType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.messenger, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setMessengerMessenger(contacts_record_h record, const char *messenger)
{
	WDEBUG("messenger %s", messenger);
	int err = CONTACTS_ERROR_NONE;

	if (messenger) {
		err = contacts_record_set_str(record, _contacts_messenger.im_id, messenger);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::setMessengerType(contacts_record_h record, int type, const char *customStr)
{
	WDEBUG("type %d", type);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_messenger.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}
	if (customStr) {
		err = contacts_record_set_str(record, _contacts_messenger.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::getMessengerIndexType(int emailType)
{
	switch (emailType) {
	case CONTACTS_MESSENGER_TYPE_CUSTOM:
		return CTTEXT_MESSENGER_TYPE_CUSTOM;
	case CONTACTS_MESSENGER_TYPE_AIM:
		return CTTEXT_MESSENGER_TYPE_AIM;
	case CONTACTS_MESSENGER_TYPE_YAHOO:
		return CTTEXT_MESSENGER_TYPE_YAHOO;
	case CONTACTS_MESSENGER_TYPE_SKYPE:
		return CTTEXT_MESSENGER_TYPE_SKYPE;
	case CONTACTS_MESSENGER_TYPE_QQ:
		return CTTEXT_MESSENGER_TYPE_QQ;
	case CONTACTS_MESSENGER_TYPE_GOOGLE:
		return CTTEXT_MESSENGER_TYPE_HANGOUTS;
	case CONTACTS_MESSENGER_TYPE_ICQ:
		return CTTEXT_MESSENGER_TYPE_ICQ;
	case CONTACTS_MESSENGER_TYPE_JABBER:
	default:
		return CTTEXT_MESSENGER_TYPE_JABBER;
	}
}

int CtInputData::getMessengerRetType(int indexType)
{
	int retType;

	switch (indexType) {
	case CTTEXT_MESSENGER_TYPE_CUSTOM:
		retType = CONTACTS_MESSENGER_TYPE_CUSTOM;
		break;
	case CTTEXT_MESSENGER_TYPE_AIM:
		retType = CONTACTS_MESSENGER_TYPE_AIM;
		break;
	case CTTEXT_MESSENGER_TYPE_YAHOO:
		retType = CONTACTS_MESSENGER_TYPE_YAHOO;
		break;
	case CTTEXT_MESSENGER_TYPE_SKYPE:
		retType = CONTACTS_MESSENGER_TYPE_SKYPE;
		break;
	case CTTEXT_MESSENGER_TYPE_QQ:
		retType = CONTACTS_MESSENGER_TYPE_QQ;
		break;
	case CTTEXT_MESSENGER_TYPE_HANGOUTS:
		retType = CONTACTS_MESSENGER_TYPE_GOOGLE;
		break;
	case CTTEXT_MESSENGER_TYPE_ICQ:
		retType = CONTACTS_MESSENGER_TYPE_ICQ;
		break;
	case CTTEXT_MESSENGER_TYPE_JABBER:
	default:
		retType = CONTACTS_MESSENGER_TYPE_JABBER;
		break;
	}

	return retType;
}

const char* CtInputData::getMessengerMessenger(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *messenger = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_messenger.im_id, &messenger);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = messenger;
	return result;
}

int CtInputData::getMessengerType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_messenger.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtInputData::getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString)
{
	WHIT();
	const char* typeStr = NULL;

	if (indexType == CONTACTS_MESSENGER_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_messenger.label);
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_AIM) {
		typeStr = V_("IDS_PB_OPT_AIM");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_YAHOO) {
		typeStr = V_("IDS_PB_OPT_YAHOO_E");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_SKYPE) {
		typeStr = V_("IDS_PB_BODY_SKYPE");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_QQ) {
		typeStr = V_("IDS_PB_OPT_QQ");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_GOOGLE) {
		typeStr = V_("IDS_PB_OPT_HANGOUTS");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_ICQ) {
		typeStr = V_("IDS_PB_OPT_ICQ");
	}
	else if (indexType == CONTACTS_MESSENGER_TYPE_JABBER) {
		typeStr = V_("IDS_PB_OPT_JABBER");
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNextMessengerType()
{
	int i;
	int messengerType;
	int indexType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	int used[CTTEXT_TYPE_OTHER+1] = {0};
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.messenger, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_messenger.type, &messengerType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		indexType = getMessengerIndexType(messengerType);
		used[indexType] = 1;
	}

	for (i=CTTEXT_MESSENGER_TYPE_AIM;i<=CTTEXT_MESSENGER_TYPE_JABBER;i++)  {
		if (0 == used[i]) {
			return getMessengerRetType(i);
		}
	}

	return CONTACTS_MESSENGER_TYPE_JABBER;
}

int CtInputData::removeMessenger(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.messenger, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtInputData::getWebsiteList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.url);
}

contacts_record_h CtInputData::addWebsite(const char *url)
{
	WDEBUG("url %s", url);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_url._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setWebsiteWebsite(record, url);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.url, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setWebsiteWebsite(contacts_record_h record, const char *url)
{
	WDEBUG("url %s", url);
	int err = CONTACTS_ERROR_NONE;

	if (url) {
		err = contacts_record_set_str(record, _contacts_url.url, url);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

const char* CtInputData::getWebsiteWebsite(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *url = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_url.url, &url);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = url;
	return result;
}

int CtInputData::removeWebsite(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.url, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtInputData::getRelationshipList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.relationship);
}

contacts_record_h CtInputData::addRelationship(const char *relationship)
{
	WDEBUG("relationship %s", relationship);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_relationship._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setRelationshipRelationship(record, relationship);
	setRelationshipType(record, getNextRelationshipType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.relationship, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setRelationshipRelationship(contacts_record_h record, const char *relationship)
{
	WDEBUG("relationship %s", relationship);
	int err = CONTACTS_ERROR_NONE;

	if (relationship) {
		err = contacts_record_set_str(record, _contacts_relationship.name, relationship);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::setRelationshipType(contacts_record_h record, int type, const char *customStr)
{
	WDEBUG("type %d", type);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_relationship.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}
	if (customStr) {
		err = contacts_record_set_str(record, _contacts_relationship.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

const char* CtInputData::getRelationshipRelationship(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *relationship = NULL;
	const char *result = NULL;

	err = contacts_record_get_str_p(record, _contacts_relationship.name, &relationship);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	result = relationship;
	return result;
}

int CtInputData::getRelationshipType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_relationship.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtInputData::getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString)
{
	WHIT();
	const char* typeStr = NULL;

	if (indexType == CONTACTS_RELATIONSHIP_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_relationship.label);
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_ASSISTANT) {
		typeStr = V_("IDS_PB_OPT_ASSISTANT_M_CONTACTS");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_BROTHER) {
		typeStr = V_("IDS_PB_OPT_BROTHER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_CHILD) {
		typeStr = V_("IDS_PB_OPT_CHILD");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_DOMESTIC_PARTNER) {
		typeStr = V_("IDS_PB_OPT_DOMESTIC_PARTNER_ABB2");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_FATHER) {
		typeStr = V_("IDS_PB_OPT_FATHER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_FRIEND) {
		typeStr = V_("IDS_PB_OPT_FRIEND");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_MANAGER) {
		typeStr = V_("IDS_PB_OPT_MANAGER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_MOTHER) {
		typeStr = V_("IDS_PB_OPT_MOTHER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_PARENT) {
		typeStr = V_("IDS_PB_OPT_PARENT");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_PARTNER) {
		typeStr = V_("IDS_PB_OPT_PARTNER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_REFERRED_BY) {
		typeStr = V_("IDS_PB_OPT_REFERRED_BY_ABB");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_RELATIVE) {
		typeStr = V_("IDS_PB_OPT_RELATIVE");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_SISTER) {
		typeStr = V_("IDS_PB_OPT_SISTER");
	}
	else if (indexType == CONTACTS_RELATIONSHIP_TYPE_SPOUSE) {
		typeStr = V_("IDS_PB_OPT_SPOUSE");
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNextRelationshipType()
{
	int i;
	int relationshipType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	int used[CONTACTS_RELATIONSHIP_TYPE_SPOUSE+1] = {0};

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.relationship, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_relationship.type, &relationshipType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		used[relationshipType] = 1;
	}

	for (i=CONTACTS_RELATIONSHIP_TYPE_ASSISTANT;i<=CONTACTS_RELATIONSHIP_TYPE_SPOUSE;i++) {
		if (!used[i])
			return i;
	}

	return CONTACTS_RELATIONSHIP_TYPE_SPOUSE;

}

int CtInputData::removeRelationship(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.relationship, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtInputData::getEventList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_contact.event);
}

contacts_record_h CtInputData::addEvent(int date)
{
	WDEBUG("date %d", date);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_event._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setEventEvent(record, date);
	setEventType(record, getNextEventType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.event, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtInputData::setEventEvent(contacts_record_h record, int date)
{
	WDEBUG("date %d", date);
	int err = CONTACTS_ERROR_NONE;

	if (date > 0) {
		err = contacts_record_set_int(record, _contacts_event.date, date);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
	}
	return err;
}

int CtInputData::setEventType(contacts_record_h record, int type, char *customStr)
{
	WDEBUG("type %d", type);
	int err = CONTACTS_ERROR_NONE;

	if (type >= 0) {
		err = contacts_record_set_int(record, _contacts_event.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}
	if (customStr) {
		err = contacts_record_set_str(record, _contacts_event.label, customStr);
		WPRET_VM(CONTACTS_ERROR_NONE != err,  err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtInputData::getEventEvent(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int date = 0;

	err = contacts_record_get_int(record, _contacts_event.date, &date);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return date;
}

int CtInputData::getEventType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_event.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtInputData::getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString)
{
	WHIT();
	const char* typeStr = NULL;

	if (eventType == CONTACTS_EVENT_TYPE_CUSTOM) {
		typeStr = getDataTypeCustom(record, _contacts_event.label);
	}
	else if (eventType == CONTACTS_EVENT_TYPE_BIRTH) {
		typeStr = V_("IDS_PB_OPT_BIRTHDAY");
	}
	else if (eventType == CONTACTS_EVENT_TYPE_ANNIVERSARY) {
		typeStr = V_("IDS_PB_OPT_ANNIVERSARY_ABB");
	}
	else {
		typeStr = V_("IDS_PB_OPT_OTHER");
	}

	char *result = elm_entry_utf8_to_markup(typeStr);
	if (result) {
		typeString = result;
		free(result);
	}
}

int CtInputData::getNextEventType()
{
	int eventType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	bool usedBirthday = false;
	bool usedAnniversary = false;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_contact.event, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_event.type, &eventType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		if (CONTACTS_EVENT_TYPE_BIRTH == eventType) {
			usedBirthday = true;
		}
		else if (CONTACTS_EVENT_TYPE_ANNIVERSARY == eventType) {
			usedAnniversary = true;
		}
		if (usedBirthday && usedAnniversary) {
			break;
		}
	}

	if (!usedBirthday) {
		return CONTACTS_EVENT_TYPE_BIRTH;
	}
	else if (!usedAnniversary) {
		return CONTACTS_EVENT_TYPE_ANNIVERSARY;
	}

	return CONTACTS_EVENT_TYPE_OTHER;

}

int CtInputData::removeEvent(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_contact.event, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

void CtInputData::__removeEmptyFiedls()
{
	removeEmptyFieldRecord(_contacts_contact.number, _contacts_number.number);
	removeEmptyFieldRecord(_contacts_contact.email, _contacts_email.email);
	removeEmptyFieldRecord(_contacts_contact.address, _contacts_address.street);
	removeEmptyFieldRecord(_contacts_contact.url, _contacts_url.url);
	removeEmptyFieldRecord(_contacts_contact.messenger, _contacts_messenger.im_id);
	removeEmptyFieldRecord(_contacts_contact.note, _contacts_note.note);
	removeEmptyFieldRecord(_contacts_contact.relationship, _contacts_relationship.name);
}

int CtInputData::insertContact(int *contactId)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_set_int(__contactRecord, _contacts_contact.address_book_id, __baseAddressbookId);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_int() Failed(%d)", err);
	err = contacts_record_set_int(__contactRecord, _contacts_contact.link_mode, CONTACTS_CONTACT_LINK_MODE_IGNORE_ONCE);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_int() Failed(%d)", err);
	return contacts_db_insert_record(__contactRecord, contactId);
}

int CtInputData::updateContact()
{
	WHIT();
	return contacts_db_update_record(__contactRecord);
}

int CtInputData::getContactId()
{
	WHIT();
	return __contactId;
}

int CtInputData::__getBaseAddressbookId(void)
{
	WHIT();
	int addressBookId = 0;
	char* addressbookName = NULL;

	int error = preference_get_string(CT_KEY_INPUT_BASE_ADDRESSBOOK, &addressbookName);
	WPRET_VM(error != PREFERENCE_ERROR_NONE, 0, "preference_get_string failed error code (%d)", error);
	WPRET_VM(!addressbookName, 0, "preference_get_string(%d) Failed", CT_KEY_INPUT_BASE_ADDRESSBOOK);

	if (addressbookName && *addressbookName) {
		addressBookId = ctCommonGetAddressBookId(addressbookName);
	}
	else {
		addressBookId = 0;
	}
	free(addressbookName);

	if(addressBookId == -1) {
		addressBookId = 0;
	}

	WDEBUG("%d",addressBookId);
	return addressBookId;
}

Eina_List* CtInputData::__getChildDataList(contacts_record_h contactRecord, unsigned int propertyId)
{
	WDEBUG("propertyId %d", propertyId);
	int index = 0;
//	Eina_List *l;
	Eina_List *dataRecordList = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
//	contacts_record_h dataRecordClone = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(contactRecord, propertyId, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		//err = contacts_record_clone(dataRecord, &dataRecordClone);
		//if (CONTACTS_ERROR_NONE != err) {
		//	WERROR("contacts_list_get_current_record_p() Failed(%d)", err);
		//	break;
		//}
		dataRecordList = eina_list_append(dataRecordList, dataRecord);
	}

	return dataRecordList;
}

void CtInputData::removeEmptyFieldRecord(int property_id, int property_id_str)
{
	int i;
	int count;
	char *str1 = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	bool empty = false;

	err = contacts_record_get_child_record_count(__contactRecord, property_id, &count);
	WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_get_child_record_count() Failed(%d)", err);

	for (i=count-1;0<=i;i--) {
		empty = false;
		err = contacts_record_get_child_record_at_p(__contactRecord, property_id, i, &record);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_child_record_at_p() Failed(%d)", err);
			continue;
		}

		err = contacts_record_get_str(record, property_id_str, &str1);
		WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);
		if (CONTACTS_ERROR_NONE != err || NULL == str1 || '\0' == *str1) {
			empty = true;
		}
		free(str1);

		if (empty) {
			err = contacts_record_remove_child_record(__contactRecord, property_id, record);
			WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_remove_child_record() Failed(%d)", err);
			err = contacts_record_destroy(record, true);
			WPRET_M(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
		}
	}
}

void CtInputData::attachListener(ICtChangeListener* listener)
{
	WPRET_M(NULL == listener, "listener is NULL");
	__listener = listener;
}

void CtInputData::detachListener()
{
	__listener = NULL;
}

contacts_record_h CtInputData::addGroupRelation(int groupId)
{
	contacts_record_h groupRelationRecord= NULL;
	int err = contacts_record_create(_contacts_group_relation._uri, &groupRelationRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	err = contacts_record_set_int(groupRelationRecord, _contacts_group_relation.group_id, groupId);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_set_int() Failed(%d)", err);
		contacts_record_destroy(groupRelationRecord, true);

		return NULL;
	}

	err = contacts_record_add_child_record(__contactRecord, _contacts_contact.group_relation, groupRelationRecord);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(groupRelationRecord, true);

		return NULL;
	}

	return groupRelationRecord;
}

// for check input addressbook item is added or not
void CtInputData::setInputAddressbookItem(bool added)
{
	__inputAddressbook = added;
}
bool CtInputData::getInputAddressbookItem(void)
{
	return __inputAddressbook;
}
