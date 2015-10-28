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

#ifndef _CT_MYPROFILE_DETAIL_DATA_H_
#define _CT_MYPROFILE_DETAIL_DATA_H_

#include <utils_i18n.h>
#include <contacts.h>

#include "ContactsCommon.h"

#include "CtType.h"

#include "CtDetailDataBase.h"
#include "ICtChangeListener.h"

class CtMyProfileDetailData : public CtDetailDataBase
{
public:
	CtMyProfileDetailData();
	virtual ~CtMyProfileDetailData();

public:

	void init();
	void deInit();

	int getPersonId();
	contacts_record_h getPersonRecord();
	Eina_List* getContactsList();
	int setFavorite(bool favorite);

	const char* getImagePath();
	const char* getDisplayName();
	const char* getExtraName();
	const char* getPhoneticName();
	const char* getNickName();
	const char* getCompany();
	const char* getMessageAlert();
	const char* getVibration();
	bool getFavorite();
	//bool getMFC();

	Eina_List* getNumberList();
	contacts_record_h getDefNumberRecord();
	const char* getNumber(contacts_record_h record);
	int getNumberType(contacts_record_h record);
	void getNumberTypeStr(contacts_record_h record, int numberType, std::string &typeString);
	const char* getNumberTypeCustom(contacts_record_h record);
	Eina_List* getEmailList();
	const char* getEmail(contacts_record_h record);
	int getEmailType(contacts_record_h record);
	void getEmailTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	Eina_List* getAddressList();
	int getAddress(contacts_record_h record, char *buf, int length);
	const char* getDataTypeCustom(contacts_record_h record, int property_id);
	int getAddressType(contacts_record_h record);
	void getAddressTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	Eina_List* getMessengerList();
	const char* getMessenger(contacts_record_h record);
	int getMessengerType(contacts_record_h record);
	void getMessengerTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	Eina_List* getUrlList();
	const char* getUrl(contacts_record_h record);
	int getUrlType(contacts_record_h record);
	Eina_List* getRelationshipList();
	const char* getRelationship(contacts_record_h record);
	int getRelationshipType(contacts_record_h record);
	void getRelationshipTypeStr(contacts_record_h record, int indexType, std::string &typeString);
	Eina_List* getEventList();
	int getEvent(contacts_record_h record);
	int getEventType(contacts_record_h record);
	void getEventTypeStr(contacts_record_h record, int eventType, std::string &typeString);
	Eina_List* getNoteList();
	const char* getNote(contacts_record_h record);
	char* getDatetimeStr(i18n_udate_format_h formatter, int year, int mon, int day);
	i18n_udate_format_h getDateFormater();
	bool getReadOnlyMode();

	void attachListener(ICtChangeListener* listener);
	void detachListener();

	char* getVcardStream() const;

private:
	static void __onPersonChanged(const char *viewUri, void *userData);
	void __freeRecordList(Eina_List *recordList);

	Eina_List* __getChildDataList(Eina_List *contactList, unsigned int propertyId);
	Eina_List* __getChildNumberList();
	contacts_record_h __getDefaultNumber();
	int __getFullAddress(contacts_record_h addresssRecord, char *buf, int length);
	void __appendStringWithLinebreak(const char *src, char *dest, int length);


	void __getBasicInfo(contacts_record_h personRecord, Eina_List *contactList);
	void __getCompany(Eina_List *contactList, char **dest);
	void __getExtraName1(Eina_List *contactList, char **nick, char **phonetic);
	void __getExtraName2(char* nick, char* phonetic, char** extra);
	void __getExtraNameStr(contacts_record_h nameRecord, contacts_record_h nicknameRecord, char **nick, char **phonetic);
	void __getCompanyStr(contacts_record_h companyRecord, char **dest);
	i18n_udate_format_h __initTimeFormatter();
private:
	int __personId;
	contacts_record_h __personRecord;
	contacts_record_h __defaultNumberRecord;

	char *__imagePath;
	char *__displayName;
	char *__extraName;
	char *__phoneticName;
	char *__nickName;
	char *__company;
	char *__messageAlert;
	char *__vibration;

	Eina_List *__contactList;
	Eina_List *__numberList;
	Eina_List *__emailList;
	Eina_List *__addressList;
	Eina_List *__messengerList;
	Eina_List *__urlList;
	Eina_List *__relationshipList;
	Eina_List *__eventList;
	Eina_List *__noteList;

	i18n_udate_format_h __formatter;

	ICtChangeListener* __listener;
};

#endif //_CT_MYPROFILE_DETAIL_DATA_H_
