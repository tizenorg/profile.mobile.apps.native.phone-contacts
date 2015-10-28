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

#include "CtDetailData.h"
#include "CtCommon.h"
#include "CtPath.h"

#include "ICtChangeListener.h"

CtDetailData::CtDetailData(int personId)
{
	WDEBUG("personId %d", personId);
	__personId = personId;
	__personRecord = NULL;

	init();

}

CtDetailData::CtDetailData(contacts_record_h personRecord)
{
	WHIT();
	__personId = 0;
	__personRecord = personRecord;

	init();

}

CtDetailData::CtDetailData()
{
	init();
}

CtDetailData::~CtDetailData()
{
	deInit();
}

void CtDetailData::deInit()
{
	SAFE_FREE(__displayName);
	SAFE_FREE(__imagePath);
	SAFE_FREE(__vibration);
	SAFE_FREE(__messageAlert);
	SAFE_FREE(__nickName);
	SAFE_FREE(__phoneticName);
	SAFE_FREE(__extraName);
	__freeRecordList(__numberList);
	__freeRecordList(__emailList);
	__freeRecordList(__addressList);
	__freeRecordList(__messengerList);
	__freeRecordList(__relationshipList);
	__freeRecordList(__urlList);
	__freeRecordList(__eventList);
	__freeRecordList(__noteList);
	__freeRecordList(__contactList);

	if (__formatter)
		i18n_udate_destroy (__formatter);
	if (__personRecord)
		contacts_record_destroy(__personRecord, true);

	int err	= contacts_db_remove_changed_cb(_contacts_person._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_remove_changed_cb() Failed(%d)", err);
}

void CtDetailData::init()
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;

	__link_count = 0;
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
	__listener = NULL;
	__favorite = false;

	// get person record
	if(__personRecord == NULL && __personId > 0)
		__personRecord = __getPerson(__personId);

	if(__personRecord && __personId == 0)
	{
		err = contacts_record_get_int(__personRecord, _contacts_person.id, &__personId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);
	}

	//get link count
	err = contacts_record_get_int(__personRecord, _contacts_person.link_count, &__link_count);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d)", err);

	// get contact list
	__contactList = ctCommonGetContactsList(__personId);

	// get basic info
	__getBasicInfo(__personRecord, __contactList);

	// get number list
	__numberList = __getChildDataList(__contactList, _contacts_contact.number);
	// get email list
	__emailList = __getChildDataList(__contactList, _contacts_contact.email);
	// get address list
	__addressList = __getChildDataList(__contactList, _contacts_contact.address);
	// get messenger list
	__messengerList = __getChildDataList(__contactList, _contacts_contact.messenger);
	// get url list
	__urlList = __getChildDataList(__contactList, _contacts_contact.url);
	// get relationship list
	__relationshipList = __getChildDataList(__contactList, _contacts_contact.relationship);
	// get event list
	__eventList = __getChildDataList(__contactList, _contacts_contact.event);
	// get note list
	__noteList = __getChildDataList(__contactList, _contacts_contact.note);


	// set db change noti
	contacts_db_get_current_version(&__version);
	err = contacts_db_add_changed_cb(_contacts_person._uri, __onPersonChanged, this);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_add_changed_cb() Failed(%d)", err);

	__formatter = __initTimeFormatter();

	WLEAVE();
}

void CtDetailData::attachListener(ICtChangeListener* listener)
{
	WPRET_M(NULL == listener, "listener is NULL");
	__listener = listener;
}

void CtDetailData::detachListener()
{
	__listener = NULL;
}

int CtDetailData::getPersonId()
{
	return __personId;
}

Eina_List* CtDetailData::getContactsList()
{
	return __contactList;
}

int CtDetailData::__setImage(contacts_record_h record, char *image, int type)
{
	WDEBUG("image %s", image);
	int err = CONTACTS_ERROR_NONE;

	if(image && *image) {
		//for update
		free(__imagePath);
		__imagePath = strdup(image);
		err = contacts_record_set_str(record, _contacts_image.path, image);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_str() Failed(%d)", err);
		err = contacts_record_set_int(record, _contacts_image.type, type);
		WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_set_int() Failed(%d)", err);
	}
	return err;
}

int CtDetailData::__removeImage(contacts_record_h contactRecord, contacts_record_h imageRecord)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	//for update
	free(__imagePath);
	__imagePath = NULL;
	err = contacts_record_remove_child_record(contactRecord, _contacts_contact.image, imageRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_remove_child_record() Failed(%d)", err);

	err = contacts_record_destroy(imageRecord, true);
	WPRET_VM(CONTACTS_ERROR_NONE != err, err, "contacts_record_destroy() Failed(%d)", err);

	return err;
}


bool CtDetailData::__getHasReadOnlyMode(void)
{
	WHIT();
	int addressbookId = -1;
	//int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_EX_NONE;
	int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_NONE;
	int err = CONTACTS_ERROR_NONE;
	Eina_List *l;
	void* tmp = NULL;
	contacts_record_h recordContact = NULL;
	contacts_record_h recordAddressbook = NULL;

	WPRET_VM(NULL == __contactList, true, "listContacts is NULL");

	EINA_LIST_FOREACH(__contactList, l, tmp) {
		if (NULL == tmp) {
			continue;
		}
		recordContact = (contacts_record_h)tmp;

		err = contacts_record_get_int(recordContact, _contacts_contact.address_book_id, &addressbookId);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &recordAddressbook);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_db_get_record() Failed(%d)", err);

		err = contacts_record_get_int(recordAddressbook, _contacts_address_book.mode, &addressbookMode);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_destroy(recordAddressbook, true);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_destroy() Failed(%d)", err);

//		if (CONTACTS_ADDRESS_BOOK_MODE_EX_READONLY & addressbookMode) {
		if (CONTACTS_ADDRESS_BOOK_MODE_READONLY & addressbookMode) {
			return true;
		}
	}
	return false;
}


int CtDetailData::setFavorite(bool favorite)
{
	WDEBUG("favorite %d", favorite);
	int err = CONTACTS_ERROR_NONE;
	__favorite = favorite;

	if(__getHasReadOnlyMode())
	{
		err = contacts_record_set_bool(__personRecord, _contacts_person.is_favorite, __favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_bool() Failed(%d)", err);
		err = contacts_db_update_record(__personRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);
		return err;
	}

	void *data = NULL;
	Eina_List *cursor = NULL;
	EINA_LIST_FOREACH(__contactList, cursor, data) {
		contacts_record_h contactRecord = (contacts_record_h)data;

		err = contacts_record_set_bool(contactRecord, _contacts_contact.is_favorite, __favorite);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_set_bool() Failed(%d)", err);
		err = contacts_db_update_record(contactRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_update_record() Failed(%d)", err);
	}

	return err;
}

bool CtDetailData::getFavorite()
{
	return __favorite;
}
/*
bool CtDetailData::getMFC()
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
		contacts_record_h recordPerson = NULL;
		int mfcIndex = 1;

		while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(personUsageList, &recordPerson)) {
			if(mfcIndex >5)
				break;
			int personId = -1;
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
const char* CtDetailData::getImagePath()
{
	return __imagePath;
}

const char* CtDetailData::getDisplayName()
{
	return __displayName;
}

const char* CtDetailData::getExtraName()
{
	return __extraName;
}

const char* CtDetailData::getPhoneticName()
{
	return __phoneticName;
}

const char* CtDetailData::getNickName()
{
	return __nickName;
}

const char* CtDetailData::getCompany()
{
	return __company;
}

const char* CtDetailData::getMessageAlert()
{
	return __messageAlert;
}

const char* CtDetailData::getVibration()
{
	return __vibration;
}


Eina_List* CtDetailData::getNumberList()
{
	return __numberList;
}

Eina_List* CtDetailData::getEmailList()
{
	return __emailList;
}

Eina_List* CtDetailData::getAddressList()
{
	return __addressList;
}

Eina_List* CtDetailData::getMessengerList()
{
	return __messengerList;
}

Eina_List* CtDetailData::getUrlList()
{
	return __urlList;
}

Eina_List* CtDetailData::getRelationshipList()
{
	return __relationshipList;
}

Eina_List* CtDetailData::getEventList()
{
	return __eventList;
}

Eina_List* CtDetailData::getNoteList()
{
	return __noteList;
}

const char* CtDetailData::getNumber(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_number.number, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtDetailData::getNumberType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_number.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, CONTACTS_NUMBER_TYPE_OTHER, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtDetailData::getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString)
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

const char* CtDetailData::getNumberTypeCustom(contacts_record_h record)
{
	char* type = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_number.label, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtDetailData::getEmail(contacts_record_h record)
{
	WHIT();
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_email.email, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtDetailData::getEmailType(contacts_record_h record)
{
	WHIT();
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_email.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, CONTACTS_EMAIL_TYPE_OTHER, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtDetailData::getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtDetailData::getAddress(contacts_record_h record, char *buf, int length)
{
	WHIT();
	return __getFullAddress(record, buf, length);
}


int CtDetailData::getAddressType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_address.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtDetailData::getDataTypeCustom(contacts_record_h record, int property_id)
{
	char* typeStr = NULL;
	const char* result = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, property_id, &typeStr);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_int() Failed(%d)", err);

	result = typeStr;
	return result;
}

void CtDetailData::getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

const char* CtDetailData::getUrl(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_url.url, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtDetailData::getUrlType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_url.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

const char* CtDetailData::getMessenger(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_messenger.im_id, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtDetailData::getMessengerType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_messenger.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtDetailData::getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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

int CtDetailData::getEvent(contacts_record_h record)
{
	int date = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_event.date, &date);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return date;
}

int CtDetailData::getEventType(contacts_record_h record)
{
	int type = 0;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_int(record, _contacts_event.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtDetailData::getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString)
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

const char* CtDetailData::getRelationship(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_relationship.name, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}

int CtDetailData::getRelationshipType(contacts_record_h record)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	int type = 0;

	err = contacts_record_get_int(record, _contacts_relationship.type, &type);
	WPRET_VM(CONTACTS_ERROR_NONE != err, 0, "contacts_record_get_int() Failed(%d)", err);

	return type;
}

void CtDetailData::getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString)
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


const char* CtDetailData::getNote(contacts_record_h record)
{
	char *value = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str_p(record, _contacts_note.note, &value);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_record_get_str_p() Failed(%d)", err);

	return value;
}


void CtDetailData::__onPersonChanged(const char *viewUri, void *userData)
{
	WHIT();

	WPRET_M(NULL == userData, "userData is NULL");
	CtDetailData* data = (CtDetailData*)userData;
	int err = CONTACTS_ERROR_NONE;
	Eina_List *cursor = NULL;
	void *record = NULL;
	int link_count = 0;
	int version = 0;
	bool matched = false;

	if(data->__personRecord) {
		contacts_record_h personRecord = NULL;
		err = contacts_db_get_record(_contacts_person._uri, data->__personId, &personRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_record() Failed(%d) id", err);
		err = contacts_record_get_int(personRecord, _contacts_person.link_count, &link_count);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d) id", err);
	}
	if(data->__link_count != link_count)
		matched = true;

	EINA_LIST_FOREACH(data->__contactList, cursor, record) {
		contacts_record_h contactRecord = (contacts_record_h)record;
		int addressbookId = 0;
		int contactId = 0;
		contacts_list_h chagnedList = NULL;
		contacts_record_h changedRecord = NULL;

		err = contacts_record_get_int(contactRecord, _contacts_contact.address_book_id, &addressbookId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d) address_book_id", err);

		err = contacts_record_get_int(contactRecord, _contacts_contact.id, &contactId);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_int() Failed(%d) id", err);

 		err = contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, addressbookId, data->__version, &chagnedList, &version);
 		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_changes_by_version() Failed(%d)", err);

		while (CONTACTS_ERROR_NONE == contacts_list_get_current_record_p(chagnedList, &changedRecord)) {
			int changedId = 0;
			err = contacts_record_get_int(changedRecord, _contacts_contact_updated_info.contact_id, &changedId);
			if(contactId == changedId) {
				WDEBUG("update matched");
				matched = true;
				break;
			}
			contacts_list_next(chagnedList);
		}

		err = contacts_list_destroy(chagnedList, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy(out_change_list) Failed(%d)", err);

		if(matched == true) {
			break;
		}
	}

	if(version > 0)
		data->__version = version;

	if(matched && data->__listener)
		data->__listener->onDbChanged(viewUri);
}



void CtDetailData::__freeRecordList(Eina_List *recordList)
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


contacts_record_h CtDetailData::__getPerson(int personId)
{
	WHIT();
	contacts_record_h personRecord = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
	WPRET_VM(CONTACTS_ERROR_NONE != err, NULL, "contacts_db_get_record() Failed(%d)", err);

	return personRecord;
}

void CtDetailData::__getBasicInfo(contacts_record_h personRecord, Eina_List *contactList)
{
	WENTER();
	int err = CONTACTS_ERROR_NONE;
	err = contacts_record_get_str(personRecord, _contacts_person.display_name, &__displayName);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str display_name %d", err);
	}
	err = contacts_record_get_str(personRecord, _contacts_person.image_thumbnail_path, &__imagePath);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str image_thumbnail_path %d", err);
	}
	err = contacts_record_get_str(personRecord, _contacts_person.vibration, &__vibration);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str vibration %d", err);
	}
	err = contacts_record_get_str(personRecord, _contacts_person.message_alert, &__messageAlert);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str message_alert %d", err);
	}
	err = contacts_record_get_bool(personRecord, _contacts_person.is_favorite, &__favorite);
	if(err != CONTACTS_ERROR_NONE) {
		WERROR("contacts_record_get_str is_favorite %d", err);
	}

	ctCommonRemoveNewline(__displayName);

	__getCompany(contactList, &__company);

	__getExtraName1(contactList, &__nickName, &__phoneticName);

	__getExtraName2(__nickName, __phoneticName, &__extraName);

	WLEAVE();
}

void CtDetailData::__getCompany(Eina_List *contactList, char **dest)
{
	WHIT();
	int err = CONTACTS_ERROR_NONE;
	void *data;
	contacts_record_h companyRecord = NULL;
	Eina_List *l;
	WPRET_M(NULL == contactList, "contactList is NULL");

	EINA_LIST_FOREACH(contactList, l, data) {
		contacts_record_h contactRecord = (contacts_record_h)data;
		if (NULL == contactRecord)
			continue;
		err = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.company, 0, &companyRecord);
		if(CONTACTS_ERROR_NONE != err)
			WERROR("contacts_record_get_child_record_at_p() Failed(%d)", err);
		else {
			__getCompanyStr(companyRecord, dest);
			break;
		}
	}
}

void CtDetailData::__getExtraName2(char* nick, char* phonetic, char** extra)
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


void CtDetailData::__getExtraName1(Eina_List *contactList, char **nick, char **phonetic)
{
	WHIT();
	int err1 = CONTACTS_ERROR_NONE;
	int err2 = CONTACTS_ERROR_NONE;
	void *data;
	contacts_record_h nameRecord = NULL;
	contacts_record_h nicknameRecord = NULL;
	Eina_List *l;

	WPRET_M(NULL == contactList, "contact_list is NULL");

	EINA_LIST_FOREACH(contactList, l, data) {
		contacts_record_h contactRecord = (contacts_record_h)data;
		if (NULL == contactRecord)
			continue;

		err1 = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.name, 0, &nameRecord);
		WPWARN(CONTACTS_ERROR_NONE != err1, "contacts_record_get_child_record_at_p() name Failed(%d)", err1);
		err2 = contacts_record_get_child_record_at_p(contactRecord, _contacts_contact.nickname, 0, &nicknameRecord);
		WPWARN(CONTACTS_ERROR_NONE != err2, "contacts_record_get_child_record_at_p() nickname Failed(%d)", err2);

		if (CONTACTS_ERROR_NONE == err1 || CONTACTS_ERROR_NONE == err2) {
			__getExtraNameStr(nameRecord, nicknameRecord, nick, phonetic);
			break;
		}
	}
}

void CtDetailData::__getExtraNameStr(contacts_record_h nameRecord, contacts_record_h nicknameRecord, char **nick, char **phonetic)
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

void CtDetailData::__getCompanyStr(contacts_record_h companyRecord, char **dest)
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

bool CtDetailData::__isDataExist(Eina_List *dataList, contacts_record_h record, unsigned int propertyId)
{
	WHIT();
	Eina_List *l;
	char *originText = NULL;
	char *tempText = NULL;
	int err = CONTACTS_ERROR_NONE;
	void* data;

	unsigned int dataPropertyId;

	if(propertyId == _contacts_contact.number)
		dataPropertyId = _contacts_number.number;
	else if(propertyId == _contacts_contact.email)
		dataPropertyId = _contacts_email.email;
	else if(propertyId == _contacts_contact.messenger)
		dataPropertyId = _contacts_messenger.im_id;
	else if(propertyId == _contacts_contact.url)
		dataPropertyId = _contacts_url.url;
	else if(propertyId == _contacts_contact.relationship)
		dataPropertyId = _contacts_relationship.name;
	else if(propertyId == _contacts_contact.note)
		dataPropertyId = _contacts_note.note;
	else {
		WERROR("cannot handle %d", propertyId);
		return false;
	}

	err = contacts_record_get_str_p(record, dataPropertyId, &originText);
	WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_str_p() Failed(%d)", err);

	EINA_LIST_FOREACH(dataList, l, data) {
		contacts_record_h tempRecord = (contacts_record_h)data;
		if (NULL == tempRecord)
			continue;

		err = contacts_record_get_str_p(tempRecord, dataPropertyId, &tempText);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_str_p() Failed(%d)", err);

		if (NULL == tempText)
			continue;

		if (0 == strcmp(originText, tempText))
			return true;
	}
	return false;
}

bool CtDetailData::__isAddressExist(Eina_List *dataList, contacts_record_h record)
{
	WHIT();
	Eina_List *l;
	void *data;
	char originAddress[CT_TEXT_MAX_LEN] = {0,};

	__getFullAddress(record, originAddress, sizeof(originAddress));

	EINA_LIST_FOREACH(dataList, l, data) {
		char tempAddress[CT_TEXT_MAX_LEN] = {0,};
		contacts_record_h tempRecord = (contacts_record_h)data;
		if (NULL == tempRecord)
			continue;

		__getFullAddress(tempRecord, tempAddress, sizeof(tempAddress));
		if (0 == strcmp(tempAddress, originAddress)) {
			return true;
		}
	}
	return false;
}

bool CtDetailData::__isEventExist(Eina_List *dataList, contacts_record_h record)
{
	WHIT();
	Eina_List *l;
	int originDate;
	int tempDate;
	int err = CONTACTS_ERROR_NONE;
	void *data;

	err = contacts_record_get_int(record, _contacts_event.date, &originDate);
	WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

	EINA_LIST_FOREACH(dataList, l, data) {
		contacts_record_h tempRecord = (contacts_record_h)data;
		if (NULL == tempRecord)
			continue;

		err = contacts_record_get_int(tempRecord, _contacts_event.date, &tempDate);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		if(originDate == tempDate)
			return true;

	}
	return false;
}

void CtDetailData::__appendStringWithLinebreak(const char *src, char *dest, int length)
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

int CtDetailData::__getFullAddress(contacts_record_h addresssRecord, char *buf, int length)
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

Eina_List* CtDetailData::__getChildDataList(Eina_List *contactList, unsigned int propertyId)
{
	WDEBUG("propertyId %d", propertyId);
	Eina_Bool checkduplicate = false;
	int index = 0;
	Eina_List *l;
	Eina_List *dataRecordList = NULL;
	int err = CONTACTS_ERROR_NONE;
	contacts_record_h dataRecord = NULL;
	contacts_record_h dataRecordClone = NULL;
	void *data = NULL;

	EINA_LIST_FOREACH(contactList, l, data) {
		contacts_record_h record = (contacts_record_h)data;
		index = 0;
		while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(record, propertyId, index++, &dataRecord)) {
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_record_get_str_p() Failed(%d)", err);
				break;
			}

			if (checkduplicate) {
				if(propertyId == _contacts_contact.address) {
					if (__isAddressExist(dataRecordList, dataRecord) == true) {
						WERROR("__isDataExist() return true %d", propertyId);
						continue;
					}
				}
				else if(propertyId == _contacts_contact.event) {
					if (__isEventExist(dataRecordList, dataRecord) == true) {
						WERROR("__isDataExist() return true %d", propertyId);
						continue;
					}
				}
				else {
					if (__isDataExist(dataRecordList, dataRecord, propertyId) == true) {
						WERROR("__isDataExist() return true %d", propertyId);
						continue;
					}
				}

			}
			err = contacts_record_clone(dataRecord, &dataRecordClone);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_list_get_current_record_p() Failed(%d)", err);
				break;
			}
			dataRecordList = eina_list_append(dataRecordList, dataRecordClone);
		}
		checkduplicate = true;
	}

	WDEBUG("count %d", eina_list_count(dataRecordList));

	return dataRecordList;
}

i18n_udate_format_h CtDetailData::__initTimeFormatter()
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


char* CtDetailData::getDatetimeStr(i18n_udate_format_h formatter, int year, int mon, int day)
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

i18n_udate_format_h CtDetailData::getDateFormater()
{
	return __formatter;
}


Eina_List* CtDetailData::getContactsWithoutReadOnly()
{
	WHIT();
	int addressbookId = -1;
	//int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_EX_FACEBOOK;
	int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_READONLY;
	int err = CONTACTS_ERROR_NONE;
	Eina_List *l;
	void* tmp = NULL;
	contacts_record_h recordContact = NULL;
	contacts_record_h contactRecordClone = NULL;
	contacts_record_h recordAddressbook = NULL;
	Eina_List *outList = NULL;

	WPRET_VM(NULL == __contactList, NULL, "listContacts is NULL");

	EINA_LIST_FOREACH(__contactList, l, tmp) {
		if (NULL == tmp) {
			continue;
		}
		recordContact = (contacts_record_h)tmp;

		err = contacts_record_get_int(recordContact, _contacts_contact.address_book_id, &addressbookId);
		if(CONTACTS_ERROR_NONE != err)
		{
			WWARNING("contacts_record_get_int() Failed(%d)", err);
			continue;
		}
		err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &recordAddressbook);
		if(CONTACTS_ERROR_NONE != err)
		{
			WWARNING("contacts_record_get_int() Failed(%d)", err);
			contacts_record_destroy(recordAddressbook, true);
			continue;
		}

		err = contacts_record_get_int(recordAddressbook, _contacts_address_book.mode, &addressbookMode);
		if(CONTACTS_ERROR_NONE != err)
		{
			WWARNING("contacts_record_get_int() Failed(%d)", err);
			contacts_record_destroy(recordAddressbook, true);
			continue;
		}

		err = contacts_record_destroy(recordAddressbook, true);
		if(CONTACTS_ERROR_NONE != err)
			WWARNING("contacts_record_destroy() Failed(%d)", err);

//		if (CONTACTS_ADDRESS_BOOK_MODE_EX_READONLY & addressbookMode) {
			if (CONTACTS_ADDRESS_BOOK_MODE_READONLY & addressbookMode) {
			continue;
		}
		err = contacts_record_clone(recordContact, &contactRecordClone);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_record_clone() Failed(%d)", err);
				continue;
			}
		outList = eina_list_append(outList, contactRecordClone);
	}
	return outList;
}

bool CtDetailData::getReadOnlyMode()
{
	WHIT();
	int addressbookId = -1;
//	int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_EX_FACEBOOK;
	int addressbookMode = CONTACTS_ADDRESS_BOOK_MODE_READONLY;
	int err = CONTACTS_ERROR_NONE;
	Eina_List *l;
	void* tmp = NULL;
	contacts_record_h recordContact = NULL;
	contacts_record_h recordAddressbook = NULL;

	WPRET_VM(NULL == __contactList, true, "listContacts is NULL");

	EINA_LIST_FOREACH(__contactList, l, tmp) {
		if (NULL == tmp) {
			continue;
		}
		recordContact = (contacts_record_h)tmp;

		err = contacts_record_get_int(recordContact, _contacts_contact.address_book_id, &addressbookId);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_db_get_record(_contacts_address_book._uri, addressbookId, &recordAddressbook);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_db_get_record() Failed(%d)", err);

		err = contacts_record_get_int(recordAddressbook, _contacts_address_book.mode, &addressbookMode);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_get_int() Failed(%d)", err);

		err = contacts_record_destroy(recordAddressbook, true);
		WPRET_VM(CONTACTS_ERROR_NONE != err, false, "contacts_record_destroy() Failed(%d)", err);

//		if (CONTACTS_ADDRESS_BOOK_MODE_EX_READONLY & addressbookMode)
		if (CONTACTS_ADDRESS_BOOK_MODE_READONLY & addressbookMode)
			continue;
		return false;
	}
	return true;
}

bool CtDetailData::isCurrentPersonValid()
{
	WHIT();

	int personId = getPersonId();

	return ctCommonIsValidPerson(personId);
}

char* CtDetailData::getVcardStream() const
{
	char* vcardStream = NULL;
	int err = CONTACTS_ERROR_NONE;
	err = contacts_vcard_make_from_person(__personRecord, &vcardStream);
	WPRET_VM(err != CONTACTS_ERROR_NONE, NULL, "contacts_vcard_make_from_person() Failed(%d)", err);

	return vcardStream;
}
