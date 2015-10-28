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

#ifndef _CT_MYPROFILE_INPUT_DATA_H_
#define _CT_MYPROFILE_INPUT_DATA_H_

#include <Eina.h>

#include <contacts.h>

#include "ContactsCommon.h"
#include "CtData.h"

class CtMyProfileInputData:public CtData
{
public:
	CtMyProfileInputData(contacts_record_h contactRecord);
	CtMyProfileInputData();
	virtual ~CtMyProfileInputData();
	contacts_record_h  addGroupRelation(int groupId){ return NULL; };

public:

	virtual int insertContact(int *contactId);
	virtual int updateContact();

	virtual int separateFullName(contacts_record_h nameRecord, const char *full_name);
	virtual int separatePhoneticFullName(contacts_record_h nameRecord, const char *full_name);
	virtual int setPrefixName(const char *text);
	virtual int setSuffixName(const char *text);
	virtual int setPhoneticFullName(const char *text);
	virtual int setPhoneticFirstName(const char *text);
	virtual int setPhoneticMiddleName(const char *text);
	virtual int setPhoneticLastName(const char *text);
	virtual int setNickname(const char *text);
	virtual int setCompany(const char *text);
	virtual int setJobTitle(const char *text);
	virtual int setFullName(const char *text);
	virtual int setFirstName(const char *text);
	virtual int setMiddleName(const char *text);
	virtual int setLastName(const char *text);
	virtual const char* getPrefixName();
	virtual const char* getSuffixName();
	virtual const char* getPhoneticFirstName();
	virtual const char* getPhoneticMiddleName();
	virtual const char* getPhoneticLastName();
	virtual const char* getNickname();
	virtual const char* getCompany();
	virtual const char* getJobTitle();
	virtual void getFullName(std::string &Name);
	virtual const char* getFirstName();
	virtual const char* getMiddleName();
	virtual const char* getLastName();
	// ...
	virtual bool getFavorite(){return false;}
	//virtual bool getMFC(){return false;}
	virtual int getPersonId(){return 0;}

	virtual int getAddressbookId();
	virtual const char* getAddressbookName();
	virtual const char* getUserName();
	virtual void setAddressbookInfo(int addressbookId);

	virtual const char* getImagePath();
	virtual contacts_record_h getImageRecord();
	virtual int removeImage(contacts_record_h record);
	virtual int setImage(contacts_record_h record, char *image, int type);
	virtual void setPreferenceCreateFirst(bool isFirst);

	virtual contacts_record_h getEmergencyNumber();
	virtual Eina_List* getNumberList();
	virtual contacts_record_h addNumber(const char *number);
	virtual int setNumberNumber(contacts_record_h record, const char *number);
	virtual int setNumberType(contacts_record_h record, int type, const char *customStr);
	virtual int removeNumber(contacts_record_h record);
	virtual const char* getNumberNumber(contacts_record_h record);
	virtual int getNumberType(contacts_record_h record);
	virtual const char *getDataTypeCustom(contacts_record_h record, int property_id);
	virtual void getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString);
	virtual int getNumberRetType(int indexType);
	virtual int getNumberIndexType(int numberType);
	virtual int getNextNumberType();

	virtual Eina_List* getEmailList();
	virtual contacts_record_h addEmail(const char *email);
	virtual int setEmailEmail(contacts_record_h record, const char *email);
	virtual int setEmailType(contacts_record_h record, int type, char *customStr);
	virtual const char* getEmailEmail(contacts_record_h record);
	virtual int getEmailType(contacts_record_h record);
	virtual void getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	virtual int getNextEmailType();
	virtual int removeEmail(contacts_record_h record);

	virtual Eina_List* getNoteList();
	virtual contacts_record_h getNoteRecord();
	virtual contacts_record_h addNote(const char *note);
	virtual int setNote(contacts_record_h record, const char *note);
	virtual const char* getNote(contacts_record_h record);
	virtual int removeNote(contacts_record_h record);

	virtual Eina_List* getAddressList();
	virtual contacts_record_h addAddress(const char *address);
	virtual int setAddressAddress(contacts_record_h record, const char *address);
	virtual int setAddressType(contacts_record_h record, int type, const char *customStr);
	virtual int getAddressAddress(contacts_record_h record, char *buf, int length);
	virtual int getAddressType(contacts_record_h record);
	virtual void getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	virtual int getNextAddressType();
	virtual int removeAddress(contacts_record_h record);

	virtual Eina_List* getMessengerList();
	virtual contacts_record_h addMessenger(const char *messenger);
	virtual int setMessengerMessenger(contacts_record_h record, const char *messenger);
	virtual int setMessengerType(contacts_record_h record, int type, const char *customStr);
	virtual int getMessengerIndexType(int emailType);
	virtual int getMessengerRetType(int indexType);
	virtual const char* getMessengerMessenger(contacts_record_h record);
	virtual int getMessengerType(contacts_record_h record);
	virtual void getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	virtual int getNextMessengerType();
	virtual int removeMessenger(contacts_record_h record);

	virtual Eina_List* getWebsiteList();
	virtual contacts_record_h addWebsite(const char *url);
	virtual int setWebsiteWebsite(contacts_record_h record, const char *url);
	virtual const char* getWebsiteWebsite(contacts_record_h record);
	virtual int removeWebsite(contacts_record_h record);

	virtual Eina_List* getRelationshipList();
	virtual contacts_record_h addRelationship(const char *relationship);
	virtual int setRelationshipRelationship(contacts_record_h record, const char *relationship);
	virtual int setRelationshipType(contacts_record_h record, int type, const char *customStr);
	virtual const char* getRelationshipRelationship(contacts_record_h record);
	virtual int getRelationshipType(contacts_record_h record);
	virtual void getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	virtual int getNextRelationshipType();
	virtual int removeRelationship(contacts_record_h record);

	virtual Eina_List* getEventList();
	virtual contacts_record_h addEvent(int date);
	virtual int setEventEvent(contacts_record_h record, int date);
	virtual int setEventType(contacts_record_h record, int type, char *customStr);
	virtual int getEventEvent(contacts_record_h record);
	virtual int getEventType(contacts_record_h record);
	virtual void getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString);
	virtual int getNextEventType();
	virtual int removeEvent(contacts_record_h record);

	virtual int getContactId();

	virtual contacts_record_h getDefaultEmail();
	virtual contacts_record_h getDefaultNumber();
	virtual bool checkChangedTypeIsDeleted();
	virtual void attachListener(ICtChangeListener* listener);
	virtual void detachListener();
	void removeEmptyFieldRecord(int property_id, int property_id_str);

	// for check input addressbook item is added or not
	virtual void setInputAddressbookItem(bool added) {}
	virtual bool getInputAddressbookItem(void) {return false;}

private:
	void __init();
	void __appendStringWithLinebreak(const char *src, char *dest, int length);
	int __getFullAddress(contacts_record_h addresssRecord, char *buf, int length);
	Eina_List* __getChildDataList(contacts_record_h contactRecord, unsigned int propertyId);
	void __removeEmptyFiedls();

private:
	contacts_record_h __contactRecord;

	int __contactId;
};

#endif //_CT_MYPROFILE_INPUT_DATA_H_
