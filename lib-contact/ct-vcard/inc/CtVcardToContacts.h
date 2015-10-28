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

#ifndef _CT_VCARD_TO_CONTACTS_H_
#define _CT_VCARD_TO_CONTACTS_H_

#include <contacts.h>
#include <condition_variable>
#include <string>
#include <vector>

#include "CtProgressController.h"

class WPopup;
class CtVcardToContacts : public CtProgressController
{
public:
	/**
	 * @brief Constructor.
	 * @param[in]   title           Progress popup title.
	 * @param[in]   totalCount      Total count of contacts that will be imported.
	 * @param[in]   vcards          Vcards which will be imported.
	 */
	CtVcardToContacts(const char *title, int totalCount, std::vector<std::string> vcards);

	/**
	 * @brief Get imported contacts count.
	 * @return Count of imported contacts.
	 */
	int getImportedContactsCount();

	/**
	 * @brief Get Cancel popup.
	 * @return Cancel popup
	 */
	WPopup *getCancelPopup();

private:
	/**
	 * @brief Private destructor. Only parent class CtProgressController should call it.
	 */
	~CtVcardToContacts();

	void createCancelPopup();
	void continueImporting(bool *destroyPopup);
	void cancelImporting(bool *destroyPopup);

	void onStart(Ecore_Thread *thread);
	void onCanceled();
	bool onCancel();

	static bool vcardParseCb(contacts_record_h record, void *user_data);

private:
	WPopup *m_CancelPopup;
	std::condition_variable m_ContinueCondition;
	std::string m_CurrentImportingVcard;
	std::vector<int> m_ImportedContacts;
	bool m_Pause;
	Ecore_Thread *m_Thread;
	std::vector<std::string> m_Vcards;
};

#endif /* _CT_VCARD_TO_CONTACTS_H_ */
