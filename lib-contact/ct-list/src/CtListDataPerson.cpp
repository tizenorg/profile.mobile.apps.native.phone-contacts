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

#include <string.h>
#include "CtListDataPerson.h"
#include "ContactsDebug.h"
#include "CtCommon.h"
#include "CtType.h"

using namespace std;


CtListDataPerson::CtListDataPerson(contacts_record_h record)
{
	__initialize();
	__setPersonRecord(record);
}

CtListDataPerson::~CtListDataPerson()
{
	free(__displayName);
	free(__displayIndex);
	free(__imagePath);
	free(__dataString);
	contacts_record_destroy(__person, true);
}

CtListDataPerson* CtListDataPerson::makeClone()
{
	WHIT();
	CtListDataPerson *dataPerson = new CtListDataPerson(__person);
	dataPerson->setAsChecked(__checked);

	dataPerson->setDataString(__dataString);
	dataPerson->setDataType(__dataType);
	dataPerson->setDataId(__dataId);

	return dataPerson;
}

contacts_record_h CtListDataPerson::getPersonRecord()
{
	return __person;
}

const char* CtListDataPerson::getDisplayName()
{
	return __displayName;
}

const char* CtListDataPerson::getDisplayIndex()
{
	return __displayIndex;
}

const char* CtListDataPerson::getImagePath()
{
	return __imagePath;
}

void CtListDataPerson::setImagePath(const char *imagePath)
{
	FREEandSTRDUP(__imagePath, imagePath);
}

int CtListDataPerson::getPersonId()
{
	return __personId;
}

int CtListDataPerson::getLinkedCount()
{
	return __linkedCount;
}

bool CtListDataPerson::getFavorite()
{
	return __favorite;
}

bool isMFC()
{
	return false;
}

void CtListDataPerson::__initialize()
{
	__displayName = NULL;
	__displayIndex = NULL;
	__personId = 0;
	__person = NULL;
	__linkedCount = 1;
	__imagePath	= NULL;
	__addressbookId = 0;
	__checked = false;
	__dataString = NULL;
	__dataType = 0;
	__dataId = 0;
	__favorite = false;

}

void CtListDataPerson::__setPersonRecord(contacts_record_h record)
{
	//WHIT();
	int err = CONTACTS_ERROR_NONE;
	const char* uri = NULL;
	contacts_record_get_uri_p(record, &uri);

	err = contacts_record_clone(record, &__person);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);

	if(strcmp(uri, _contacts_contact._uri) == 0) {
		err = contacts_record_get_str(record, _contacts_contact.display_name, &__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_contact.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_contact.address_book_id, &__addressbookId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_bool(record, _contacts_contact.is_favorite, &__favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

	}
	else if(strcmp(uri, _contacts_person_number._uri) == 0){
		err = contacts_record_get_str(record, _contacts_person_number.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person_number.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_number.display_name_index, &__displayIndex);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_number.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

	}
	else if(strcmp(uri, _contacts_person_email._uri) == 0){
		err = contacts_record_get_str(record, _contacts_person_email.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person_email.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_email.display_name_index, &__displayIndex);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_email.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

	}
	else if(strcmp(uri, _contacts_person._uri) == 0){
		char* tmpStr = NULL;
		err = contacts_record_get_str(record, _contacts_person.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person.id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person.display_name_index, &__displayIndex);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person.addressbook_ids, &tmpStr);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_bool(record, _contacts_person.is_favorite, &__favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		if(tmpStr) {
			int linkCount = 0;
			char* ptr = NULL;
			char* token = strtok_r(tmpStr, " ", &ptr);
			while (token)
			{
				linkCount++;
				if(linkCount == 1)
					__addressbookId = atoi(token);

				token = strtok_r(NULL, " ", &ptr);
			}
			__linkedCount = linkCount;
			free(tmpStr);
		}
	}
	else if(strcmp(uri, _contacts_person_contact._uri) == 0){
		char* tmpStr = NULL;
		err = contacts_record_get_str(record, _contacts_person_contact.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person_contact.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_contact.display_name_index, &__displayIndex);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_contact.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_contact.addressbook_ids, &tmpStr);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_bool(record, _contacts_person_contact.is_favorite, &__favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		if(tmpStr) {
			int linkCount = 0;
			char* ptr = NULL;
			char* token = strtok_r(tmpStr, " ", &ptr);
			while (token)
			{
				linkCount++;
				if(linkCount == 1)
					__addressbookId = atoi(token);

				token = strtok_r(NULL, " ", &ptr);
			}
			__linkedCount = linkCount;
			free(tmpStr);
		}
	}
	else if(strcmp(uri, _contacts_person_usage._uri) == 0){
		err = contacts_record_get_str(record, _contacts_person_usage.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person_usage.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_usage.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

	}
	else if (strcmp(uri, _contacts_person_grouprel._uri) == 0) {
		err = contacts_record_get_str(record, _contacts_person_grouprel.display_name,&__displayName);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

		err = contacts_record_get_int(record, _contacts_person_grouprel.person_id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_get_str(record, _contacts_person_grouprel.image_thumbnail_path, &__imagePath);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);
	}

	ctCommonRemoveNewline(__displayName);
}

bool CtListDataPerson::isChecked()
{
    return __checked;
}

void CtListDataPerson::setAsChecked(bool checked)
{
    __checked = checked;
}

Eina_List* CtListDataPerson::getNumberList()
{
	if (__personId == 0) return NULL; // vcard

	return __getDataList(__personId, _contacts_person_number._uri);
}

Eina_List* CtListDataPerson::getEmailList()
{
	if (__personId == 0) return NULL; // vcard

	return __getDataList(__personId, _contacts_person_email._uri);
}


const char* CtListDataPerson::getDataString()
{
	return __dataString;
}

int CtListDataPerson::getDataType()
{
	return __dataType;
}

int CtListDataPerson::getDataId()
{
	return __dataId;
}

int CtListDataPerson::getAddressbookId()
{
	return __addressbookId;
}

void CtListDataPerson::setDataString(const char *data)
{
	__dataString = SAFE_STRDUP(data);
}

void CtListDataPerson::setDataType(int dataType)
{
	__dataType = dataType;
}

void CtListDataPerson::setDataId(int dataId)
{
	__dataId = dataId;
}

bool CtListDataPerson::getSearchNumber(const char *searchStr, std::string &numberStr)
{
	std::string search = searchStr;
	bool result = false;

	if(search != __searchStr) {
		__searchStr = searchStr;

		char* number = NULL;
		int err = CONTACTS_ERROR_NONE;
		contacts_list_h list = NULL;
		contacts_record_h record = NULL;
		contacts_query_h query = NULL;
		contacts_filter_h filter = NULL;
		contacts_filter_h filter2 = NULL;

		err = contacts_query_create(_contacts_person_number._uri, &query);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_create Failed(%d)", err);
		err = contacts_filter_create(_contacts_person_number._uri, &filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create Failed(%d)", err);
		err = contacts_filter_create(_contacts_person_number._uri, &filter2);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_create Failed(%d)", err);
		err = contacts_filter_add_int(filter, _contacts_person_number.person_id, CONTACTS_MATCH_EQUAL, getPersonId());
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_int Failed(%d)", err);
		err = contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_operator Failed(%d)", err);
		err = contacts_filter_add_str(filter2, _contacts_person_number.normalized_number, CONTACTS_MATCH_CONTAINS, __searchStr.c_str());
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_str Failed(%d)", err);
		err = contacts_filter_add_filter(filter, filter2);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_filter_add_filter Failed(%d)", err);
		err = contacts_query_set_filter(query, filter);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_query_set_filter Failed(%d)", err);
		err = contacts_db_get_records_with_query(query, 0, 0, &list);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_records_with_query Failed(%d)", err);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
		contacts_filter_destroy(filter2);
		if (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &record)) {
			err = contacts_record_get_str_p(record, _contacts_person_number.number, &number);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p Failed(%d)", err);
		}
		if(SAFE_STRLEN(number) > 0) {
			numberStr = number;
			__searchNumber = number;
			result = true;
		}
		else {
			__searchNumber.clear();
		}

		contacts_list_destroy(list, true);
	}
	else {
		if(__searchNumber.length() > 0) {
			numberStr = __searchNumber;
			result = true;
		}
	}

	return result;


}

Eina_List* CtListDataPerson::__getDataList(int personId, const char* viewUri)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	contacts_record_h recordClone = NULL;
	contacts_list_h list = NULL;
	Eina_List *result = NULL;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;

	err = contacts_filter_create(viewUri, &filter);
	WPWARN(CONTACTS_ERROR_NONE != err, "contactcs_filter_create() Failed(%d)", err);

	if(strcmp(viewUri, _contacts_person_number._uri) == 0)
		err = contacts_filter_add_int(filter, _contacts_person_number.person_id, CONTACTS_MATCH_EQUAL, personId);
	else if(strcmp(viewUri, _contacts_person_email._uri) == 0)
		err = contacts_filter_add_int(filter, _contacts_person_email.person_id, CONTACTS_MATCH_EQUAL, personId);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_filter_add_int() Failed(%d)", err);
		contacts_filter_destroy(filter);
		return NULL;
	}
	err = contacts_query_create(viewUri, &query);
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
	err = contacts_db_get_records_with_query(query, 0, 0, &list);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_db_get_records_with_query() Failed(%d)", err);
		if(list)
			contacts_list_destroy(list, true);
		contacts_filter_destroy(filter);
		contacts_query_destroy(query);
		return NULL;
	}

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);

	while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(list, &record)) {
		err = contacts_record_clone(record, &recordClone);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_clone() Failed(%d)", err);
			break;
		}
		result = eina_list_append(result, recordClone);
		contacts_list_next(list);
	}
	contacts_list_destroy(list, true);
	return result;
}

