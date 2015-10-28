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

#ifndef _CT_DETAIL_DATA_BASE_H_
#define _CT_DETAIL_DATA_BASE_H_

#include <utils_i18n.h>

#include <contacts.h>

#include "ContactsCommon.h"

#include "CtType.h"

class CtDetailDataBase
{
public:
	virtual ~CtDetailDataBase(){};

public:

	virtual void init() = 0;
	virtual void deInit() = 0;

	virtual int getPersonId() = 0;
	virtual Eina_List* getContactsList() = 0;
	virtual int setFavorite(bool favorite) = 0;

	virtual const char* getImagePath() = 0;
	virtual const char* getDisplayName() = 0;
	virtual const char* getExtraName() = 0;
	virtual const char* getPhoneticName() = 0;
	virtual const char* getNickName() = 0;
	virtual const char* getCompany() = 0;
	virtual const char* getMessageAlert() = 0;
	virtual const char* getVibration() = 0;
	virtual bool getFavorite() = 0;
	//virtual bool getMFC() = 0;

	virtual Eina_List* getNumberList() = 0;
	virtual const char* getNumber(contacts_record_h record) = 0;
	virtual int getNumberType(contacts_record_h record) = 0;
	virtual void getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString) = 0;
	virtual const char* getNumberTypeCustom(contacts_record_h record) = 0;
	virtual Eina_List* getEmailList() = 0;
	virtual const char* getEmail(contacts_record_h record) = 0;
	virtual int getEmailType(contacts_record_h record) = 0;
	virtual void getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual Eina_List* getAddressList() = 0;
	virtual int getAddress(contacts_record_h record, char *buf, int length) = 0;
	virtual const char* getDataTypeCustom(contacts_record_h record, int property_id) = 0;
	virtual int getAddressType(contacts_record_h record) = 0;
	virtual void getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual Eina_List* getMessengerList() = 0;
	virtual const char* getMessenger(contacts_record_h record) = 0;
	virtual int getMessengerType(contacts_record_h record) = 0;
	virtual void getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual Eina_List* getUrlList() = 0;
	virtual const char* getUrl(contacts_record_h record) = 0;
	virtual int getUrlType(contacts_record_h record) = 0;
	virtual Eina_List* getRelationshipList() = 0;
	virtual const char* getRelationship(contacts_record_h record) = 0;
	virtual int getRelationshipType(contacts_record_h record) = 0;
	virtual void getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString) = 0;
	virtual Eina_List* getEventList() = 0;
	virtual int getEvent(contacts_record_h record) = 0;
	virtual int getEventType(contacts_record_h record) = 0;
	virtual void getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString) = 0;
	virtual Eina_List* getNoteList() = 0;
	virtual const char* getNote(contacts_record_h record) = 0;
	virtual char* getDatetimeStr(i18n_udate_format_h formatter, int year, int mon, int day) = 0;
	virtual i18n_udate_format_h getDateFormater() = 0;
	virtual bool getReadOnlyMode() = 0;
};

#endif //_CT_DETAIL_DATA_BASE_H_
