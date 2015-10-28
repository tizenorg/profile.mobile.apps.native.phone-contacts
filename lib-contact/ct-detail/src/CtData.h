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

#ifndef _CT_DATA_H_
#define _CT_DATA_H_

#include <Eina.h>

#include <contacts.h>

#include "ContactsCommon.h"
#include "ICtChangeListener.h"

#include "CtType.h"

typedef enum {
	CTTEXT_TYPE_CUSTOM = 1,
	CTTEXT_TYPE_MOBILE,
	CTTEXT_TYPE_WORK,
	CTTEXT_TYPE_HOME,
	CTTEXT_TYPE_MAIN,
	CTTEXT_TYPE_FAX_WORK,
	CTTEXT_TYPE_FAX_HOME,
	CTTEXT_TYPE_PAGER,
	CTTEXT_TYPE_OTHER,
} CtInputNumberDataType;

typedef enum {
	CTTEXT_MESSENGER_TYPE_CUSTOM,
	CTTEXT_MESSENGER_TYPE_AIM,
	CTTEXT_MESSENGER_TYPE_YAHOO,
	CTTEXT_MESSENGER_TYPE_SKYPE,
	CTTEXT_MESSENGER_TYPE_QQ,
	CTTEXT_MESSENGER_TYPE_HANGOUTS,
	CTTEXT_MESSENGER_TYPE_ICQ,
	CTTEXT_MESSENGER_TYPE_JABBER,
} CtInputMessengerDataType;

enum {
	INPUT_ADDED_ITEM_INDEX_NONE = 0,
	INPUT_ADDED_ITEM_INDEX_PHONETIC_INFO,
	INPUT_ADDED_ITEM_INDEX_NICKNAME,
	INPUT_ADDED_ITEM_INDEX_ORGANIZATION,
	INPUT_ADDED_ITEM_INDEX_NUMBER,
	INPUT_ADDED_ITEM_INDEX_EMAIL,
	INPUT_ADDED_ITEM_INDEX_NOTE,
	INPUT_ADDED_ITEM_INDEX_URL,
	INPUT_ADDED_ITEM_INDEX_ADDRESS,
	INPUT_ADDED_ITEM_INDEX_IM,
	INPUT_ADDED_ITEM_INDEX_RELATIONSHIP,
};

class CtData
{
public:
	virtual ~CtData(){};
public:

	virtual int insertContact(int *contactId) = 0;
	virtual int updateContact() = 0;

	virtual int separateFullName(contacts_record_h nameRecord, const char *full_name) = 0;
	virtual int separatePhoneticFullName(contacts_record_h nameRecord, const char *full_name) = 0;
	virtual int setPrefixName(const char *text) = 0;
	virtual int setSuffixName(const char *text) = 0;
	virtual int setPhoneticFullName(const char *text) = 0;
	virtual int setPhoneticFirstName(const char *text) = 0;
	virtual int setPhoneticMiddleName(const char *text) = 0;
	virtual int setPhoneticLastName(const char *text) = 0;
	virtual int setNickname(const char *text) = 0;
	virtual int setCompany(const char *text) = 0;
	virtual int setJobTitle(const char *text) = 0;
	virtual int setFullName(const char *text) = 0;
	virtual int setFirstName(const char *text) = 0;
	virtual int setMiddleName(const char *text) = 0;
	virtual int setLastName(const char *text) = 0;
	virtual const char* getPrefixName() = 0;
	virtual const char* getSuffixName() = 0;
	virtual const char* getPhoneticFirstName() = 0;
	virtual const char* getPhoneticMiddleName() = 0;
	virtual const char* getPhoneticLastName() = 0;
	virtual const char* getNickname() = 0;
	virtual const char* getCompany() = 0;
	virtual const char* getJobTitle() = 0;
	virtual void getFullName(std::string &Name) = 0;
	virtual const char* getFirstName() = 0;
	virtual const char* getMiddleName() = 0;
	virtual const char* getLastName() = 0;
	// ...
	virtual bool getFavorite() =0;
	virtual int getPersonId() =0;
	//virtual bool getMFC()=0;

	virtual int getAddressbookId() = 0;
	virtual const char* getAddressbookName() = 0;
	virtual const char* getUserName() = 0;
	virtual void setAddressbookInfo(int addressbookId) = 0;
	virtual void setPreferenceCreateFirst(bool isFirst) = 0;

	virtual const char* getImagePath() = 0;
	virtual contacts_record_h getImageRecord() = 0;
	virtual int removeImage(contacts_record_h record) = 0;
	virtual int setImage(contacts_record_h record, char *image, int type) = 0;

	virtual contacts_record_h getEmergencyNumber() = 0;
	virtual Eina_List* getNumberList() = 0;
	virtual contacts_record_h addNumber(const char *number) = 0;
	virtual int setNumberNumber(contacts_record_h record, const char *number) = 0;
	virtual int setNumberType(contacts_record_h record, int type, const char *customStr) = 0;
	virtual int removeNumber(contacts_record_h record) = 0;
	virtual const char* getNumberNumber(contacts_record_h record) = 0;
	virtual int getNumberType(contacts_record_h record) = 0;
	virtual const char *getDataTypeCustom(contacts_record_h record, int property_id) = 0;
	virtual void getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString) = 0;
	virtual int getNumberRetType(int indexType) = 0;
	virtual int getNumberIndexType(int numberType) = 0;
	virtual int getNextNumberType() = 0;

	virtual Eina_List* getEmailList() = 0;
	virtual contacts_record_h addEmail(const char *email) = 0;
	virtual int setEmailEmail(contacts_record_h record, const char *email) = 0;
	virtual int setEmailType(contacts_record_h record, int type, char *customStr) = 0;
	virtual const char* getEmailEmail(contacts_record_h record) = 0;
	virtual int getEmailType(contacts_record_h record) = 0;
	virtual void getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual int getNextEmailType() = 0;
	virtual int removeEmail(contacts_record_h record) = 0;

	virtual Eina_List* getNoteList() = 0;
	virtual contacts_record_h getNoteRecord() = 0;
	virtual contacts_record_h addNote(const char *note) = 0;
	virtual int setNote(contacts_record_h record, const char *note) = 0;
	virtual const char* getNote(contacts_record_h record) = 0;
	virtual int removeNote(contacts_record_h record) = 0;

	virtual Eina_List* getAddressList() = 0;
	virtual contacts_record_h addAddress(const char *address) = 0;
	virtual int setAddressAddress(contacts_record_h record, const char *address) = 0;
	virtual int setAddressType(contacts_record_h record, int type, const char *customStr) = 0;
	virtual int getAddressAddress(contacts_record_h record, char *buf, int length) = 0;
	virtual int getAddressType(contacts_record_h record) = 0;
	virtual void getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual int getNextAddressType() = 0;
	virtual int removeAddress(contacts_record_h record) = 0;

	virtual Eina_List* getMessengerList() = 0;
	virtual contacts_record_h addMessenger(const char *messenger) = 0;
	virtual int setMessengerMessenger(contacts_record_h record, const char *messenger) = 0;
	virtual int setMessengerType(contacts_record_h record, int type, const char *customStr) = 0;
	virtual int getMessengerIndexType(int emailType) = 0;
	virtual int getMessengerRetType(int indexType) = 0;
	virtual const char* getMessengerMessenger(contacts_record_h record) = 0;
	virtual int getMessengerType(contacts_record_h record) = 0;
	virtual void getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual int getNextMessengerType() = 0;
	virtual int removeMessenger(contacts_record_h record) = 0;

	virtual Eina_List* getWebsiteList() = 0;
	virtual contacts_record_h addWebsite(const char *url) = 0;
	virtual int setWebsiteWebsite(contacts_record_h record, const char *url) = 0;
	virtual const char* getWebsiteWebsite(contacts_record_h record) = 0;
	virtual int removeWebsite(contacts_record_h record) = 0;

	virtual Eina_List* getRelationshipList() = 0;
	virtual contacts_record_h addRelationship(const char *relationship) = 0;
	virtual int setRelationshipRelationship(contacts_record_h record, const char *relationship) = 0;
	virtual int setRelationshipType(contacts_record_h record, int type, const char *customStr) = 0;
	virtual const char* getRelationshipRelationship(contacts_record_h record) = 0;
	virtual int getRelationshipType(contacts_record_h record) = 0;
	virtual void getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual int getNextRelationshipType() = 0;
	virtual int removeRelationship(contacts_record_h record) = 0;

	virtual Eina_List* getEventList() = 0;
	virtual contacts_record_h addEvent(int date) = 0;
	virtual int setEventEvent(contacts_record_h record, int date) = 0;
	virtual int setEventType(contacts_record_h record, int type, char *customStr) = 0;
	virtual int getEventEvent(contacts_record_h record) = 0;
	virtual int getEventType(contacts_record_h record) = 0;
	virtual void getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString) = 0;
	virtual int getNextEventType() = 0;
	virtual int removeEvent(contacts_record_h record) = 0;
	// ...


	virtual int getContactId() = 0;

	virtual contacts_record_h getDefaultEmail() = 0;
	virtual contacts_record_h getDefaultNumber() = 0;
	virtual bool checkChangedTypeIsDeleted() = 0;

	virtual void attachListener(ICtChangeListener* listener) = 0;
	virtual void detachListener() = 0;

	virtual contacts_record_h  addGroupRelation(int groupId) = 0;

	// for check input addressbook item is added or not
	virtual void setInputAddressbookItem(bool added) = 0;
	virtual bool getInputAddressbookItem(void) = 0;

};

#endif //_CT_DATA_H_
