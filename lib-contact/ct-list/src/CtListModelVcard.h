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

#ifndef __CT_LIST_MODEL_VCARD_H_
#define __CT_LIST_MODEL_VCARD_H_

#include <vector>
#include <string>
#include <functional>
#include <contacts.h>
#include <memory>
#include <Ecore.h>
#include <Elementary.h>

#include "CtCommon.h"
#include "ContactsCommon.h"
#include "CtListModel.h"
#include "CtListDataPerson.h"

class ICtChangeListener;

/**
 * @brief Provides access to the vcard contacts data list.
 */
class CtListModelVcard : public CtListModel
{
public:
	/**
	 * @brief Creates vcard list model.
	 *
	 * @param[in]   vcardPath   The vcard path for which list model will be created
	 */
	CtListModelVcard(const std::string &vcardPath);

	virtual ~CtListModelVcard();

public:
	// CtListModel interfaces
	/**
	 * @see CtListModel::getContactsCount()
	 */
	virtual int getContactsCount(CtListQueryType queryType);

	/**
	 * @see CtListModel::getContacts()
	 */
	virtual int getContacts(CtListQueryType queryType, CtGetContactsType syncType);

	/**
	 * @see CtListModel::getAtPersonData()
	 */
	virtual std::shared_ptr<CtListDataPerson> getAtPersonData(int index, CtListQueryType queryType);

	/**
	 * @see CtListModel::getPersonListCount()
	 */
	virtual int getPersonListCount(CtListQueryType queryType);

	/**
	 * @see CtListModel::attachListener()
	 */
	virtual void attachListener(ICtChangeListener *listener);

	/**
	 * @brief Sets callback which will be called when all contacts will be loaded from vcard file.
	 *
	 * @param[in]   loadingCb   The callback
	 */
	void setOnLoadCompleteCb(std::function<void (int, int, bool)> loadingCb);

private:
	void __getVcardPathFromUri(void);
	int __makePersonListFromVcard();
	static bool __onEachContactInVcard(contacts_record_h record, void *data);

private:
	ICtChangeListener* __listener;
	Ecore_Thread* __parser;
	bool __nQuery;
	std::string __vcardPath;
	std::vector<std::shared_ptr<CtListDataPerson>> __personDataList;
	std::function<void (int count, int total, bool)> __loadCompleteCb;
	int __loadedCount;
	int __totalCount;
	int __realTotalCount;
	bool __onTerminating;
};

#endif

