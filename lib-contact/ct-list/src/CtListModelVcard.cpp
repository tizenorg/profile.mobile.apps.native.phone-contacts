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

#include <thread>
#include <contacts.h>

#include "ContactsDebug.h"
#include "ContactsCommon.h"
#include "CtListDataPerson.h"
#include "CtListModelVcard.h"

CtListModelVcard::CtListModelVcard(const std::string &vcardPath)
: __listener(NULL)
, __parser(NULL)
, __nQuery(false)
, __vcardPath(vcardPath)
, __loadedCount(0)
, __totalCount(-1)
, __realTotalCount(-1)
, __onTerminating(false)
{
	WENTER();

	__getVcardPathFromUri();
	getPersonListCount(LIST_QUERY_DEFAULT);
}

CtListModelVcard::~CtListModelVcard()
{
	WENTER();

	if (__parser) {
		__onTerminating = true;
		while (__parser) {
			sleep(1);
		}
	}

	__personDataList.clear();

}

int CtListModelVcard::getContactsCount(CtListQueryType queryType)
{
	return getPersonListCount(LIST_QUERY_DEFAULT);
}

int CtListModelVcard::getContacts(CtListQueryType queryType, CtGetContactsType syncType)
{
	WHIT();

	if(queryType == LIST_QUERY_DEFAULT) {
		if (__personDataList.size() == 0) {
			__makePersonListFromVcard();
		}

		return __personDataList.size();
	}
	else
		WERROR("wrong queryType %d", queryType);

	return 0;
}

std::shared_ptr<CtListDataPerson> CtListModelVcard::getAtPersonData(int index, CtListQueryType queryType)
{
	if(queryType == LIST_QUERY_DEFAULT) {
		if(index < (int)__personDataList.size()) {
			return __personDataList[index];
		}
		else {
			return NULL;
		}
	}
	else
		return NULL;
}

int CtListModelVcard::getPersonListCount(CtListQueryType queryType)
{
	WENTER();

	if (__totalCount == -1) {
		int err = contacts_vcard_get_entity_count(__vcardPath.c_str(), &__realTotalCount);
		if (err != CONTACTS_ERROR_NONE) {
			return 0;
		}

		__totalCount = __realTotalCount;

		// show maximum 100 contacts
		if (__totalCount > 100)
			__totalCount = 100;

		WDEBUG("real count %d", __realTotalCount);
	}

	return __totalCount;
}

void CtListModelVcard::attachListener(ICtChangeListener *listener)
{
	WHIT();
	__listener = listener;
}

int	CtListModelVcard::__makePersonListFromVcard()
{
	__nQuery = true;
	contacts_vcard_parse_to_contact_foreach(__vcardPath.c_str(), __onEachContactInVcard, this);
	__nQuery = false;

	WDEBUG("total: %d, loaded: %d", __totalCount, __loadedCount);
	if (__loadedCount == __totalCount)
	{
		return 0;
	}

	ecore_idler_add([](void* data)->Eina_Bool{
		WHIT();
		CtListModelVcard* self = static_cast<CtListModelVcard*>(data);

		self->__loadedCount = 0;
		self->__parser = ecore_thread_run([](void *data, Ecore_Thread *thread){ // run callback
			CtListModelVcard* self = static_cast<CtListModelVcard*>(data);

			contacts_vcard_parse_to_contact_foreach(self->__vcardPath.c_str(), __onEachContactInVcard, data);

		},[](void *data, Ecore_Thread *thread){ // end callback
			WHIT();
			CtListModelVcard* self = static_cast<CtListModelVcard*>(data);
			self->__parser = NULL;

			if (self->__onTerminating)
				return;

			WDEBUG("listener:%x", self->__listener);
			if (self->__listener) {
				self->__listener->onCompleteGetContacts(20); // nquery
			}
		},[](void *data, Ecore_Thread *thread){
			WHIT();
		}, self);

		return ECORE_CALLBACK_CANCEL;
	}, this);

	return 0;
}

bool CtListModelVcard::__onEachContactInVcard(contacts_record_h record, void *data)
{
	CtListModelVcard* self = static_cast<CtListModelVcard*>(data);
	if (self->__onTerminating) {
		WERROR("pending cancel statue...");
		return false;
	}

	self->__loadedCount++;

	if (self->__nQuery || self->__loadedCount > 20)
	{
		int err = CONTACTS_ERROR_NONE;
		contacts_record_h newRecord = NULL;
		err = contacts_record_clone(record, &newRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);
		self->__personDataList.push_back(std::make_shared<CtListDataPerson>(newRecord));
	}

	if (self->__nQuery)
	{
		if (self->__loadedCount == 20)
		{
			WERROR("__loadedCount 20");
			return false;
		}
	}

	if (self->__loadedCount == self->__totalCount) {
		self->__loadCompleteCb(self->__loadedCount, self->__totalCount, self->__realTotalCount > 100);
		return false;
	}

	return true;
}

void CtListModelVcard::__getVcardPathFromUri(void)
{
	size_t position = 0;
	std::string prefix("file://");

	if ((position = __vcardPath.find(prefix)) != std::string::npos) {
		__vcardPath.erase(position, prefix.length());
	}

	WDEBUG("vcard path is %s", __vcardPath.c_str());
}

void CtListModelVcard::setOnLoadCompleteCb(std::function<void (int, int, bool)> loadCompleteCb)
{
	__loadCompleteCb = loadCompleteCb;
}
