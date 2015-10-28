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

#ifndef _CT_CONTACTS_TO_VCARD_H_
#define _CT_CONTACTS_TO_VCARD_H_

#include <string>
#include <vector>

#include "CtVcardCommon.h"
#include "CtProgressController.h"

class CtContactsToVcard : public CtProgressController
{
public:

	/**
	 * @brief Constructor.
	 * @param[in]   title           Progress popup title.
	 * @param[in]   vcardStorage    Type of storage where vcard will be created.
	 * @param[in]   personIdList    Person id list.
	 */
	CtContactsToVcard(const char *title, TargetStorage vcardStorage, std::vector<int> personIdList);

	/**
	 * @brief Get vcard path.
	 * @return Vcard path.
	 */
	std::string getVcardPath();

	/**
	 * @brief Get total count.
	 * @return Total count.
	 */
	int getTotalCount();

private:
	void createDirectory();
	void vcardFilePathGet();

	void onStart(Ecore_Thread *thread);
	void onCanceled();

private:
	std::vector<int> m_PersonIdList;
	std::string m_VcardPath;
	TargetStorage m_VcardStorage;
};

#endif /* _CT_CONTACTS_TO_VCARD_H_ */
