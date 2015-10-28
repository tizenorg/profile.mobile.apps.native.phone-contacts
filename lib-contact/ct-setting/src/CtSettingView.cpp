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

#include <map>
#include <string>
#include <vector>

#include <app.h>
#include <contacts.h>
#include <notification.h>

#include "WNaviframe.h"
#include "WWindow.h"
#include "WPopup.h"
#include "WToastPopup.h"
#include "WGenlistItem.h"
#include "ContactsDebug.h"
#include "CtType.h"
#include "ContactsAppControl.h"

#include "CtContactsToVcard.h"
#include "CtString.h"
#include "CtSettingData.h"
#include "CtSettingView.h"
#include "CtSettingRadioPopup.h"
#include "CtVcardToContacts.h"

#define MYFILE_UG "ug-myfile-efl"

class __SortByItem: public WGenlistItem
{
public:
	__SortByItem( CtSettingData* data ) { __data = data; }
	virtual ~__SortByItem(){}
public:
	virtual Elm_Genlist_Item_Class* getItemClassNew() {
		Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
		WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
		itc->item_style = "type1";
		itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
		{
			__SortByItem* p = static_cast<__SortByItem*>(data);

			if(!strcmp(part, "elm.text")) {
				return strdup(V_("IDS_PB_MBODY_SORT_BY"));
			}
			else if(!strcmp(part, "elm.text.sub")){
				if ( p->__data->getSortMode() == CtSettingData::SORT_MODE_FIRST_NAME)
					return strdup(V_("IDS_PB_OPT_FIRST_NAME"));
				else
					return strdup(V_("IDS_PB_BODY_LAST_NAME_ABB"));
			}
			return NULL;
		};
		itc->func.del = WGENLIST_ITEM_DEL();
		return itc;
	}
private:
	__SortByItem();
	CtSettingData* __data;
};

class __DisplayNameByItem: public WGenlistItem
{
public:
	__DisplayNameByItem( CtSettingData* data ) { __data = data; }
	virtual ~__DisplayNameByItem(){}
public:
	virtual Elm_Genlist_Item_Class* getItemClassNew() {
		Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
		WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
		itc->item_style = "type1";
		itc->func.text_get = __getText;
		itc->func.del = WGENLIST_ITEM_DEL();
		return itc;
	}
private:
	static char* __getText(void* data, Evas_Object* obj, const char* part ) {
		__DisplayNameByItem* p = static_cast<__DisplayNameByItem*>(data);
		if(!strcmp(part, "elm.text")) {
			return strdup(V_("IDS_PB_TMBODY_NAME_FORMAT"));
		}
		else if(!strcmp(part, "elm.text.sub")) {
			if ( p->__data->getNameOrder() == CtSettingData::NAME_ORDER_FIRST_NAME_FIRST)
				return strdup(V_("IDS_PB_OPT_FIRST_NAME_FIRST_ABB"));
			else
				return strdup(V_("IDS_PB_OPT_LAST_NAME_FIRST_ABB"));
		}
		return NULL;
	}
private:
	__DisplayNameByItem();
	CtSettingData* __data;
};

CtSettingView::CtSettingView()
: __genlist(NULL)
, __settingData(NULL)
{
	WHIT();
}

CtSettingView::~CtSettingView()
{
	WHIT();

	delete __settingData;
}

Evas_Object* CtSettingView::onCreate( Evas_Object* parent, void* viewParam)
{
	WHIT();

	setTitle(V_("IDS_PB_HEADER_SETTINGS"));

	__settingData = new CtSettingData();
	__settingData->init();

	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(layout, "layout", "application", "default");

	__genlist = elm_genlist_add(parent);
	if (__genlist == NULL) {
		WERROR("elm_genlist_add failed");
		return NULL;
	}

	elm_genlist_mode_set(__genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(__genlist, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", __genlist);
	evas_object_show(layout);

	Elm_Genlist_Item_Class* itc = NULL;
	WGenlistItem* item = NULL;

	item = new __SortByItem(__getSettingData() );
	item->setElmObjectItem( elm_genlist_item_append(__genlist, itc = item->getItemClassNew(), item, NULL, ELM_GENLIST_ITEM_NONE, __onSortByItemSelectedCb, this) );
	elm_genlist_item_class_free(itc);

	item = new __DisplayNameByItem(__getSettingData() );
	item->setElmObjectItem( elm_genlist_item_append(__genlist, itc = item->getItemClassNew(), item, NULL, ELM_GENLIST_ITEM_NONE, __onDisplayNameByItemSelectedCb, this));
	elm_genlist_item_class_free(itc);

	itc = getOneLineItc();
	elm_genlist_item_append(__genlist, itc, V_("IDS_PB_MBODY_IMPORT"), NULL, ELM_GENLIST_ITEM_NONE, __onImportItemSelectedCb, this);
	elm_genlist_item_append(__genlist, itc, V_("IDS_PB_MBODY_EXPORT"), NULL, ELM_GENLIST_ITEM_NONE, __onExportItemSelectedCb, this);
	elm_genlist_item_class_free(itc);

	return layout;
}

void CtSettingView::onDestroy()
{
	WHIT();
	destroyPopup();
}

CtSettingData* CtSettingView::__getSettingData()
{
	return __settingData;
}

Elm_Genlist_Item_Class *CtSettingView::getOneLineItc()
{
	Elm_Genlist_Item_Class* itc = elm_genlist_item_class_new();
	WPRET_VM(!itc, NULL, "elm_genlist_item_class_new() failed");
	itc->item_style = "type1";
	itc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char* {
		if(!strcmp(part, "elm.text") && data) {
			return strdup((const char *)data);
		}
		return NULL;
	};
	return itc;
}

void CtSettingView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();

	elm_naviframe_item_title_enabled_set(naviItem, EINA_TRUE, EINA_TRUE);
}

void CtSettingView::__onSortByItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);
	CtSettingView* p = static_cast<CtSettingView*>(data);
	CtSettingData::SortMode sortMode = p->__getSettingData()->getSortMode();

	std::vector<std::string> label = {V_("IDS_PB_OPT_FIRST_NAME"), V_("IDS_PB_BODY_LAST_NAME_ABB")};

	Elm_Object_Item* objItem = (Elm_Object_Item *)event_info;
	CtSettingData* settingData = p->__getSettingData();

	CtSettingRadioPopup* popup = new CtSettingRadioPopup(label, sortMode == CtSettingData::SORT_MODE_FIRST_NAME ? 0 : 1);
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle( "IDS_PB_MBODY_SORT_BY");
	popup->setOnResultCb([popup,objItem, settingData](int index){
		WDEBUG("item %d selected", index);
		if (index == 0)
			settingData->setSortMode(CtSettingData::SORT_MODE_FIRST_NAME);
		else
			settingData->setSortMode(CtSettingData::SORT_MODE_LAST_NAME);

		elm_genlist_item_fields_update( objItem, "elm.text.sub", ELM_GENLIST_ITEM_FIELD_TEXT);
		popup->destroy();
	});
	p->attachPopup( popup );
}

void CtSettingView::__onDisplayNameByItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);
	CtSettingView* p = static_cast<CtSettingView*>(data);
	CtSettingData::NameOrder nameOrder = p->__getSettingData()->getNameOrder();

	std::vector<std::string> label = {V_("IDS_PB_OPT_FIRST_NAME_FIRST_ABB"), V_("IDS_PB_OPT_LAST_NAME_FIRST_ABB")};
	Elm_Object_Item* objItem = (Elm_Object_Item *)event_info;
	CtSettingData* settingData = p->__getSettingData();

	CtSettingRadioPopup* popup = new CtSettingRadioPopup(label, nameOrder == CtSettingData::NAME_ORDER_FIRST_NAME_FIRST ? 0 : 1);
	popup->setTextTranslatable(PACKAGE);
	popup->setTitle( "IDS_PB_TMBODY_NAME_FORMAT");
	popup->setOnResultCb([popup,objItem, settingData](int index){
		WDEBUG("item %d selected", index);
		if (index == 0)
			settingData->setNameOrder(CtSettingData::NAME_ORDER_FIRST_NAME_FIRST);
		else
			settingData->setNameOrder(CtSettingData::NAME_ORDER_LAST_NAME_FIRST);

		elm_genlist_item_fields_update(objItem, "elm.text.sub", ELM_GENLIST_ITEM_FIELD_TEXT);
		popup->destroy();
	});
	p->attachPopup( popup );
}

void CtSettingView::__onImportItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);
	CtSettingView* view = static_cast<CtSettingView*>(data);

	view->__importFromStorage(true);
}

void CtSettingView::__onExportItemSelectedCb(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);
	CtSettingView* p = static_cast<CtSettingView*>(data);

	p->__exportContactsTo(TargetStorage::DEVICE);
}

void CtSettingView::__importFromStorage(bool phone)
{
	WHIT();

	app_control_h service;

	app_control_create(&service);
	if (phone)
		app_control_add_extra_data(service, "path", "/opt/usr/media");
	else
		app_control_add_extra_data(service, "path", "/opt/storage/sdcard");

	app_control_add_extra_data(service, "select_type", "IMPORT_PATH_SELECT");
	app_control_add_extra_data(service, "file_type", "vcf");

	app_control_set_app_id(service, MYFILE_UG);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);

	int err = app_control_send_launch_request(service, __importFromStrorageReplyCb, this);
	WPWARN(err != APP_CONTROL_ERROR_NONE, "app_control_send_launch_request() failed(%d)", err);
	app_control_destroy(service);
}

void CtSettingView::__importFromStrorageReplyCb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WPRET_M(NULL == data, "data is NULL");
	CtSettingView* view = static_cast<CtSettingView*>(data);

	std::vector<std::string> vcards;
	char* val = NULL;
	char* last = NULL;
	app_control_get_extra_data(reply, "result", &val);

	if (val == NULL)
		return;

	char* path = strtok_r(val, "?", &last);
	while (path) {
		vcards.push_back(path);
		WHIT();
		path = strtok_r(NULL, "?", &last);
	}

	free(val);

	WHIT();
	view->__excuteContactImporter(std::move(vcards));

	return;
}

std::vector<int> __getPersonIdListFromReply(app_control_h reply)
{
	WHIT();
	int length = 0;
	char **resultArray = NULL;
	std::vector<int> idList;

	app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &resultArray, &length);
	idList.reserve(length);
	for (int i = 0; i < length; ++i) {
		int personId = atoi(resultArray[i]);
		idList.push_back(personId);
		free(resultArray[i]);
	}
	free(resultArray);
	return idList;
}

void CtSettingView::__exportContactsTo(TargetStorage exportTo)
{
	WHIT();
	if (exportTo == TargetStorage::DEVICE)
	{
		launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE,
			APP_CONTROL_DATA_TYPE_ID, [](app_control_h request, app_control_h reply, app_control_result_e result, void *data){
			CtSettingView *view = static_cast<CtSettingView*>(data);
		view->__excuteContactExporter(TargetStorage::DEVICE, request, reply, result, data);
		}, this);
	}
	else if (exportTo == TargetStorage::SD_CARD) {
		launchContactPick(APP_CONTROL_DATA_SELECTION_MODE_MULTIPLE,
			APP_CONTROL_DATA_TYPE_ID, [](app_control_h request, app_control_h reply, app_control_result_e result, void *data){
			CtSettingView *view = static_cast<CtSettingView*>(data);
		view->__excuteContactExporter(TargetStorage::SD_CARD, request, reply, result, data);
		}, this);
	}
}

void CtSettingView::__excuteContactExporter(TargetStorage saveTo, app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	WHIT();
	WPRET_M(__getPersonIdListFromReply(reply).empty(), "No contacts checked");
	CtContactsToVcard *vcard = new CtContactsToVcard(V_("IDS_PB_HEADER_EXPORT_CONTACTS_ABB"), saveTo, __getPersonIdListFromReply(reply));
	vcard->setOnFinish(std::bind(&CtSettingView::__createExportResultPopup, this, std::placeholders::_1));

	attachPopup(vcard->getProgressPopup());
	vcard->run();
}

void CtSettingView::__createExportResultPopup(CtProgressController &controller)
{
	WHIT();
	CtContactsToVcard &vcard = static_cast<CtContactsToVcard&>(controller);

	const char *deviceDirName = "Device";
	const char *devicePosition = strstr(vcard.getVcardPath().c_str(), deviceDirName);
	char content[CT_TEXT_SHORT_LEN] = { 0, };
	snprintf(content, sizeof(content), ctGetContactsString("IDS_PB_POP_P1SD_CONTACTS_HAVE_BEEN_EXPORTED_TO_P2SS"), vcard.getTotalCount(), devicePosition);

	WPopup *finishPopup = new WPopup();
	finishPopup->setTextTranslatable(PACKAGE);
	finishPopup->setTitle("IDS_PB_HEADER_CONTACTS_EXPORTED_ABB");
	finishPopup->setContent(content);
	finishPopup->addButton("IDS_PB_BUTTON_OK_ABB2", NULL);
	attachPopup(finishPopup);
}

void CtSettingView::__excuteContactImporter(std::vector<std::string> vcards)
{
	WHIT();
	int totalCount = 0;
	for (auto &&vcard : vcards) {
		int count = 0;
		int err = contacts_vcard_get_entity_count(vcard.c_str(), &count);
		if (err != CONTACTS_ERROR_NONE) {
			WERROR("contacts_vcard_get_entity_count failed, vcard = %s", vcard.c_str());
		}
		totalCount += count;
	}

	CtVcardToContacts *importer = new CtVcardToContacts(V_("IDS_PB_HEADER_IMPORT_CONTACTS_ABB2"), totalCount, std::move(vcards));
	importer->setOnFinish(std::bind(&CtSettingView::__finishImportCb, this, std::placeholders::_1));
	attachPopup(importer->getProgressPopup());
	attachHiddenPopup(importer->getCancelPopup());
	importer->run();
}

void CtSettingView::__finishImportCb(CtProgressController &controller)
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

extern "C" {
	WAPP_ASSIST_EXPORT WView* createSettingView() {
		CtSettingView* view = new CtSettingView;
		return view;
	}
}

