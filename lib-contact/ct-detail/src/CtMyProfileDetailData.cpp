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

#include "CtMyProfileDetailData.h"
#include "CtCommon.h"

CtMyProfileDetailData::CtMyProfileDetailData()
{
	init();
	int err = CONTACTS_ERROR_NONE;

	__listener = NULL;
	err = contacts_db_add_changed_cb(_contacts_my_profile._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() Failed(%d)", err);
}

CtMyProfileDetailData::~CtMyProfileDetailData()
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	err = contacts_db_remove_changed_cb(_contacts_my_profile._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);
	deInit();
}

void CtMyProfileDetailData::deInit()
{
	WHIT();
	i18n_udate_destroy (__formatter);

	__freeRecordList(__numberList);
	__freeRecordList(__emailList);
	__freeRecordList(__addressList);
	__freeRecordList(__messengerList);
	__freeRecordList(__relationshipList);
	__freeRecordList(__urlList);
	__freeRecordList(__eventList);
	__freeRecordList(__noteList);
	__freeRecordList(__contactList);

	contacts_record_destroy(__defaultNumberRecord, true);
	contacts_record_destroy(__personRecord, true);
	free(__displayName);
	free(__imagePath);
	WHIT();
}

void CtMyProfileDetailData::init()
{
	WHIT();

	__imagePath = NULL;
	__displayName = NULL;
	__extraName = NULL;
	__phoneticName = NULL;
	__nickName = NULL;
	__company = NULL;
	__messageAlert = NULL;
	__vibration = NULL;

	__contactList = NULL;
	__numberList = NULL;
	__emailList = NULL;
	__addressList = NULL;
	__messengerList = NULL;
	__relationshipList = NULL;
	__urlList = NULL;
	__eventList = NULL;
	__noteList = NULL;

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h myprofileList = NULL;
	int count = 0;
	contacts_record_h record = NULL;

	err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

	if (myprofileList != NULL) {
		contacts_list_get_count(myprofileList, &count);
		if (count > 0) {
			err = contacts_list_get_current_record_p(myprofileList, &record);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);
			err = contacts_record_get_int(record, _contacts_my_profile.id, &__personId);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
		}
	}
	if(NULL == record){
			WPWARN(NULL == record, "record is NULL");
			contacts_list_destroy(myprofileList, true);
			return;
	}
	err = contacts_record_clone(record, &__personRecord);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
	err = contacts_list_destroy(myprofileList, true);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_count() Failed(%d)", err);

	// get basic info
	__getBasicInfo(__personRecord, __contactList);

	// get number list
	__defaultNumberRecord = __getDefaultNumber();
	__numberList = __getChildNumberList();
	// get email list
	__emailList = __getChildDataList(__contactList, _contacts_my_profile.email);
	// get address list
	__addressList = __getChildDataList(__contactList, _contacts_my_profile.address);
	// get messenger list
	__messengerList = __getChildDataList(__contactList, _contacts_my_profile.messenger);
	// get url list
	__urlList = __getChildDataList(__contactList, _contacts_my_profile.url);
	// get relationship list
	__relationshipList = __getChildDataList(__contactList, _contacts_my_profile.relationship);
	// get event list
	__eventList = __getChildDataList(__contactList, _contacts_my_profile.event);
	// get note list
	__noteList = __getChildDataList(__contactList, _contacts_my_profile.note);

	__formatter = __initTimeFormatter();
}

int CtMyProfileDetailData::getPersonId()
{
	WHIT();

	return __personId;
}

contacts_record_h CtMyProfileDetailData::getPersonRecord()
{
	WHIT();

	return __personRecord;
}

Eina_List* CtMyProfileDetailData::getContactsList()
{
	return __contactList;
}

int CtMyProfileDetailData::setFavorite(bool favorite)
{
	return 0;
}

bool CtMyProfileDetailData::getFavorite()
{
	return false;
}

const char* CtMyProfileDetailData::getImagePath()
{
	return __imagePath;
}

const char* CtMyProfileDetailData::getDisplayName()
{
	return __displayName;
}

const char* CtMyProfileDetailData::getExtraName()
{
	return __extraName;
}

const char* CtMyProfileDetailData::getPhoneticName()
{
	return __phoneticName;
}

const char* CtMyProfileDetailData::getNickName()
{
	return __nickName;
}

const char* CtMyProfileDetailData::getCompany()
{
	return __company;
}

const char* CtMyProfileDetailData::getMessageAlert()
{
	return __messageAlert;
}

const char* CtMyProfileDetailData::getVibration()
{
	return __vibration;
}


Eina_List* CtMyProfileDetailData::getNumberList()
{
	return __numberList;
}

Eina_List* CtMyProfileDetailData::getEmailList()
{
	return __emailList;
}

Eina_List* CtMyProfileDetailData::getAddressList()
{
	return __addressList;
}

Eina_List* CtMyProfileDetailData::getMessengerList()
{
	return __messengerList;
}

Eina_List* CtMyProfileDetailData::getUrlList()
{
	return __urlList;
}

Eina_List* CtMyProfileDetailData::getRelationshipList()
{
	return __relationshipList;
}

Eina_List* CtMyProfileDetailData::getEventList()
{
	return __eventList;
}

Eina_List* CtMyProfileDetailData::getNoteList()
{
	return __noteList;
}

contacts_record_h CtMyProfileDetailData::getDefNumberRecord()
{
	return __defaultNumberRecord;
}

const char* CtMyProfileDetailData::getNumber(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_number.number, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtMyProfileDetailData::getNumberType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_number.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, CONTACTS_NUMBER_TYPE_OTHER, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileDetailData::getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString)
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

const char* CtMyProfileDetailData::getNumberTypeCustom(contacts_record_h record)
{
	char* type = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_number.label, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtMyProfileDetailData::getEmail(contacts_record_h record)
{
	WHIT();
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_email.email, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtMyProfileDetailData::getEmailType(contacts_record_h record)
{
	WHIT();
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_email.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, CONTACTS_EMAIL_TYPE_OTHER, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileDetailData::getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtMyProfileDetailData::getAddress(contacts_record_h record, char *buf, int length)
{
	WHIT();
	return __getFullAddress(record, buf, length);
}


int CtMyProfileDetailData::getAddressType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_address.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtMyProfileDetailData::getDataTypeCustom(contacts_record_h record, int property_id)
{
	char* typeStr = NULL;
	const char* result = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, property_id, &typeStr);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	result = typeStr;
	return result;
}

void CtMyProfileDetailData::getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

const char* CtMyProfileDetailData::getUrl(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_url.url, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtMyProfileDetailData::getUrlType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_url.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtMyProfileDetailData::getMessenger(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_messenger.im_id, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtMyProfileDetailData::getMessengerType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_messenger.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileDetailData::getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtMyProfileDetailData::getEvent(contacts_record_h record)
{
	int date = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_event.date, &date);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return date;
}

int CtMyProfileDetailData::getEventType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_event.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileDetailData::getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString)
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

const char* CtMyProfileDetailData::getRelationship(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_relationship.name, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtMyProfileDetailData::getRelationshipType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_relationship.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtMyProfileDetailData::getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

const char* CtMyProfileDetailData::getNote(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_note.note, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

void CtMyProfileDetailData::__onPersonChanged(const char *viewUri, void *userData)
{
	WHIT();
	WPRET_M(NULL == userData, "userData is NULL");
	CtMyProfileDetailData* data = (CtMyProfileDetailData*)userData;
	WPRET_M(NULL == data->__listener, "__listener is NULL");

	data->deInit();
	data->init();

	data->__listener->onDbChanged(viewUri);
}

void CtMyProfileDetailData::__freeRecordList(Eina_List *recordList)
{
	int err = CONTACTS_ERROR_NONE;
	void *data = NULL;
	if(NULL == recordList) {
		return;
	}

	EINA_LIST_FREE(recordList, data) {
		contacts_record_h record = (contacts_record_h)data;
		if (NULL == record) continue;
		err = contacts_record_destroy(record, true);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_destroy() Failed(%d)", err);
			continue;
		}
	}
}

void CtMyProfileDetailData::__getBasicInfo(contacts_record_h personRecord, Eina_List *contactList)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str(personRecord, _contacts_my_profile.display_name, &__displayName);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str display_name %d", err);
	}
	err = contacts_record_get_str(personRecord, _contacts_my_profile.image_thumbnail_path, &__imagePath);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str image_thumbnail_path %d", err);
	}
	ctCommonRemoveNewline(__displayName);

	__getCompany(contactList, &__company);

	__getExtraName1(contactList, &__nickName, &__phoneticName);

	__getExtraName2(__nickName, __phoneticName, &__extraName);

}

void CtMyProfileDetailData::__getCompany(Eina_List *contactList, char **dest)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;

	contacts_record_h companyRecord = NULL;

	err = contacts_record_get_child_record_at_p(__personRecord, _contacts_my_profile.company, 0, &companyRecord);
	if(CONTACTS_ERROR_NONE != err)
		WERROR("contacts_record_get_child_record_at_p() Failed(%d)", err);
	else {
		__getCompanyStr(companyRecord, dest);
	}
}

void CtMyProfileDetailData::__getExtraName2(char* nick, char* phonetic, char** extra)
{
	WHIT();
	char name[CT_TEXT_MAX_LEN+1] = {0, };

	free(*extra);

	if(phonetic && *phonetic) {
		if(nick && *nick)
			snprintf(name, sizeof(name), "%s, %s", phonetic, nick);
		else
			snprintf(name, sizeof(name), "%s", phonetic);
	}
	else if(nick && *nick)
		snprintf(name, sizeof(name), "%s", nick);

	if( strlen(name) > 0)
		*extra = strdup(name);
	else
		*extra = NULL;
}


void CtMyProfileDetailData::__getExtraName1(Eina_List *contactList, char **nick, char **phonetic)
{
	WHIT();
	int err1 = CONTACTS_ERROR_NONE;
	int err2 = CONTACTS_ERROR_NONE;
	contacts_record_h nameRecord = NULL;
	contacts_record_h nicknameRecord = NULL;

	err1 = contacts_record_get_child_record_at_p(__personRecord, _contacts_my_profile.name, 0, &nameRecord);
	WPWARN(CONTACTS_ERROR_NONE != err1, "contacts_record_get_child_record_at_p() name Failed(%d)", err1);
	err2 = contacts_record_get_child_record_at_p(__personRecord, _contacts_my_profile.nickname, 0, &nicknameRecord);
	WPWARN(CONTACTS_ERROR_NONE != err2, "contacts_record_get_child_record_at_p() nickname Failed(%d)", err2);

	if (CONTACTS_ERROR_NONE == err1 || CONTACTS_ERROR_NONE == err2) {
		__getExtraNameStr(nameRecord, nicknameRecord, nick, phonetic);
	}
}

void CtMyProfileDetailData::__getExtraNameStr(contacts_record_h nameRecord, contacts_record_h nicknameRecord, char **nick, char **phonetic)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *nickname = NULL;
	int length = 0;

	free(*nick);
	free(*phonetic);
	*nick = NULL;
	*phonetic = NULL;

	char *phoneticFirst = NULL;
	char *phoneticMiddle = NULL;
	char *phoneticLast = NULL;
	char *phoneticFull = NULL;

	if(nicknameRecord) {
		err = contacts_record_get_str_p(nicknameRecord, _contacts_nickname.name, (char**)&nickname);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() name Failed(%d)", err);
		*nick = SAFE_STRDUP(nickname);
	}

	if(nameRecord) {

		 contacts_name_display_order_e name_display_order;
		 contacts_setting_get_name_display_order(&name_display_order);

		 err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_first, (char**)&phoneticFirst);
		 WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() phoneticname Failed(%d)", err);

		 err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_middle, (char**)&phoneticMiddle);
		 WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() phoneticname Failed(%d)", err);

		 err = contacts_record_get_str_p(nameRecord, _contacts_name.phonetic_last, (char**)&phoneticLast);
		 WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() phoneticname Failed(%d)", err);

		 int phoneticLength = SAFE_STRLEN(phoneticFirst) + SAFE_STRLEN(phoneticMiddle) + SAFE_STRLEN(phoneticLast) + 3;
		 WDEBUG("phoneticLength = %d", phoneticLength);
		 phoneticFull = (char*)malloc(sizeof(char)*phoneticLength);
		 WPRET_M(NULL == phoneticFull, "failed to malloc");
		 memset(phoneticFull, 0, sizeof(char)*phoneticLength);

		 if (CONTACTS_NAME_DISPLAY_ORDER_FIRSTLAST == name_display_order) {
			 if (phoneticFirst && *phoneticFirst) {
				 strncat(phoneticFull, phoneticFirst, SAFE_STRLEN(phoneticFirst));
			 }
			 if (phoneticMiddle && *phoneticMiddle) {
				 if (phoneticFull && *phoneticFull) {
					 strncat(phoneticFull, " ", 1);
				 }
				 strncat(phoneticFull, phoneticMiddle, SAFE_STRLEN(phoneticMiddle));
			 }
			 if (phoneticLast && *phoneticLast) {
				 if (phoneticFull && *phoneticFull) {
					 strncat(phoneticFull, " ", 1);
				 }
				 strncat(phoneticFull, phoneticLast, SAFE_STRLEN(phoneticLast));
			 }
		 }
		 else {
			 if (phoneticLast && *phoneticLast) {
				 strncat(phoneticFull, phoneticLast, SAFE_STRLEN(phoneticLast));
			 }
			 if (phoneticFirst && *phoneticFirst) {
				 if (phoneticFull && *phoneticFull) {
					 strncat(phoneticFull, " ", 1);
				 }
				 strncat(phoneticFull, phoneticFirst, SAFE_STRLEN(phoneticFirst));
			 }
			 if (phoneticMiddle && *phoneticMiddle) {
				 if (phoneticFull && *phoneticFull) {
					 strncat(phoneticFull, " ", 1);
				 }
				 strncat(phoneticFull, phoneticMiddle, SAFE_STRLEN(phoneticMiddle));
			 }
		 }
		 length += phoneticLength;

		 *phonetic = SAFE_STRDUP(phoneticFull);
		 free(phoneticFull);
	}

}

void CtMyProfileDetailData::__getCompanyStr(contacts_record_h companyRecord, char **dest)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	char *company, *job, *department;
	char *fullName = NULL;
	int length = 0;
	*dest = NULL;

	err = contacts_record_get_str_p(companyRecord, _contacts_company.name, (char**)&company);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
	err = contacts_record_get_str_p(companyRecord, _contacts_company.department, (char**)&department);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
	err = contacts_record_get_str_p(companyRecord, _contacts_company.job_title, (char**)&job);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);

	length = SAFE_STRLEN(company) + SAFE_STRLEN(department) + SAFE_STRLEN(job) + 5;
	WDEBUG("length = %d", length);

	fullName = (char*)malloc(sizeof(char)*length);
	WPRET_M(NULL == fullName, "failed to malloc");
	memset(fullName, 0, sizeof(char)*length);

	if (company) {
		strncat(fullName, company, SAFE_STRLEN(company));
	}

	if (department) {
		if (fullName && *fullName) {
			strncat(fullName, ", ", 2);
		}
		strncat(fullName, department, SAFE_STRLEN(department));
	}

	if (job) {
		if (fullName && *fullName) {
			strncat(fullName, ", ", 2);
		}
		strncat(fullName, job, SAFE_STRLEN(job));
	}

	*dest = SAFE_STRDUP(fullName);
	free(fullName);
	fullName = NULL;
}

void CtMyProfileDetailData::__appendStringWithLinebreak(const char *src, char *dest, int length)
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

int CtMyProfileDetailData::__getFullAddress(contacts_record_h addresssRecord, char *buf, int length)
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


Eina_List* CtMyProfileDetailData::__getChildDataList(Eina_List *contactList, unsigned int propertyId)
{
	WDEBUG("propertyId %d", propertyId);
	int index = 0;

	Eina_List *dataRecordList = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	contacts_record_h dataRecordClone = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__personRecord, propertyId, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}
		err = contacts_record_clone(dataRecord, &dataRecordClone);
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_list_get_current_record_p() Failed(%d)", err);
			break;
		}
		dataRecordList = eina_list_append(dataRecordList, dataRecordClone);
	}
	return dataRecordList;
}

Eina_List* CtMyProfileDetailData::__getChildNumberList()
{
	WHIT();

	int index = 0;
	bool value = 0;
	Eina_List *dataRecordList = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	contacts_record_h dataRecordClone = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__personRecord, _contacts_my_profile.number, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		err = contacts_record_get_bool(dataRecord, _contacts_number.is_default, &value);
		WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_bool() Failed(%d)", err);
		if (value != true) {
			err = contacts_record_clone(dataRecord, &dataRecordClone);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_record_clone() Failed(%d)", err);
				break;
			}
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
			dataRecordList = eina_list_append(dataRecordList, dataRecordClone);
		}
	}

	return dataRecordList;
}

contacts_record_h CtMyProfileDetailData::__getDefaultNumber()
{
	int index = 0;
	bool value = 0;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	contacts_record_h dataRecordClone = NULL;

	while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(__personRecord, _contacts_my_profile.number, index++, &dataRecord)) {
		if (CONTACTS_ERROR_NONE != err) {
			WERROR("contacts_record_get_str_p() Failed(%d)", err);
			break;
		}

		err = contacts_record_get_bool(dataRecord, _contacts_number.is_default, &value);
		WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_bool() Failed(%d)", err);
		if (value == true) {
			err = contacts_record_clone(dataRecord, &dataRecordClone);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_record_clone() Failed(%d)", err);
				break;
			}
			return dataRecordClone;
		}
	}

	return NULL;
}

i18n_udate_format_h CtMyProfileDetailData::__initTimeFormatter()
{
	const char *locale;
	int32_t pattern_capacity;
	i18n_uchar pattern[256];
	i18n_uchar customSkeleton[256];
	i18n_udate_format_h formatter = NULL;
	i18n_udatepg_h generator = NULL;
	int status = I18N_ERROR_NONE;
	int bestPatternLength;

	i18n_ustring_copy_ua_n(customSkeleton,  "yMMMMd", SAFE_STRLEN("yMMMMd"));

	status = i18n_ulocale_get_default(&locale);
	WPRET_VM(NULL == locale, NULL, "uloc_getDefault() return NULL");

	status = i18n_udatepg_create (locale, &generator);

	pattern_capacity = (int32_t) (sizeof(pattern) / sizeof((pattern)[0]));
	status = i18n_udatepg_get_best_pattern (generator, customSkeleton, 6, pattern, pattern_capacity, &bestPatternLength);

	if (!status)
		status = i18n_udate_create (I18N_UDATE_PATTERN , I18N_UDATE_PATTERN , locale, NULL, -1, pattern, -1, &formatter);

	i18n_udatepg_destroy(generator);
	return formatter;
}

char* CtMyProfileDetailData::getDatetimeStr(i18n_udate_format_h formatter, int year, int mon, int day)
{
	char *ret_str = NULL;
	char formatted_str[256];
	i18n_uchar formatted[256] = {0, };
	int status = I18N_ERROR_NONE;
	i18n_udate  millis = 0;
	i18n_ucalendar_h cal = NULL;
	int formattedLength = 0;

	const char* locale = NULL;
	status = i18n_ulocale_get_default(&locale);

	status = i18n_ucalendar_create(NULL, -1, locale, I18N_UCALENDAR_TRADITIONAL , &cal);
	WPRET_VM(status, NULL, "ucal_open() Failed()");

	status = i18n_ucalendar_set_date_time(cal, year, mon-1, day, 0, 0, 0);
	status = i18n_ucalendar_set_attribute(cal, I18N_UCALENDAR_LENIENT , 1);
	status = i18n_ucalendar_get_milliseconds(cal, &millis);

	if (status) {
		WDEBUG("i18n_ucalendar_get_milliseconds() Failed()");
		i18n_ucalendar_destroy(cal);
		return NULL;
	}

	status = i18n_udate_format_date(formatter, millis, formatted, 256, NULL, &formattedLength);

	if (status) {
		WDEBUG("udat_format() Failed()");
		i18n_ucalendar_destroy(cal);
		return NULL;
	}

	i18n_ustring_copy_au (formatted_str, formatted);
	ret_str = g_strdup(formatted_str);
	i18n_ucalendar_destroy(cal);

	return ret_str;
}

i18n_udate_format_h CtMyProfileDetailData::getDateFormater()
{
	return __formatter;
}

bool CtMyProfileDetailData::getReadOnlyMode()
{
	return false;
}

void CtMyProfileDetailData::attachListener(ICtChangeListener* listener)
{
	WASSERT(listener);
	__listener = listener;
}

void CtMyProfileDetailData::detachListener()
{
	__listener = NULL;
}

char* CtMyProfileDetailData::getVcardStream() const
{
	char* vcardStream = NULL;
	int err = contacts_vcard_make_from_my_profile(__personRecord, &vcardStream);
	WPRET_VM(err != CONTACTS_ERROR_NONE, NULL, "contacts_vcard_make_from_my_profile() Failed(%d)", err);
	return vcardStream;
}
