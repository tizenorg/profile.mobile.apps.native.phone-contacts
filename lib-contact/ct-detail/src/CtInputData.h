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

#ifndef _CT_INPUT_DATA_H_
#define _CT_INPUT_DATA_H_

#include <Eina.h>

#include <contacts.h>

#include "ContactsCommon.h"
#include "CtData.h"

class CtInputData:public CtData
{
public:
	CtInputData(contacts_record_h contactRecord);
	CtInputData(int personId, const char *number, const char *email, const char *url);
	CtInputData(const char *name, const char *number, const char *email, const char *url);
	CtInputData();
	virtual ~CtInputData();

public:

	int insertContact(int *contactId);
	int updateContact();

	int separateFullName(contacts_record_h nameRecord, const char *full_name);
	int separatePhoneticFullName(contacts_record_h nameRecord, const char *full_name);
	int setPrefixName(const char *text);
	int setSuffixName(const char *text);
	int setPhoneticFullName(const char *text);
	int setPhoneticFirstName(const char *text);
	int setPhoneticMiddleName(const char *text);
	int setPhoneticLastName(const char *text);
	int setNickname(const char *text);
	int setCompany(const char *text);
	int setJobTitle(const char *text);
	int setFullName(const char *text);
	int setFirstName(const char *text);
	int setMiddleName(const char *text);
	int setLastName(const char *text);
	const char* getPrefixName();
	const char* getSuffixName();
	const char* getPhoneticFirstName();
	const char* getPhoneticMiddleName();
	const char* getPhoneticLastName();
	const char* getNickname();
	const char* getCompany();
	const char* getJobTitle();
	void getFullName(std::string &Name);
	const char* getFirstName();
	const char* getMiddleName();
	const char* getLastName();
	// ...

	int getAddressbookId() {return __baseAddressbookId;}
	const char* getAddressbookName() {return (const char*)__baseAddressbookName;}
	const char* getUserName() {return (const char*)__userName;}
	void setAddressbookInfo(int addressbookId);
	void setPreferenceCreateFirst(bool isFirst);

	bool getFavorite(){return __favorite;}
	//bool getMFC();
	int getPersonId(){return __personId;}
	const char* getImagePath();
	contacts_record_h getImageRecord();
	int removeImage(contacts_record_h record);
	int setImage(contacts_record_h record, char *image, int type);

	contacts_record_h getEmergencyNumber();
	Eina_List* getNumberList();
	contacts_record_h addNumber(const char *number);
	int setNumberNumber(contacts_record_h record, char const *number);
	int setNumberType(contacts_record_h record, int type, const char *customStr);
	int removeNumber(contacts_record_h record);
	const char* getNumberNumber(contacts_record_h record);
	contacts_record_h getDefaultEmail();
	contacts_record_h getDefaultNumber();
	int getNumberType(contacts_record_h record);
	const char *getDataTypeCustom(contacts_record_h record, int property_id);
	void getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString);
	int getNumberRetType(int indexType);
	int getNumberIndexType(int numberType);
	int getNextNumberType();

	Eina_List* getEmailList();
	Eina_List* getNoteList();
	contacts_record_h addEmail(const char *email);
	int setEmailEmail(contacts_record_h record, const char *email);
	int setEmailType(contacts_record_h record, int type, char *customStr);
	const char* getEmailEmail(contacts_record_h record);
	int getEmailType(contacts_record_h record);
	void getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	int getNextEmailType();
	int removeEmail(contacts_record_h record);

	contacts_record_h getNoteRecord();
	contacts_record_h addNote( const char *note);
	int setNote(contacts_record_h record, const char *note);
	const char* getNote(contacts_record_h record);
	int removeNote(contacts_record_h record);

	Eina_List* getAddressList();
	contacts_record_h addAddress(const char *address);
	int setAddressAddress(contacts_record_h record, const char *address);
	int setAddressType(contacts_record_h record, int type, const char *customStr);
	int getAddressAddress(contacts_record_h record, char *buf, int length);
	int getAddressType(contacts_record_h record);
	void getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	int getNextAddressType();
	int removeAddress(contacts_record_h record);

	Eina_List* getMessengerList();
	contacts_record_h addMessenger(const char *messenger);
	int setMessengerMessenger(contacts_record_h record, const char *messenger);
	int setMessengerType(contacts_record_h record, int type, const char *customStr);
	int getMessengerIndexType(int emailType);
	int getMessengerRetType(int indexType);
	const char* getMessengerMessenger(contacts_record_h record);
	int getMessengerType(contacts_record_h record);
	void getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	int getNextMessengerType();
	int removeMessenger(contacts_record_h record);

	Eina_List* getWebsiteList();
	contacts_record_h addWebsite(const char *url);
	int setWebsiteWebsite(contacts_record_h record, const char *url);
	const char* getWebsiteWebsite(contacts_record_h record);
	int removeWebsite(contacts_record_h record);

	Eina_List* getRelationshipList();
	contacts_record_h addRelationship(const char *relationship);
	int setRelationshipRelationship(contacts_record_h record, const char *relationship);
	int setRelationshipType(contacts_record_h record, int type, const char *customStr);
	const char* getRelationshipRelationship(contacts_record_h record);
	int getRelationshipType(contacts_record_h record);
	void getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	int getNextRelationshipType();
	int removeRelationship(contacts_record_h record);

	Eina_List* getEventList();
	contacts_record_h addEvent(int date);
	int setEventEvent(contacts_record_h record, int date);
	int setEventType(contacts_record_h record, int type, char *customStr);
	int getEventEvent(contacts_record_h record);
	int getEventType(contacts_record_h record);
	void getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString);
	int getNextEventType();
	int removeEvent(contacts_record_h record);
	void removeEmptyFieldRecord(int property_id, int property_id_str);

	int getContactId();


	bool checkChangedTypeIsDeleted();
	void attachListener(ICtChangeListener* listener);
	void detachListener();

	contacts_record_h  addGroupRelation(int groupId);

	// for check input addressbook item is added or not
	void setInputAddressbookItem(bool added);
	bool getInputAddressbookItem(void);

private:
	void __init();
	static void __onPersonChanged(const char *viewUri, void *userData);

	contacts_record_h __getInitContact(int personId, const char *number, const char *email, const char *url);
	contacts_record_h __getWritableContact(int personId);
	int __getBaseAddressbookId(void);

	void __appendStringWithLinebreak(const char *src, char *dest, int length);
	int __getFullAddress(contacts_record_h addresssRecord, char *buf, int length);
	Eina_List* __getChildDataList(contacts_record_h contactRecord, unsigned int propertyId);
	void __removeEmptyFiedls();
	void __setContactInfo(int personId);
	bool __hasSuffix(const char *src);

private:
	contacts_record_h __contactRecord;

	int __personId;
	bool __favorite;
	int __contactId;
	int __baseAddressbookId;
	char* __baseAddressbookName;
	char* __userName;
	int __currentVersion;
	bool __inputAddressbook;

	ICtChangeListener* __listener;
};

#endif //_CT_INPUT_DATA_H_
