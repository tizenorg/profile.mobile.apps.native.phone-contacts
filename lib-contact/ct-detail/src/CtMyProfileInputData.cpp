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

#include "ContactsDebug.h"
#include "CtMyProfileInputData.h"
#include "CtCommon.h"

CtMyProfileInputData::CtMyProfileInputData(contacts_record_h contactRecord)
{
	WHIT();
	__contactRecord = contactRecord;
	__contactId = 0;
	__init();

}

CtMyProfileInputData::CtMyProfileInputData()
{
	WHIT();
	__init();
}

CtMyProfileInputData::~CtMyProfileInputData()
{
	WHIT();
	contacts_record_destroy(__contactRecord, true);
}

//contacts_record_h CtMyProfileInputData::getContactRecord()
//{
//	WHIT();
//	return __contactRecord;
//}

void CtMyProfileInputData::__init()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	contacts_list_h myprofileList = NULL;
	int count = 0;
	contacts_record_h record = NULL;

	err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

	if (myprofileList != NULL) {
//		int myprofile_id = 0;
		contacts_list_get_count(myprofileList, &count);
		if (count > 0) {
			err = contacts_list_get_current_record_p(myprofileList, &record);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);
			err = contacts_record_get_int(record, _contacts_my_profile.id, &__contactId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		}

		err = contacts_record_clone(record, &__contactRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
		contacts_list_destroy(myprofileList, true);
	}

	if (__contactRecord == NULL) {
		err = contacts_record_create(_contacts_my_profile._uri, &__contactRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d)", err);
	}
}

int CtMyProfileInputData::separateFullName(contacts_record_h nameRecord, const char *full_name)
{
	WHIT();
	WPRET_VM(NULL == nameRecord, -1, "nameRecord is NULL");
	WPRET_VM(NULL == full_name, -1, "full_name is NULL");
	int err = CONTACTS_ERROR_NONE;
	char *temp_name = NULL;
	char *temp_first = NULL;
	char *temp_middle = NULL;
	char *temp_last = NULL;
	char tmp[CT_TEXT_MAX_LEN] = {0};

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
			err = contacts_record_set_str(nameRecord, _contacts_name.first, temp_first);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

			err = contacts_record_set_str(nameRecord, _contacts_name.addition, temp_middle);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

			err = contacts_record_set_str(nameRecord, _contacts_name.last, temp_last);
			WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
		}
		else {
			if(temp_middle && *temp_middle) {
				err = contacts_record_set_str(nameRecord, _contacts_name.first, temp_first);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.addition, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.last, temp_middle);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
			}
			else {
				err = contacts_record_set_str(nameRecord, _contacts_name.first, temp_first);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.addition, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

				err = contacts_record_set_str(nameRecord, _contacts_name.last, NULL);
				WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
			}
		}
	}else {
		err = contacts_record_set_str(nameRecord, _contacts_name.first, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.addition, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);

		err = contacts_record_set_str(nameRecord, _contacts_name.last, NULL);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtMyProfileInputData::separatePhoneticFullName(contacts_record_h nameRecord, const char *full_name)
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

int CtMyProfileInputData::setPrefixName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setSuffixName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setPhoneticFullName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	//WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	if(nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = separatePhoneticFullName(nameRecord, text);
	return err;
}

int CtMyProfileInputData::setPhoneticFirstName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setPhoneticMiddleName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setPhoneticLastName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setNickname(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.nickname, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_nickname._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.nickname, nameRecord);
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

int CtMyProfileInputData::setCompany(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.company, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_company._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.company, nameRecord);
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

int CtMyProfileInputData::setJobTitle(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.company, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_company._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.company, nameRecord);
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

int CtMyProfileInputData::setFullName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	//WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	if(nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
		if(CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_add_child_record() Failed(%d)", err);
			contacts_record_destroy(nameRecord, true);
			return err;
		}
	}

	err = separateFullName(nameRecord, text);
	return err;
}

int CtMyProfileInputData::setFirstName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setMiddleName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

int CtMyProfileInputData::setLastName(const char *text)
{
	WDEBUG("text %s", text);
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);

	if (nameRecord == NULL) {
		err = contacts_record_create(_contacts_name._uri, &nameRecord);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_create() Failed(%d)", err);

		err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.name, nameRecord);
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

const char* CtMyProfileInputData::getPrefixName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *prefix = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.prefix, &prefix);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return prefix;
}

const char* CtMyProfileInputData::getSuffixName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *suffix = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.suffix, &suffix);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return suffix;
}

const char* CtMyProfileInputData::getPhoneticFirstName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_first, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return phoneticName;
}

const char* CtMyProfileInputData::getPhoneticMiddleName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_middle, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return phoneticName;
}

const char* CtMyProfileInputData::getPhoneticLastName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *phoneticName = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_last, &phoneticName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return phoneticName;
}

const char* CtMyProfileInputData::getNickname()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *nickname = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.nickname, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_nickname.name, &nickname);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return nickname;
}

const char* CtMyProfileInputData::getCompany()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *company = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.company, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_company.name, &company);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return company;
}

const char* CtMyProfileInputData::getJobTitle()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *jobTitle = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.company, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_company.job_title, &jobTitle);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	return jobTitle;
}

/**
  * @return must be freed by you
  */
void CtMyProfileInputData::getFullName(std::string &Name)
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

const char* CtMyProfileInputData::getFirstName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *firstName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.first, &firstName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	result = firstName;
	return result;
}

const char* CtMyProfileInputData::getMiddleName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *middleName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.addition, &middleName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	result = middleName;
	return result;
}

const char* CtMyProfileInputData::getLastName()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	char *lastName = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(nameRecord, _contacts_name.last, &lastName);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	result = lastName;
	return result;
}

int CtMyProfileInputData::removeImage(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.image, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	err = contacts_record_destroy(record, true);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_destroy() Failed(%d)", err);

	return err;
}

const char* CtMyProfileInputData::getImagePath()
{
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h imageRecord = NULL;
	char *image = NULL;
	const char *result = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.image, 0, &imageRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);

	err = contacts_record_get_str_p(imageRecord, _contacts_image.path, &image);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_set_str() Failed(%d)", err);

	result = image;
	return result;
}
contacts_record_h CtMyProfileInputData::getImageRecord()
{
	WHIT();

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.image, 0, &record);
	if (NULL != record)	{
		return record;
	}

	err = contacts_record_create(_contacts_image._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.image, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_add_child_record() Failed(%d)", err);

	return record;
}

int CtMyProfileInputData::setImage(contacts_record_h record, char *image, int type)
{
	WDEBUG("image %s", image);
	int err = CONTACTS_ERROR_NONE;

	if(image && *image) {
		err = contacts_record_set_int(record, _contacts_image.type, type);
		err = contacts_record_set_str(record, _contacts_image.path, image);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

Eina_List* CtMyProfileInputData::getNumberList()
{
	WHIT();

	int index = 0;
	bool value = 0;
	Eina_List *dataRecordList = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	const char* typeStr = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.number, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		err = contacts_record_get_bool(dataRecord, _contacts_number.is_default, &value);
		WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_bool() Failed(%d)", err);
		typeStr = getDataTypeCustom(dataRecord, _contacts_number.label);

		if (value == true && (typeStr && 0 == strcmp(typeStr, CT_KEY_SIM_CARD_NUMBER))) {
			continue;
		}
		dataRecordList = eina_list_append(dataRecordList, dataRecord);
	}

	return dataRecordList;
}

contacts_record_h CtMyProfileInputData::getEmergencyNumber()
{
	WHIT();
	return NULL;
}

contacts_record_h CtMyProfileInputData::getDefaultNumber()
{
	int index = 0;
	bool value = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	const char* typeStr = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.number, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		err = contacts_record_get_bool(dataRecord, _contacts_number.is_default, &value);
		WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_bool() Failed(%d)", err);

		typeStr = getDataTypeCustom(dataRecord, _contacts_number.label);
		if (value == true && (typeStr && 0 == strcmp(typeStr, CT_KEY_SIM_CARD_NUMBER))) {
			return dataRecord;
		}
	}

	return NULL;
}

contacts_record_h CtMyProfileInputData::addNumber(const char *number)
{
	WDEBUG("number %s", number);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_number._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setNumberNumber(record, number);
	setNumberType(record, getNextNumberType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.number, record);
	if(CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setNumberNumber(contacts_record_h record, const char *number)
{
	WDEBUG("number %s", number);
	int err = CONTACTS_ERROR_NONE;

	if(number) {
		err = contacts_record_set_str(record, _contacts_number.number, number);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtMyProfileInputData::setNumberType(contacts_record_h record, int type, const char *customStr)
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

int CtMyProfileInputData::removeNumber(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.number, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

const char* CtMyProfileInputData::getNumberNumber(contacts_record_h record)
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

int CtMyProfileInputData::getNumberType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_number.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

Eina_List* CtMyProfileInputData::getEmailList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.email);
}

const char* CtMyProfileInputData::getDataTypeCustom(contacts_record_h record, int property_id)
{
	char* typeStr = NULL;
	const char* result = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, property_id, &typeStr);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	result = typeStr;
	return result;
}

void CtMyProfileInputData::getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString)
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
			typeStr = V_("IDS_PB_BODY_TELEPHONE");
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

int CtMyProfileInputData::getNumberRetType(int indexType)
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
int CtMyProfileInputData::getNumberIndexType(int numberType)
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
	WERROR("Not handled case numberType=%d", numberType);
	return (CTTEXT_TYPE_OTHER);
}

int CtMyProfileInputData::getNextNumberType()
{
	int i;
	int numberType;
	int indexType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	int used[CTTEXT_TYPE_OTHER+1] = {0};
	contacts_record_h record = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.number, index++, &record)) {
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

contacts_record_h CtMyProfileInputData::addEmail(const char *email)
{
	WDEBUG("email %s", email);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_email._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setEmailEmail(record, email);
	setEmailType(record, getNextEmailType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.email, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setEmailEmail(contacts_record_h record, const char *email)
{
	WDEBUG("email %s", email);
	int err = CONTACTS_ERROR_NONE;

	if (email) {
		err = contacts_record_set_str(record, _contacts_email.email, email);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtMyProfileInputData::setEmailType(contacts_record_h record, int type, char *customStr)
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

const char* CtMyProfileInputData::getEmailEmail(contacts_record_h record)
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

int CtMyProfileInputData::getEmailType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_email.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileInputData::getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtMyProfileInputData::getNextEmailType()
{
	int i;
	int used = 0;
	int emailType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.email, index++, &record)) {
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

int CtMyProfileInputData::removeEmail(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.email, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

contacts_record_h CtMyProfileInputData::getNoteRecord()
{
	WHIT();
	contacts_record_h record = NULL;
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.note, 0, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_child_record_at_p() Failed(%d)", err);
	return record;
}

contacts_record_h CtMyProfileInputData::addNote(const char *note)
{
	WDEBUG("note %s", note);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_note._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setNote(record, note);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.note, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setNote(contacts_record_h record, const char *note)
{
	WDEBUG("note %s", note);
	int err = CONTACTS_ERROR_NONE;

	if (note) {
		err = contacts_record_set_str(record, _contacts_note.note, note);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

Eina_List* CtMyProfileInputData::getNoteList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.note);
}

const char* CtMyProfileInputData::getNote(contacts_record_h record)
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

int CtMyProfileInputData::removeNote(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.note, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtMyProfileInputData::getAddressList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.address);
}

contacts_record_h CtMyProfileInputData::addAddress(const char *address)
{
	WDEBUG("address %s", address);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_address._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setAddressAddress(record, address);
	setAddressType(record, getNextAddressType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.address, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setAddressAddress(contacts_record_h record, const char *address)
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

int CtMyProfileInputData::setAddressType(contacts_record_h record, int type, const char *customStr)
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

int CtMyProfileInputData::getAddressAddress(contacts_record_h record, char *buf, int length)
{
	WHIT();
	return __getFullAddress(record, buf, length);
}

void CtMyProfileInputData::__appendStringWithLinebreak(const char *src, char *dest, int length)
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

int CtMyProfileInputData::__getFullAddress(contacts_record_h addresssRecord, char *buf, int length)
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

int CtMyProfileInputData::getAddressType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_address.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileInputData::getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtMyProfileInputData::getNextAddressType()
{
	int i;
	int used = 0;
	int addressType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.address, index++, &record)) {
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

int CtMyProfileInputData::removeAddress(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.address, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtMyProfileInputData::getMessengerList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.messenger);
}

contacts_record_h CtMyProfileInputData::addMessenger(const char *messenger)
{
	WDEBUG("messenger %s", messenger);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_messenger._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setMessengerMessenger(record, messenger);
	setMessengerType(record, getNextMessengerType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.messenger, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setMessengerMessenger(contacts_record_h record, const char *messenger)
{
	WDEBUG("messenger %s", messenger);
	int err = CONTACTS_ERROR_NONE;

	if (messenger) {
		err = contacts_record_set_str(record, _contacts_messenger.im_id, messenger);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

int CtMyProfileInputData::setMessengerType(contacts_record_h record, int type, const char *customStr)
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

int CtMyProfileInputData::getMessengerIndexType(int emailType)
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
	case CONTACTS_MESSENGER_TYPE_ICQ:
		return CTTEXT_MESSENGER_TYPE_ICQ;
	case CONTACTS_MESSENGER_TYPE_JABBER:
	default:
		return CTTEXT_MESSENGER_TYPE_JABBER;
	}
}

int CtMyProfileInputData::getMessengerRetType(int indexType)
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

const char* CtMyProfileInputData::getMessengerMessenger(contacts_record_h record)
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

int CtMyProfileInputData::getMessengerType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_messenger.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileInputData::getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtMyProfileInputData::getNextMessengerType()
{
	int i;
	int messengerType;
	int indexType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	int used[CTTEXT_TYPE_OTHER+1] = {0};
	contacts_record_h record = NULL;
	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.messenger, index++, &record)) {
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

	return CONTACTS_MESSENGER_TYPE_CUSTOM;

}

int CtMyProfileInputData::removeMessenger(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.messenger, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtMyProfileInputData::getWebsiteList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.url);
}

contacts_record_h CtMyProfileInputData::addWebsite(const char *url)
{
	WDEBUG("url %s", url);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_url._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setWebsiteWebsite(record, url);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.url, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setWebsiteWebsite(contacts_record_h record, const char *url)
{
	WDEBUG("url %s", url);
	int err = CONTACTS_ERROR_NONE;

	if (url) {
		err = contacts_record_set_str(record, _contacts_url.url, url);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
	}

	return err;
}

const char* CtMyProfileInputData::getWebsiteWebsite(contacts_record_h record)
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

int CtMyProfileInputData::removeWebsite(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.url, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

Eina_List* CtMyProfileInputData::getEventList()
{
	WHIT();
	return __getChildDataList(__contactRecord, _contacts_my_profile.event);
}

contacts_record_h CtMyProfileInputData::addEvent(int date)
{
	WDEBUG("date %d", date);

	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;

	err = contacts_record_create(_contacts_event._uri, &record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_create() Failed(%d)", err);

	setEventEvent(record, date);
	setEventType(record, getNextEventType(), NULL);

	err = contacts_record_add_child_record(__contactRecord, _contacts_my_profile.event, record);
	if (CONTACTS_ERROR_NONE != err) {
		WERROR("contacts_record_add_child_record() Failed(%d)", err);
		contacts_record_destroy(record, true);
		return NULL;
	}
	return record;
}

int CtMyProfileInputData::setEventEvent(contacts_record_h record, int date)
{
	WDEBUG("date %d", date);
	int err = CONTACTS_ERROR_NONE;

	if (date > 0) {
		err = contacts_record_set_int(record, _contacts_event.date, date);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
	}
	return err;
}

int CtMyProfileInputData::setEventType(contacts_record_h record, int type, char *customStr)
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

int CtMyProfileInputData::getEventEvent(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int date = 0;

	err = contacts_record_get_int(record, _contacts_event.date, &date);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return date;
}

int CtMyProfileInputData::getEventType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_event.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileInputData::getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString)
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

int CtMyProfileInputData::getNextEventType()
{
	int eventType;
	int index = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h record = NULL;
	bool usedBirthday = false;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__contactRecord, _contacts_my_profile.event, index++, &record)) {
		err = contacts_record_get_int(record, _contacts_event.type, &eventType);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		if (CONTACTS_EVENT_TYPE_BIRTH == eventType) {
			usedBirthday = true;
			break;
		}
	}

	if (usedBirthday)
		return CONTACTS_EVENT_TYPE_ANNIVERSARY;

	return CONTACTS_EVENT_TYPE_BIRTH;

}

int CtMyProfileInputData::removeEvent(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	err = contacts_record_remove_child_record(__contactRecord, _contacts_my_profile.event, record);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	return err;
}

void CtMyProfileInputData::__removeEmptyFiedls()
{
	removeEmptyFieldRecord(_contacts_my_profile.number, _contacts_number.number);
	removeEmptyFieldRecord(_contacts_my_profile.email, _contacts_email.email);
	removeEmptyFieldRecord(_contacts_my_profile.address, _contacts_address.street);
	removeEmptyFieldRecord(_contacts_my_profile.url, _contacts_url.url);
	removeEmptyFieldRecord(_contacts_my_profile.messenger, _contacts_messenger.im_id);
	removeEmptyFieldRecord(_contacts_my_profile.note, _contacts_note.note);
}

int CtMyProfileInputData::insertContact(int *contactId)
{
	WHIT();
	return contacts_db_insert_record(__contactRecord, contactId);
}

int CtMyProfileInputData::updateContact()
{
	WHIT();
	return contacts_db_update_record(__contactRecord);
}

int CtMyProfileInputData::getContactId()
{
	WHIT();
	return __contactId;
}

Eina_List* CtMyProfileInputData::__getChildDataList(contacts_record_h contactRecord, unsigned int propertyId)
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

void CtMyProfileInputData::setPreferenceCreateFirst(bool isFirst)
{
	WHIT();
}

int CtMyProfileInputData::getAddressbookId()
{
	WHIT();
	return 0;
}

const char* CtMyProfileInputData::getAddressbookName()
{
	WHIT();
	return NULL;
}

const char* CtMyProfileInputData::getUserName()
{
	WHIT();
	return NULL;
}

void CtMyProfileInputData::setAddressbookInfo(int addressbookId)
{
	WHIT();
}

Eina_List* CtMyProfileInputData::getRelationshipList()
{
	WHIT();
	return NULL;
}

contacts_record_h CtMyProfileInputData::addRelationship(const char *relationship)
{
	WHIT();
	return NULL;
}

int CtMyProfileInputData::setRelationshipRelationship(contacts_record_h record, const char *relationship)
{
	WHIT();
	return 0;
}

int CtMyProfileInputData::setRelationshipType(contacts_record_h record, int type, const char *customStr)
{
	WHIT();
	return 0;
}

const char* CtMyProfileInputData::getRelationshipRelationship(contacts_record_h record)
{
	WHIT();
	return NULL;
}

int CtMyProfileInputData::getRelationshipType(contacts_record_h record)
{
	WHIT();
	return 0;
}

void CtMyProfileInputData::getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString)
{
	WHIT();
}

int CtMyProfileInputData::getNextRelationshipType()
{
	WHIT();
	return 0;
}

int CtMyProfileInputData::removeRelationship(contacts_record_h record)
{
	WHIT();
	return 0;
}

contacts_record_h CtMyProfileInputData::getDefaultEmail()
{
	WHIT();
	return NULL;
}

bool CtMyProfileInputData::checkChangedTypeIsDeleted()
{
	WHIT();
	return false;
}

void CtMyProfileInputData::attachListener(ICtChangeListener* listener)
{
	WHIT();
}

void CtMyProfileInputData::detachListener()
{
	WHIT();
}

void CtMyProfileInputData::removeEmptyFieldRecord(int property_id, int property_id_str)
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
