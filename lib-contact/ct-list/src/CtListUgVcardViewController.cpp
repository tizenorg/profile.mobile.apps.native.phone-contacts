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

#include <vector>
#include <thread>
#include <Elementary.h>
#include <app.h>
#include <contacts.h>
#include <notification.h>

#include "WView.h"
#include "WNaviframe.h"
#include "WPopup.h"
#include "WToastPopup.h"

#include "ContactsDebug.h"
#include "CtType.h"
#include "CtListDataPerson.h"
#include "CtCommon.h"
#include "CtListView.h"
#include "CtListModelVcard.h"
#include "CtListUgVcardViewController.h"
#include "CtProgressBarPopup.h"
#include "CtVcardToContacts.h"


CtListUgVcardViewController::CtListUgVcardViewController(const std::string& vcardPath, CtListView *listView, CtListModelVcard* listData)
: __vcardPath(vcardPath)
, __listView(listView)
, __listModel(listData)
, __showLoadingPopup(false)
{
	__getVcardPathFromUri();

	__listModel->setOnLoadCompleteCb([this](int count, int total, bool showToast){
		if (showToast) {
			ecore_job_add([](void* data){
				WENTER();
				CtListUgVcardViewController* self = static_cast<CtListUgVcardViewController*>(data);
				WToastPopup* popup = new WToastPopup("Only first 100 contacts displayed.");
				self->__listView->attachPopup(popup);
				WLEAVE();
			}, this);
		}
	});
}

CtListUgVcardViewController::~CtListUgVcardViewController()
{
	WHIT();
}

void CtListUgVcardViewController::onSelectItem(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();
	elm_genlist_item_selected_set(selectedItem, EINA_FALSE);
}

void CtListUgVcardViewController::onSelectDone()
{
	WHIT();
	std::vector<std::string> vcards;
	vcards.push_back(__vcardPath);

	CtVcardToContacts *importer = new CtVcardToContacts(V_("IDS_PB_HEADER_IMPORT_CONTACTS_ABB2"), __listModel->getContactsCount(LIST_QUERY_DEFAULT), std::move(vcards));
	importer->setOnFinish(std::bind(&CtListUgVcardViewController::__finishImportCb, this, std::placeholders::_1));
	__listView->attachPopup(importer->getProgressPopup());
	__listView->attachHiddenPopup(importer->getCancelPopup());
	importer->run();
}

void CtListUgVcardViewController::onSelectCancel()
{
	WHIT();
	__listView->popOut();
}

void CtListUgVcardViewController::onSelectBack()
{
	WHIT();
}

void CtListUgVcardViewController::onSelectCheckAll(Elm_Object_Item *selectedItem, CtListView *view, CtCheckState checkState)
{
	WHIT();
}

void CtListUgVcardViewController::onImportFinished(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data)
{
	WHIT();
	CtListUgVcardViewController* c = static_cast<CtListUgVcardViewController*>(user_data);

	c->__listView->popOut();
}

void CtListUgVcardViewController::__getVcardPathFromUri(void)
{
	std::string prefix("file://");

	size_t position = 0;
	if ((position = __vcardPath.find(prefix)) != std::string::npos) {
		__vcardPath.erase(position, prefix.length());
	}

	WDEBUG("vcard path is %s", __vcardPath.c_str());
}

void CtListUgVcardViewController::__finishImportCb(CtProgressController &controller)
{
	WHIT();
	CtVcardToContacts &importer = static_cast<CtVcardToContacts&>(controller);
	int count = importer.getImportedContactsCount();

	if (count > 0) {
		int err = NOTIFICATION_ERROR_NONE;

		if (count == 1) {
			err = notification_status_message_post(V_("IDS_PB_TPOP_1_CONTACT_IMPORTED"));
		} else {
			char buf[CT_TEXT_SHORT_LEN] = { 0, };
			snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_TPOP_PD_CONTACTS_IMPORTED"), count);
			err = notification_status_message_post(buf);
		}

		if (err != NOTIFICATION_ERROR_NONE) {
			WERROR("notification_status_message_post: %d", err);
		}
	}
}
