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

#include <dlfcn.h>
#include <app.h>
#include <contacts.h>
#include <string>
#include <telephony.h>
#include <system_settings.h>
#include <app_preference.h>

#include "WNaviframe.h"
#include "WWindow.h"
#include "WMenuPopup.h"
#include "WEditfield.h"
#include "ContactsDebug.h"

#include "CtPath.h"
#include "CtCommon.h"
#include "ContactsCommon.h"
#include "ContactsAppControl.h"
#include "ContactsThumbnail.h"

#include "CtListView.h"
#include "CtListModelDb.h"
#include "CtListModelVcard.h"

#include "CtListDataPerson.h"

#include "CtListBasicController.h"
#include "CtListUgSinglePickController.h"
#include "CtListUgMultiPickController.h"
#include "CtListUgVcardViewController.h"
#include "CtRegistryMgr.h"
#include "CtThemeMgr.h"

#include "CtMyProfileDetailView.h"
#include "CtMyProfileInputView.h"

#ifdef __cplusplus
	extern "C" {
#endif
#include <efl_extension.h>
#ifdef __cplusplus
	}
#endif

#ifdef _SDK_BUILD
	//#define //ELM_IMAGE_ASYNC_OPEN_SET(ICON,VALUE)	WERROR("not support API: //ELM_IMAGE_ASYNC_OPEN_SET()")
#else
	//#define //ELM_IMAGE_ASYNC_OPEN_SET(ICON,VALUE) 	//ELM_IMAGE_ASYNC_OPEN_SET(ICON, VALUE);
#endif

#define CONTACTS_GAL_SEARCH_LIST 20
#define CT_LIST_IMAGE_LOAD_TIMER 0.3

static const Eina_Unicode __hangulCompatibilityChoseong[14] = {
	12593, 12596, 12599, 12601, 12609, 12610, 12613,			//unicode 3131 ~ 314E
	12615, 12616, 12618, 12619, 12620, 12621, 12622};

static const Eina_Unicode __hangulJamoChoseong[14] = {
	4352, 4354, 4355, 4357, 4358, 4359, 4361,					//unicode 1100 ~ 1112
	4363, 4364, 4366, 4367, 4368, 4369, 4370};

namespace
{
	const char *SYMBOL_MAGNIFIER = "\U0001f50d";
	const char *SYMBOL_STAR = "\uF896";
}

CtListView::CtListView(const char *vcardPath)
{
	WENTER();
	__initialize();
	__hasTitle = true;
	__hasDefaultList = true;
	__hasDoneCancel = true;
	__enableDone = true;

	CtListModelVcard *model = new CtListModelVcard(vcardPath);
	__listModel = model;
	__listController = new CtListUgVcardViewController(vcardPath, this, model);
	setTitle(V_("IDS_PB_HEADER_IMPORT_CONTACTS_ABB2"));
}

CtListView::CtListView(CtListSelectMode selectMode, CtListResultType resultType, int limit)
{
	WENTER();
	__initialize();

	__hasTitle = true;
	__hasSearchbar = true;
	__hasDefaultList = true;
	__hasGroupTitle = true;
	__hasFastscroll = true;
	__hasFavoriteList = true;
	CtListHasType hasType = LIST_HAS_NONE;
	switch (resultType) {
		case LIST_RESULT_NUMBER: hasType = LIST_HAS_NUMBER; break;
		case LIST_RESULT_EMAIL: hasType = LIST_HAS_EMAIL; break;
		case LIST_RESULT_NUMBER_OR_EMAIL: hasType = LIST_HAS_NUMBER_OR_EMAIL; break;
		case LIST_RESULT_ID:
		case LIST_RESULT_VCARD:
		default:
			hasType = LIST_HAS_NONE; break;
	}

	__listModel = new CtListModelDb(hasType, limit);
	__listModel->setSelectedPersonId(__selectedPersonId);

	if (selectMode == LIST_SELECT_MULTIPLE) {
		char buf[CT_TEXT_SHORT_LEN] = {0, };
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), 0);
		setTitle(buf);

		__viewType = LIST_VIEW_CHECK;
		__hasSelectAll = true;
		__hasDoneCancel = true;
		__listController = new CtListUgMultiPickController(resultType, this, (CtListModelDb*) __listModel);
	} else {
		if (selectMode == LIST_SELECT_SINGLE_OR_CREATE) {
			WDEBUG("LIST_SELECT_SINGLE_OR_CREATE");
			__hasCreateButton = true;
		}

		setTitle(V_("IDS_PB_HEADER_SELECT"));
		__listController = new CtListUgSinglePickController(resultType, this, (CtListModelDb*) __listModel);
	}
}

CtListView::CtListView(CtListViewType viewType, CtListDataType dataType, CtListCtrlType ctrlType)
{
	WENTER();
	__initialize();
	__initViewCondition(viewType, ctrlType);
	__ctrlType = ctrlType;

	CtListModelDb* model = new CtListModelDb(dataType);
	__listModel = model;
	__listController = new CtListBasicController(ctrlType, this, model);

	__viewType = viewType;
}

CtListView::~CtListView()
{
	WENTER();

	Evas_Object *conformant = NULL;
	if(getWindow())
		conformant = getWindow()->getConformantEvasObj();

	if(conformant) {
		evas_object_smart_callback_del(conformant, "virtualkeypad,state,on", __onSipUpCb);
		evas_object_smart_callback_del(conformant, "virtualkeypad,state,off", __onSipDownCb);
	}

	evas_object_smart_callback_del(__genlist, "scroll,anim,start", __onGenlistScrollStartCb);
	evas_object_smart_callback_del(__genlist, "scroll,anim,stop", __onGenlistScrollStopCb);

	if(__loadImageTimer)
		ecore_timer_del(__loadImageTimer);

	if(__myProfileThread)
		__myProfileThread->destroy();

	if(__myProfileIdler)
		ecore_idler_del(__myProfileIdler);

	__freeFastIndexData();

	if(__listItc)
		elm_genlist_item_class_free(__listItc);
	if(__groupTitleItc)
		elm_genlist_item_class_free(__groupTitleItc);
	if(__myProfileItc)
		elm_genlist_item_class_free(__myProfileItc);
	if(__selectAllItc)
		elm_genlist_item_class_free(__selectAllItc);
	if(__searchbarItc)
		elm_genlist_item_class_free(__searchbarItc);
	if (__favoriteTitleItc)
		elm_genlist_item_class_free(__favoriteTitleItc);
	if (__mfcTitleItc)
		elm_genlist_item_class_free(__mfcTitleItc);

	if(__searchGetContactThread){
		__searchGetContactThread->destroy();
		__searchGetContactThread = NULL;
		__listModel->cancelGetContacts();
	}

	delete __listController;
	delete __listModel;

	WLEAVE();
}

Eina_Bool CtListView::__onInitMyProfileIdlerCb(void *data)
{
	WHIT();
	CtListView* view = (CtListView*)data;
	view->__myProfileIdler = NULL;
	view->__myProfileThread = new CtThread(__onInitMyProfileThreadRunCb, view, __onInitMyProfileThreadEndCb, view);
	view->__myProfileThread->start();

	return ECORE_CALLBACK_CANCEL;
}

void CtListView::__onInitMyProfileThreadRunCb(void *data)
{
	WHIT();
	CtListView *view = (CtListView*)data;
	view->__initMyProfile();
}

void CtListView::__onInitMyProfileThreadEndCb(void *data)
{
	WHIT();
	CtListView* view = (CtListView*)data;

	if(view->__myProfileItem)
		elm_genlist_item_update(view->__myProfileItem);

	if(view->__myProfileThread)
		view->__myProfileThread->destroy();
	view->__myProfileThread = NULL;

}

char* CtListView::__getMsisdnFromSim()
{
	WENTER();
	char *msisdn = NULL;
	int simCount = 0;
	telephony_handle_list_s tapiHandleList;
	int ret = telephony_init(&tapiHandleList);
	if (ret != TELEPHONY_ERROR_NONE)
		WDEBUG("no sim");
	else {
		simCount = tapiHandleList.count;

		/*my numbers(SIM1 and SIM2)*/
		for(int i=0; i<simCount; i ++){
			if(tapiHandleList.handle[i])
				ret = telephony_sim_get_subscriber_number(tapiHandleList.handle[i], &msisdn);
			if (msisdn && *msisdn) {
				WDEBUG("msisdn exists");
				break;
			}
		}

		ret = telephony_deinit(&tapiHandleList);
		if (ret != TELEPHONY_ERROR_NONE) {
			WERROR( "telephony_deinit failed(%d)", ret);
			return NULL;
		}
	}
	WLEAVE();
	return msisdn;
}

void CtListView::__initMyProfile(void)
{
	WENTER();

	char *msisdn = __getMsisdnFromSim();
	WPRET_M(msisdn == NULL, "no msisdn");

	int err = CONTACTS_ERROR_NONE;
	contacts_list_h myprofileList = NULL;
	contacts_record_h numRecord = NULL;
	contacts_record_h myRecord = NULL;
	bool changed = false;
	bool hasRecord = false;

	err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

	if (myprofileList != NULL) {
		contacts_record_h contactRecord = NULL;
		int count = 0;

		contacts_list_get_count(myprofileList, &count);
		if (count > 0) {
			err = contacts_list_get_current_record_p(myprofileList, &contactRecord);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);
		}
		err = contacts_record_clone(contactRecord, &myRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_clone() Failed(%d)", err);

		err = contacts_list_destroy(myprofileList, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);

		int index = 0;
		char *number = NULL;
		contacts_record_h record = NULL;

		while (CONTACTS_ERROR_NONE == contacts_record_get_child_record_at_p(myRecord, _contacts_my_profile.number, index++, &record)) {
			err = contacts_record_get_str_p(record, _contacts_number.number, &number);
			if (CONTACTS_ERROR_NONE != err) {
				WERROR("contacts_record_get_int() Failed(%d)", err);
				continue;
			}

			if (strcmp(number, msisdn) == 0) {
				changed = true;
				break;
			}
		}

		if(changed) {
			SAFE_FREE(msisdn);
			err = contacts_record_destroy(myRecord, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_destroy() Failed(%d)", err);
			WDEBUG("my profile not changed");
			return;
		}
	}

	if(myRecord == NULL) {
		err = contacts_record_create(_contacts_my_profile._uri, &myRecord);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_create() Failed(%d)", err);
	}
	else {
		hasRecord = true;
	}

	if(numRecord == NULL) {
		do {
			if (CONTACTS_ERROR_NONE != (err = contacts_record_create(_contacts_number._uri, &numRecord))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_record_set_str(numRecord, _contacts_number.number, msisdn))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_record_set_int(numRecord, _contacts_number.type, CONTACTS_NUMBER_TYPE_CELL))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_record_set_bool(numRecord, _contacts_number.is_default, true))) break;
			if (CONTACTS_ERROR_NONE != (err = contacts_record_set_str(numRecord, _contacts_number.label, CT_KEY_SIM_CARD_NUMBER))) break;
		} while (0);

		if(CONTACTS_ERROR_NONE != err) {
			SAFE_FREE(msisdn);
			contacts_record_destroy(myRecord, true);
			contacts_record_destroy(numRecord, true);
			WERROR("contacts_record_set_str() Failed(%d)", err);
			return;
		}
	}

	err = contacts_record_add_child_record(myRecord, _contacts_my_profile.number, numRecord);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_person_set_default_property() Failed(%d)", err);


	int contactId = 0;
	if(hasRecord == true || changed) {
		WDEBUG("update my profile");
		err = contacts_db_update_record(myRecord);
	}
	else {
		WDEBUG("insert my profile");
		err = contacts_db_insert_record(myRecord, &contactId);
	}

	if (CONTACTS_ERROR_NONE != err) {
		SAFE_FREE(msisdn);
		contacts_record_destroy(myRecord, true);
		WERROR("contacts_db_update_record() Failed(%d)", err);
		return;
	}

	SAFE_FREE(msisdn);
	contacts_record_destroy(myRecord, true);

	WLEAVE();
}

const char* CtListView::getClassName()
{
	return "CtListView";
}

Evas_Object* CtListView::onCreate( Evas_Object* parent, void* viewParam)
{
	WENTER();

	Evas_Object *base = createBaseLayout(parent);
	__baseLayout = elm_layout_add(base);

	std::string contactsEdjePath = ContactsCommon::getResourcePath(CONTACTS_EDJ);
	std::string detailEdjePath = ContactsCommon::getResourcePath(DETAIL_LAYOUT_EDJ);

	CtThemeMgr::getInstance()->addExtensionTheme(contactsEdjePath.c_str());
	CtThemeMgr::getInstance()->addExtensionTheme(detailEdjePath.c_str());
	CtThemeMgr::getInstance()->setObjectTheme(__baseLayout);

	elm_layout_file_set(__baseLayout, contactsEdjePath.c_str(), "list/base_layout");
	evas_object_size_hint_weight_set(__baseLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__baseLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(base, "elm.swallow.content", __baseLayout);
	evas_object_show(__baseLayout);

	if(__hasFavoriteList)
		__listModel->getContacts(LIST_QUERY_FAVORITE, GET_AS_SYNC);
	if(__hasMfcList)
		__listModel->getContacts(LIST_QUERY_MFC, GET_AS_SYNC);

	int listCount = __listModel->getContacts(LIST_QUERY_DEFAULT, GET_AS_SYNC_ASYNC);

	// in remove favorites view, it needs to show MFC
	if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
		listCount += __listModel->getContactsCount(LIST_QUERY_MFC);
	}

	if(__hasMyProfile == false && __onSearching == false) {
		if(listCount > 0) {
			__showGenlist();
		}
		else {
			__showNoContent();
		}
	}
	else {
		WDEBUG("listCount = %d", listCount);
		__showGenlist();
	}

	WLEAVE();

	return base;
}

void CtListView::onLanguageChanged()
{
	WHIT();

	if(__genlist)
		elm_genlist_realized_items_update(__genlist);

	if(__viewType == LIST_VIEW_REORDER)
	{
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", V_("IDS_PB_OPT_REORDER"));
	}
	Evas_Object* saveBtn = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
	if(saveBtn){
		if(__ctrlType == LIST_CTRL_DELETE_CONTACTS)
			elm_object_text_set(saveBtn, V_("IDS_TPLATFORM_ACBUTTON_DELETE_ABB"));
		else
			elm_object_text_set(saveBtn, V_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));
	}
	Evas_Object* cancelBtn = elm_object_item_part_content_get(getNaviItem(), "title_left_btn");
	if(cancelBtn){
		elm_object_text_set(cancelBtn, V_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
	}
}

bool CtListView::onPop()
{
	WHIT();

	__listController->onSelectBack();

	return true;
}

void CtListView::onDestroy()
{
	WENTER();
}

void CtListView::onDbChanged(const char* viewUri)
{
	WENTER();

	if(__loaded == false) {
		__updateQueue = 2;
		WERROR("not loaded yet");
		return;
	}

	// special case for 1 delete item peformace
	int deletedPersonId = 0;
	CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_DELETED_PERSON_ID, &deletedPersonId);
	if(deletedPersonId > 0) {
		Elm_Object_Item *deletedItem = getGenlistItemByPersonId(deletedPersonId);
		if(deletedItem) {
			WDEBUG("item of personid %d delete early", deletedPersonId);
			Elm_Object_Item *nextitem = elm_genlist_item_next_get(deletedItem);
			if(nextitem) {
				Elm_Genlist_Item_Type itemType = elm_genlist_item_type_get(nextitem);
				if(itemType == ELM_GENLIST_ITEM_GROUP) {
					Elm_Object_Item *previtem = elm_genlist_item_prev_get(deletedItem);
					if(previtem) {
						itemType = elm_genlist_item_type_get(previtem);
						if(itemType == ELM_GENLIST_ITEM_GROUP)
							elm_object_item_del(previtem);
					}
				}
			}

			elm_object_item_del(deletedItem);
		}
	}

	//while DB is being updated, popup is destroyed
	//destroyPopup();

	if(__onProgressOperation == true) {
		WDEBUG("do not update list on progressing");
		WLEAVE();
		return;
	}

	if(__background == true) {
		WDEBUG("now is background");

		int updated = 0;
		CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_FAVORITE_UPDATED, &updated);

		int inserted = 0;
		CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_INSERTED_PERSON, &inserted);

		int deleted = 0;
		CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_DELETED_PERSON, &deleted);

		if(!updated && !inserted && !deleted) {
			__updateQueue = 1;
			WLEAVE();
			return;
		}
		else
			WDEBUG("updated %d, inserted %d, deleted %d", updated, inserted, deleted);
	}

	WDEBUG(" __updateQueue %d", __updateQueue);
	if(__updateQueue < 2) {
		__updateQueue = 1;
		if(__hasFavoriteList)
			__listModel->getContacts(LIST_QUERY_FAVORITE, GET_AS_ASYNC);
		if(__hasMfcList)
			__listModel->getContacts(LIST_QUERY_MFC, GET_AS_ASYNC);
		__listModel->getContacts(LIST_QUERY_DEFAULT, GET_AS_ASYNC);
	}
	else {
		WDEBUG("already update requested %d. wait", __updateQueue);
	}
	if (__hasMyProfile) {
		int updateMyprofile = (long) evas_object_data_del(getViewNaviframe(), "ct_list_update_myprofile");
		if (updateMyprofile) {
			__myProfileIdler = ecore_idler_add(__onInitMyProfileIdlerCb, this);
		}
	}

	WLEAVE();
}

void CtListView::onCompleteGetContacts(int offset)
{
	WDEBUG("offset %d", offset);
	//Todo We need to more action

	__loaded = true;

	if (__onShowMemberList == true) {
		return;
	}

	bool showNocontent = false;

	if(offset == 0) {
		__curAppendIndex = 0;
	}

	if(__launched == 0) {
		__launched = 1;
	}

	int listCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
	// in remove favorites view, it needs to show MFC
	if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
		listCount += __listModel->getContactsCount(LIST_QUERY_MFC);
	}

	if (listCount == 0) {
		if(__hasMyProfile == false && __onSearching == false) {
			__genlist = NULL;
			__showNoContent();
			showNocontent = true;
		}
	}

	if(showNocontent == false) {
		if(__genlist == NULL)
			__showGenlist();
		else {
			__appendGenlistItems(offset);
			__updateCheckRelatedInfo();
			// after all item appended, link reset items to fastscroll
			__linkItemToRestIndex();
		}
	}

	__updateQueue--;
	WDEBUG(" %d", __updateQueue);

	if(__updateQueue > 0) {
		WDEBUG("update request exist yet %d, update again", __updateQueue);
		__updateQueue = 0;
		if(__hasFavoriteList)
			__listModel->getContacts(LIST_QUERY_FAVORITE, GET_AS_ASYNC);
		if(__hasMfcList)
			__listModel->getContacts(LIST_QUERY_MFC, GET_AS_ASYNC);

		__listModel->getContacts(LIST_QUERY_DEFAULT, GET_AS_ASYNC);
	}
	else {
		__updateQueue = 0;
	}

	Eina_Bool sipStatus = false;
	if(__searchbarItem){
		Evas_Object *entry = elm_object_item_part_content_get(__searchbarItem, "elm.swallow.content");
		sipStatus = elm_object_focus_get(entry);
	}
	if (false == __onSearching && false == sipStatus) {
		elm_genlist_realized_items_update(__genlist);
	}
	else{
		WDEBUG("sipStatus is TRUE");
	}

	if(__selectedCheckAll == true) {
		__listController->onSelectCheckAll(__selectAllItem, this, CHECK_ON);
		__updateCheckRelatedInfo();
		__selectedCheckAll = false;
	}

	WLEAVE();
}

void CtListView::onConditionChagned()
{
	WENTER();
	if(__hasFavoriteList)
		__listModel->getContacts(LIST_QUERY_FAVORITE, GET_AS_ASYNC);
	if(__hasMfcList)
		__listModel->getContacts(LIST_QUERY_MFC, GET_AS_ASYNC);
	__listModel->getContacts(LIST_QUERY_DEFAULT, GET_AS_ASYNC);
}


void CtListView::onPushed(Elm_Object_Item* naviItem)
{
	WHIT();

	int listCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
	// in remove favorites view, it needs to show MFC
	if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
		listCount += __listModel->getContactsCount(LIST_QUERY_MFC);
	}

	if (listCount > 0)
		__showTitleButton(true);
	else
		__showTitleButton(false);

	if (__hasMenu)
		enableMoreButton(naviItem, __onSelectMenu, this);

	// set virtual keypad
	Evas_Object *conformant = getWindow()->getConformantEvasObj();
	evas_object_smart_callback_add(conformant, "virtualkeypad,state,on", __onSipUpCb, this);
	evas_object_smart_callback_add(conformant, "virtualkeypad,state,off", __onSipDownCb, this);

	__listModel->attachListener(this);
}

Evas_Object* CtListView::__createToolbar(Evas_Object *navi)
{
	Evas_Object *toolbar = NULL;
	toolbar = elm_toolbar_add(navi);
	elm_object_style_set(toolbar, "default");
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_NONE);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	evas_object_show(toolbar);

	return toolbar;
}

void CtListView::onTabSelect(Elm_Object_Item *naviItem)
{
	WHIT();

	evas_object_smart_callback_add(getNaviframe()->getEvasObj(), "transition,finished", __onNaviTransitionFinishedCb, this);
	elm_object_focus_set(getEvasObj(), EINA_TRUE);

	__background = false;

	WDEBUG("__hasTitle %d __hasToolbar %d __hasDoneCancel %", __hasTitle, __hasToolbar, __hasDoneCancel);

	if (naviItem == elm_naviframe_top_item_get(getNaviframe()->getEvasObj())) {
		__showCreateButton();
	}

	__cleanSearchbar();
	destroyPopup();

	if(__hasTitle) {
		WDEBUG("item %p, title %s", naviItem, getTitle());
		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", getTitle());
	}


	if(__hasToolbar) {
		WDEBUG("need toobar");
		Evas_Object *toolbar = __createToolbar(getViewNaviframe());
		elm_object_item_part_content_set(naviItem, "toolbar", toolbar);
		elm_toolbar_item_append(toolbar, NULL, "IDS_PB_BUTTON_DONE_ABB3", __onToolbarDone, this);
	}

	__showTitleButton(true);

	enableMoreButton(naviItem, __onSelectMenu, this);

	if(__updateQueue > 0) {
		WDEBUG("update job remains");
		__updateQueue = 0;
		onDbChanged(NULL);
	}
	if(__onSearching) {
		__hideCreateButton();
	}
}
void CtListView::onTabUnselect(Elm_Object_Item *naviItem)
{
	WHIT();

	evas_object_smart_callback_del(getNaviframe()->getEvasObj(), "transition,finished", __onNaviTransitionFinishedCb);

	__background = true;

	destroyPopup();

	if(__hasCreateButton) {
		__hideCreateButton();
	}

	if (__onShowMemberList == true) {
		__onShowMemberList = false;
		__curAppendIndex = 0;
		__appendGenlistItems(0);
	}
	else {
		__cleanSearchbar();
		__resetCheckitems();
	}
}

bool CtListView::onTabPop()
{
	WHIT();

	if (__onShowMemberList == true) {
		__onShowMemberList = false;
		__curAppendIndex = 0;
		__appendGenlistItems(0);

		return false;
	}
	return true;
}

void CtListView::onEvent(int eventType)
{
	elm_genlist_realized_items_update(__genlist);
}

void CtListView::__focusInCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	WPRET_M(NULL == data, "data is NULL!");

	CtListView *listView = (CtListView*)data;

	if (listView->__onSearching) {
		Evas_Object *editfield = elm_object_item_part_content_get(listView->__searchbarItem, "elm.swallow.content");
		WEditfield *weditfield = (WEditfield *)evas_object_data_get(editfield, "weditfield");
		if(weditfield) {
			eext_object_event_callback_del(weditfield->getEntry(), EEXT_CALLBACK_BACK, __onEntryBackButton);
			eext_object_event_callback_add(weditfield->getEntry(), EEXT_CALLBACK_BACK, __onEntryBackButton, listView);
		} else {
			WERROR("weditfield is NULL");
		}
	}
}

void CtListView::onSetAsPageContent(Elm_Object_Item* parentNaviItem)
{
	WHIT();

	evas_object_smart_callback_add(getWindow()->getEvasObj(), "focus,in", __focusInCb, this);

	// set virtual keypad
	Evas_Object *conformant = getWindow()->getConformantEvasObj();
	evas_object_smart_callback_add(conformant, "virtualkeypad,state,on", __onSipUpCb, this);
	evas_object_smart_callback_add(conformant, "virtualkeypad,state,off", __onSipDownCb, this);

	__listModel->attachListener(this);
}

Evas_Object* CtListView::getViewWindow(void)
{
	if(getWindow())
		return getWindow()->getEvasObj();
	else
		WASSERT_EX(1, "window null");

	return NULL;
}

Evas_Object* CtListView::getViewNaviframe(void)
{
	return getNaviframe()->getEvasObj();
}

WNaviframe* CtListView::getWNaviframe(void)
{
	return getNaviframe();
}

void CtListView::__resetCheckitems()
{
	WHIT();

	__selectAllState = CHECK_OFF;

	Evas_Object* layout = elm_object_item_part_content_get(__selectAllItem, "elm.icon.2");
	if(layout) {
		Evas_Object* check = elm_layout_content_get(layout, "elm.swallow.content");
		if(check)
			elm_check_state_set(check, EINA_FALSE);
	}

	if(__listModel->getPersonListCount(LIST_QUERY_DEFAULT) > 0)
		__listController->onSelectCheckAll(__selectAllItem, this, __selectAllState);
}


void CtListView::__initialize(void)
{
	__baseLayout = NULL;
	__fastLayout = NULL;
	__fastScroll = NULL;
	__genlist = NULL;
	__selectAllObj = NULL;
	__compose_btn = NULL;
	__noContentLayout = NULL;

	__searchbarItem = NULL;
	__selectAllItem = NULL;
	__favoriteTitleItem = NULL;
	__mfcTitleItem = NULL;
	__myProfileItem = NULL;

	__listController = NULL;
	__listModel = NULL;
	__viewType = LIST_VIEW_DEFAULT;
	__ctrlType = LIST_CTRL_DEFAULT;

	__onSearching = false;
	__onShowMemberList = false;

	__sipStatus = false;
	__launching = false;
	__genlistScrolling = false;
	__curAppendIndex = 0;
	__launched = 0;
	__loaded = false;
	__selectedCheckAll = false;
	__onProgressOperation = false;
	__onNoContentLayout = false;

	__updateQueue = 0;
	__fastIndexList = NULL;

	__listItc = NULL;
	__groupTitleItc = NULL;
	__myProfileItc = NULL;
	__selectAllItc = NULL;
	__searchbarItc = NULL;
	__favoriteTitleItc = NULL;
	__mfcTitleItc = NULL;

	__hasTitle = false;
	__hasSearchbar = false;
	__hasFastscroll = false;
	__hasToolbar = false;
	__hasCreateButton = false;
	__hasSelectAll = false;
	__hasTotalCount = false;
	__hasGroupTitle = false;
	__hasMenu = false;
	__hasDoneCancel = false;
	__hasFavGroupTitle = false;
	__hasMyProfile = false;
	__enableDone = false;
	__hasFavoriteList = false;
	__hasMfcList = false;
	__hasDefaultList = false;

	__selectAllState = CHECK_OFF;
	__selectedPersonId = -1;
	__topItemPersonId = 0;

	__myProfileThread = NULL;
	__myProfileIdler = NULL;
	__loadImageTimer = NULL;
	__searchGetContactThread = NULL;

	__background = false;
	__isFirstGroupSync = false;
	__isFirstGroupMemberSync = false;
}

void CtListView::__onToolbarDone(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	//Do more job in view
	CtListView *listView = (CtListView*)data;
	listView->__listController->onSelectDone();
}

char *CtListView::__getDelPopupText()
{
	char buf[CT_TEXT_SHORT_LEN] ={0,};
	int listCount = 0;
	int checkedCount = 0;
	int index = 0;

	if (__onSearching) {
		listCount = __listModel->getOriginalPersonDataListSize();
		checkedCount = __listModel->getCheckedDataCount();
		WDEBUG("listCount = %d, checkedCount = %d",listCount,checkedCount);
	}
	else {
		listCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
		while( auto personData = __listModel->getAtPersonData(index, LIST_QUERY_DEFAULT) )
		{
			if(personData->isChecked())
				checkedCount++;
			index++;
		}
	}

	if(checkedCount == listCount) {
		snprintf(buf,  sizeof(buf), V_("IDS_PB_POP_ALL_CONTACTS_WILL_BE_DELETED"));
		return strdup(buf);
	}
	else if (checkedCount > 1) {
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_POP_PD_CONTACTS_WILL_BE_DELETED"), checkedCount);
		return strdup(buf);
	}
	else if (checkedCount == 1) {
		snprintf(buf, sizeof(buf), V_("IDS_PB_POP_1_CONTACT_WILL_BE_DELETED"));
		return strdup(buf);
	}
	return NULL;
}
void CtListView::__onNaviTitleDone(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	//Do more job in view
	CtListView *listView = (CtListView*)data;
	listView->__listController->onSelectDone();
}

void CtListView::__onNaviTitleCancel(void *data, Evas_Object *obj, void *event_info)
{

	WHIT();
	//Do more job in view
	CtListView *listView = (CtListView*)data;
	listView->__listController->onSelectCancel();
}

void CtListView::__onNaviTitleBack(void *data, Evas_Object *obj, void *event_info)
{

	WHIT();
	WPRET_M(!data, "invalid data");
	CtListView *listView = (CtListView*)data;

	WPRET_M(!listView->__listController, "invalid controller");
	listView->popOut();
}

void CtListView::__showTitleButton(bool show)
{
	WHIT();
	WPRET_M(!getNaviframe(), "getNaviframe() is NULL");

	if(__hasDoneCancel) {
		WDEBUG("title show %d", show);
		if(show) {
			Evas_Object* btn = elm_object_item_part_content_get(getNaviItem(), "title_left_btn");
			if(btn == NULL) {
				btn = elm_button_add(getViewNaviframe());
				elm_object_style_set(btn, "naviframe/title_left");
				elm_object_text_set(btn, V_("IDS_TPLATFORM_ACBUTTON_CANCEL_ABB"));
				evas_object_smart_callback_add(btn, "clicked",__onNaviTitleCancel, this);
				elm_object_item_part_content_set(getNaviItem(), "title_left_btn", btn);
			}

			btn = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
			if(btn == NULL) {
				btn = elm_button_add(getViewNaviframe());
				elm_object_style_set(btn, "naviframe/title_right");
				elm_object_text_set(btn, V_("IDS_TPLATFORM_ACBUTTON_DONE_ABB"));

				evas_object_smart_callback_add(btn, "clicked", __onNaviTitleDone, this);
				elm_object_item_part_content_set(getNaviItem(), "title_right_btn", btn);
				if (!__enableDone)
					elm_object_disabled_set(btn, EINA_TRUE);
			}
			else {
				if (!__enableDone)
					elm_object_disabled_set(btn, EINA_TRUE);
				else
					elm_object_disabled_set(btn, EINA_FALSE);
			}
		}
		else {
			Evas_Object* btn = elm_object_item_part_content_get(getNaviItem(), "title_left_btn");
			if(btn) {
				elm_object_item_part_content_unset(getNaviItem(), "title_left_btn");
				evas_object_del(btn);
			}
			btn = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
			if(btn) {
				elm_object_item_part_content_unset(getNaviItem(), "title_right_btn");
				evas_object_del(btn);
			}
		}
	} else {
		//Back button should be added manually. Auto back button is not created
		//because there is no previous page.

		Evas_Object *backButton = elm_button_add(getViewNaviframe());
		elm_object_style_set(backButton, "naviframe/back_btn/default");
		evas_object_smart_callback_add(backButton, "clicked",__onNaviTitleBack, this);
		elm_object_item_part_content_set(getNaviItem(), "prev_btn", backButton);
	}
}

void CtListView::__freeFastIndexData()
{
	WENTER();
	void *data = NULL;

	if(__hasFastscroll == false)
		return;

	EINA_LIST_FREE(__fastIndexList, data) {
		CtListFastIndexData *fastIndexdata = (CtListFastIndexData*)data;
		delete fastIndexdata;
	}
}

void CtListView::__appendFastScrollLetter(const std::string &str)
{
	WENTER();

	CtListFastIndexData *indexData = new CtListFastIndexData();
	strncpy(indexData->index, str.c_str(), sizeof(indexData->index));
	indexData->index[sizeof(indexData->index)/sizeof(indexData->index[0])-1] = '\0';
	indexData->item = elm_index_item_append(__fastScroll, str.c_str(), NULL, NULL);
	__fastIndexList = eina_list_append(__fastIndexList, indexData);
	WLEAVE();
}

void CtListView::__hideFastScroll(void)
{
	WENTER();

	elm_object_signal_emit(__fastLayout, "elm,state,fastscroll,hide", "");
}

void CtListView::__showFastScroll(void)
{
	WENTER();

	if(__listModel->getPersonListCount(LIST_QUERY_DEFAULT) > 0)
		elm_object_signal_emit(__fastLayout, "elm,state,fastscroll,show", "");
}

void CtListView::__onChangedSearchEditField(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	CtListView *listView = (CtListView*)data;
	char* search = NULL;

	search = ctCommonMarkupToUtf8(elm_entry_entry_get(obj));
	WDEBUG("search %s, old search %s", search, listView->__listModel->getSearchString());

	if (SAFE_STRLEN(search) == 0 && listView->__listModel->getSearchString() == NULL) {
		if (SAFE_STRLEN(elm_entry_entry_get(obj)) > 0) {
			SAFE_FREE(search);
			return;
		} else if (SAFE_STRLEN(elm_entry_entry_get(obj)) == 0) {
			WDEBUG("no data");
			SAFE_FREE(search);
			elm_object_part_text_set(obj, "elm.guide", V_("IDS_PB_NPBODY_SEARCH"));
		}
	}

	if (SAFE_STRLEN(search) > 0) {
		listView->__onSearching = true;
	} else {
		listView->__onSearching = false;
		if (listView->__hasCreateButton) {
			Evas_Object *editfield = elm_object_item_part_content_get(listView->__searchbarItem, "elm.swallow.content");
			if(!elm_object_focus_get(editfield)) {
				listView->__showCreateButton();
			}
		}
		elm_object_part_text_set(obj, "elm.guide", V_("IDS_PB_NPBODY_SEARCH"));
	}

	if (true == listView->__onSearching) {
		if (listView->__listModel->getSearchString() && (SAFE_STRCMP(search, listView->__listModel->getSearchString()) == 0)) {
			// if search text is not changed
			SAFE_FREE(search);
			return;
		}
	}
	else {
		if (SAFE_STRLEN(search) == 0 && listView->__listModel->getSearchString() == NULL) {
			// if search text is not changed
			SAFE_FREE(search);
			return;
		}
	}

	if (true == listView->__onSearching) {
		if (NULL == listView->__listModel->getSearchString()) {
			// if start searching, initial the original checked list at the first time
			listView->__listModel->initialOriginalCheckedIdList();
			listView->__listModel->initialOriginalPersonDataListSize();
		}
	}
	else {
		listView->__listModel->clearOriginalCheckedIdList();
		listView->__listModel->clearOriginalPersonDataListSize();
	}

	listView->__listModel->setSearchString(search);

	if(listView->__searchGetContactThread){
		listView->__searchGetContactThread->destroy();
		listView->__searchGetContactThread = NULL;
		listView->__listModel->cancelGetContacts();
	}

	listView->__searchGetContactThread = new CtThread(__onSearchGetContactsThreadRunCb, listView, __onSearchGetContactsThreadEndCb, listView);
	listView->__searchGetContactThread->start();

	SAFE_FREE(search);
}

void CtListView::__onSearchbarActivatedCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	elm_object_focus_set(obj, EINA_FALSE);
}

void CtListView::__fastIndexChanged(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	const Elm_Object_Item *item = (Elm_Object_Item*)event_info;
	Elm_Object_Item *item2 = (Elm_Object_Item*)elm_object_item_data_get(item);

	elm_index_indicator_disabled_set(obj, EINA_FALSE);
	elm_genlist_item_show(item2, ELM_GENLIST_ITEM_SCROLLTO_TOP);
}

void CtListView::__fastIndexSelected(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	Elm_Object_Item *item = (Elm_Object_Item*)event_info;
	Elm_Object_Item *item2 = (Elm_Object_Item*)elm_object_item_data_get(item);

	elm_genlist_item_show(item2, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	elm_index_item_selected_set(item, EINA_FALSE);
}

void CtListView::__showGenlist(void)
{
	WENTER();

	__setItemClassStyle();

	Evas_Object* oldLayout = elm_object_part_content_get(__baseLayout, "section.content");
	if(oldLayout){
		WDEBUG("delete old layout");
		elm_object_part_content_unset(__baseLayout, "section.content");
		evas_object_del(oldLayout);
	}

	//add list base layout
	__fastLayout = elm_layout_add(__baseLayout);
	if(__hasCreateButton) {
		__showCreateButton();
	}
	elm_layout_theme_set(__fastLayout, "layout", "application", "fastscroll");

	evas_object_size_hint_weight_set(__fastLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(__baseLayout, "section.content", __fastLayout);
	evas_object_show(__fastLayout);

	//add fastscroll
	Evas_Object* fastscroll = __createFastscroll(__fastLayout);
	if(fastscroll)
		elm_object_part_content_set(__fastLayout, "elm.swallow.fastscroll", fastscroll);
	else
		elm_object_signal_emit(__fastLayout, "elm,state,fastscroll,hide", "");

	Evas_Object* box = elm_box_add(__fastLayout);
	elm_object_part_content_set(__fastLayout, "elm.swallow.content", box);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(box);

	//add genlist
	__genlist = __createGenlist(box);
	elm_box_pack_end(box, __genlist);

	__noContentLayout = __setNoContentsLayout(box);
	elm_box_pack_end(box, __noContentLayout);

	__curAppendIndex = 0;
	__appendGenlistItems(0);

	WLEAVE();

	return;

}

void CtListView::__onGenlistScrollStartCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtListView *view = (CtListView*)data;

	view->__genlistScrolling = true;

	if(view->__loadImageTimer)
		ecore_timer_del(view->__loadImageTimer);
	view->__loadImageTimer = NULL;
}

void CtListView::__onGenlistScrollStopCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtListView *view = (CtListView*)data;

	view->__genlistScrolling = false;

	if(NULL == view->__genlist)
		WDEBUG("__genlist is NULL");
	else
		WDEBUG("__genlist is NOT NULL");

	if(view->__loadImageTimer)
		ecore_timer_del(view->__loadImageTimer);
	view->__loadImageTimer = NULL;

	view->__loadImageTimer = ecore_timer_add(CT_LIST_IMAGE_LOAD_TIMER, __loadImageTimerCb, view);
}

Eina_Bool CtListView::__loadImageTimerCb(void *user_data)
{
	WHIT();
	CtListView *view = (CtListView*)user_data;

	if(view->__loadImageTimer)
		ecore_timer_del(view->__loadImageTimer);
	view->__loadImageTimer = NULL;

	if(view->__genlistScrolling == false) {
		WDEBUG("load image on scrolling stop");
		elm_genlist_realized_items_update(view->__genlist);
	}
	else {
		WDEBUG("load image pass on scrolling");
	}

	return ECORE_CALLBACK_CANCEL;
}

void CtListView::__onNaviTransitionFinishedCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtListView *view = (CtListView*)data;

	Elm_Object_Item *item = elm_naviframe_top_item_get(obj);
	if(item == view->getNaviItem()) {
		WDEBUG("emerged");
		view->__background = false;

		int updatedMFC = 0;
		CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_MFC_UPDATED, &updatedMFC);

		elm_genlist_realized_items_update(view->__genlist);

		if(view->__updateQueue > 0 || updatedMFC>0) {
			WDEBUG("update job remains. updatedMFC %d", updatedMFC);
			view->__updateQueue = 0;
			view->onDbChanged(NULL);
		}

		if (view->__hasCreateButton) {
			view->__showCreateButton();
		}
	}
	else {
		WDEBUG("set background");
		view->__background = true;
		if(view->__hasCreateButton) {
			view->__hideCreateButton();
		}
	}

	WLEAVE();
}

void CtListView::__onSearchGetContactsThreadEndCb(void *data)
{
	WENTER();

	CtListView* listView = (CtListView*)data;
	listView->__curAppendIndex = 0;
	listView->__appendGenlistItems(0);

	WLEAVE();
}

void CtListView:: __onSearchGetContactsThreadRunCb(void *data)
{
	WENTER();

	CtListView *listView = (CtListView*)data;
	listView->__listModel->getContacts(LIST_QUERY_DEFAULT, GET_AS_SYNC_ASYNC);

	WLEAVE();
}

void CtListView:: __genlistReoderCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtListView *view = (CtListView*)data;
	view->__enableDone = true;
	view->__showTitleButton(true);

	Elm_Object_Item *item = NULL;
	Elm_Object_Item *previtem = NULL;
	Elm_Object_Item *nextitem = NULL;

	int personId = 0;
	int prevPersonId = 0;
	int nextPersonId = 0;

	CtListView::CtListDataItem* itemData = NULL;

	item = (Elm_Object_Item*) event_info;

	Elm_Genlist_Item_Type itemType = elm_genlist_item_type_get(item);
	if(itemType == ELM_GENLIST_ITEM_GROUP) {
		return;
	}

	itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(item);
	if(itemData) {
		personId = itemData->personData->getPersonId();
	}

	previtem = elm_genlist_item_prev_get(item);
	if(previtem) {
		itemType = elm_genlist_item_type_get(previtem);
		if(itemType != ELM_GENLIST_ITEM_GROUP) {
			itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(previtem);
			if(itemData)
				prevPersonId = itemData->personData->getPersonId();
		}
	}

	nextitem = elm_genlist_item_next_get(item);
	if(nextitem) {
		itemType = elm_genlist_item_type_get(nextitem);
		if(itemType != ELM_GENLIST_ITEM_GROUP) {
			itemData = (CtListView::CtListDataItem*)elm_object_item_data_get(nextitem);
			if(itemData)
				nextPersonId = itemData->personData->getPersonId();
		}
	}

	CtListFavoriteReorderData reorderData;
	reorderData.personId = personId;
	reorderData.prevPersonId = prevPersonId;
	reorderData.nextPersonId = nextPersonId;

	view->__reorderData.push_back(reorderData);
	elm_genlist_reorder_mode_set(obj, EINA_FALSE);

}

Evas_Object* CtListView::__createGenlist(Evas_Object* parent)
{
	WHIT();

	Evas_Object* genlist = elm_genlist_add(parent);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	//evas_object_smart_callback_add(genlist, "realized", __onGenlistRealizedCb, NULL);
	//for speed up scrolling list
	evas_object_smart_callback_add(genlist, "scroll,anim,start", __onGenlistScrollStartCb, this);
	evas_object_smart_callback_add(genlist, "scroll,anim,stop", __onGenlistScrollStopCb, this);

	evas_object_show(genlist);

	if(__viewType == LIST_VIEW_REORDER){
		elm_genlist_longpress_timeout_set(genlist, 0.3);
		evas_object_smart_callback_add(genlist, "moved", __genlistReoderCb, this);
	}

	return genlist;
}

Evas_Object* CtListView::__createFastscroll(Evas_Object* parent)
{
	WENTER();

	if(__hasFastscroll == false) {
		return NULL;
	}

	Evas_Object* fastscroll = elm_index_add(parent);
	evas_object_smart_callback_add(fastscroll, "changed", __fastIndexChanged, NULL);
	evas_object_smart_callback_add(fastscroll, "selected", __fastIndexSelected, NULL);
	evas_object_show(fastscroll);
	elm_index_autohide_disabled_set(fastscroll, EINA_TRUE);
	elm_index_item_clear(fastscroll);
	__appendFastScrollLetter(SYMBOL_MAGNIFIER);
	if(__favoriteTitleItem) {
		__appendFastScrollLetter(SYMBOL_STAR);
	}
	__fastScroll = fastscroll;

	WLEAVE();
	return fastscroll;
}

void CtListView::__hideCreateButton(void)
{
	WENTER();
	if (NULL == __compose_btn) {
		WDEBUG("__compose_btn is NULL");
		return;
	}

	WWindow *window = getWindow();
	if (!window) {
		return;
	}

	elm_object_part_content_unset(window->getBaseLayoutEvasObj(), "elm.swallow.floatingbutton");
	evas_object_hide(__compose_btn);
}

void CtListView::__showCreateButton(void)
{
	WENTER();
	WWindow *window = getWindow();
	if (!window) {
		return;
	}

	Evas_Object *parent = window->getBaseLayoutEvasObj();

	if (NULL == __compose_btn) {
		__compose_btn = __createFloatBtn(parent);
	}

	if (!__background) {
		elm_object_part_content_set(parent, "elm.swallow.floatingbutton", __compose_btn);
	}
}

Evas_Object *CtListView::__createFloatBtn(Evas_Object* parent)
{
	WHIT();
	Evas_Object *floatBtn = eext_floatingbutton_add(parent);
	Evas_Object *button= elm_button_add(floatBtn);
	if(button) {
		elm_object_part_content_set(floatBtn, "button1", button);
		Evas_Object *image = elm_image_add(button);
		elm_image_file_set(image, ContactsCommon::getResourcePath(CT_IMG_ICON_LIST_ADD_BUTTON).c_str(), NULL);
		elm_object_part_content_set(button, "icon", image);
		evas_object_smart_callback_add(button, "clicked", __onSelectFloatBtn, this);
	}
	return floatBtn;
}

void CtListView::__onSelectFloatBtn(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();
	CtListView *listView = (CtListView*)data;
	listView->__listController->onSelectCreate();
}

void CtListView::__cleanSearchbar()
{
	WHIT();
	if(__searchbarItem) {
		//elm_genlist_item_show(__searchbarItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
		Evas_Object *layout = elm_object_item_part_content_get(__searchbarItem, "elm.swallow.content");
		WControl *control = WControl_getInstanceFromEvasObj(layout);
		WEditfield *editfield = static_cast<WEditfield *>(control);
		if(editfield) {
			if(elm_entry_is_empty(editfield->getEntry()) == EINA_FALSE) {
				elm_entry_entry_set(editfield->getEntry(), "");
			}
		}

		__showFastScroll();
	}
}

Evas_Object* CtListView::__createNoContent(Evas_Object* parent)
{
	WHIT();

	Evas_Object* noContent = elm_layout_add(parent);
	elm_layout_theme_set(noContent, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(noContent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(noContent, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_object_domain_translatable_part_text_set(noContent, "elm.text", PACKAGE, "IDS_PB_NPBODY_NO_CONTACTS");
	elm_object_domain_translatable_part_text_set(noContent, "elm.help.text", PACKAGE, "IDS_PB_BODY_AFTER_YOU_CREATE_CONTACTS_THEY_WILL_BE_SHOWN_HERE");

	elm_layout_signal_emit(noContent, "text,disabled", "");
	elm_layout_signal_emit(noContent, "align.center", "elm");
	// to work signal directly not on idler
	edje_object_message_signal_process(elm_layout_edje_get(noContent));

	evas_object_show(noContent);

	__showTitleButton(false);

	return noContent;
}

Eina_Bool CtListView::__createNocontentsPopupCb(void* data)
{
	WHIT();
	CtListView *listView = (CtListView*)data;
	listView->__listController->onSelectMenu();
	listView->__launching = false;

	return ECORE_CALLBACK_CANCEL;
}

void CtListView::__showNoContent(void)
{
	WHIT();

	Evas_Object* oldLayout = elm_object_part_content_get(__baseLayout, "section.content");
	if(oldLayout){
		WDEBUG("delete old layout");
		if(oldLayout == __fastLayout)
			__fastLayout = NULL;
		evas_object_del(oldLayout);
	}

	Evas_Object *noContent = __createNoContent(__baseLayout);
	elm_object_part_content_set(__baseLayout, "section.content", noContent);

	__updateCheckRelatedInfo();

	return;
}

void CtListView::__updateCheckRelatedInfo()
{
	WHIT();

	if (__viewType == LIST_VIEW_CHECK) {
		char buf[CT_TEXT_SHORT_LEN] = {0};

		int checkedIdNum = __listModel->getCheckedDataCount();
		if(__listModel->getLimitCount() == 0)
			snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), checkedIdNum);
		else
			snprintf(buf, sizeof(buf), "%d/%d", checkedIdNum, __listModel->getLimitCount());

		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", buf);
		Evas_Object *btn = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		WDEBUG("%i", checkedIdNum);
		if(btn) {
			if( checkedIdNum > 0) {
				elm_object_disabled_set(btn, EINA_FALSE);
				__enableDone = true;
			}
			else {
				elm_object_disabled_set(btn, EINA_TRUE);
				__enableDone = false;
			}
		}


		if (__selectAllItem) {
			if (__onSearching) {
				if (__listModel->checkIsSelectAllOnSearching()) {
					__selectAllState = CHECK_ON;
				}
				else {
					__selectAllState = CHECK_OFF;
				}
			}
			else {
				int maxCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
				if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
					maxCount += __listModel->getPersonListCount(LIST_QUERY_MFC);
				}

				if (checkedIdNum == maxCount) {
					__selectAllState = CHECK_ON;
				}
				else {
					__selectAllState = CHECK_OFF;
				}
			}
			elm_genlist_item_fields_update( __selectAllItem, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

void CtListView::__updateCheckAllRelatedInfo()
{
	WHIT();

	if (__viewType == LIST_VIEW_CHECK) {
		char buf[CT_TEXT_SHORT_LEN] = {0};

		int checkedCount = 0;
		int limitCount = __listModel->getLimitCount();
		if(__loaded)
			checkedCount = __listModel->getCheckedDataCount();
		else
			checkedCount = __listModel->getContactsCount(LIST_QUERY_DEFAULT);

		if(!__onSearching && __selectAllState == CHECK_OFF)
			checkedCount = 0;

		if(limitCount == 0)
			snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), checkedCount);
		else
			snprintf(buf, sizeof(buf), "%d/%d", checkedCount, limitCount);

		elm_object_item_part_text_set(getNaviItem(), "elm.text.title", buf);
		Evas_Object *btn = elm_object_item_part_content_get(getNaviItem(), "title_right_btn");
		if(btn) {
			if(checkedCount > 0) {
				elm_object_disabled_set(btn, EINA_FALSE);
				__enableDone = true;
			}
			else {
				elm_object_disabled_set(btn, EINA_TRUE);
				__enableDone = false;
			}
		}
	}

}

void CtListView::__onSelectItem(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtListView *listView = (CtListView*)data;
	elm_genlist_item_selected_set((Elm_Object_Item *)event_info, EINA_FALSE);

	if (listView->__viewType == LIST_VIEW_REORDER)
		return;

	Elm_Object_Item *item = (Elm_Object_Item*)event_info;
	Evas_Object* check = elm_object_item_part_content_get(item,"elm.swallow.end");
	if (check) {
		CtListView::CtListDataItem* itemData = (CtListView::CtListDataItem*)elm_object_item_data_get((Elm_Object_Item*)event_info);
		//Eina_Bool state = elm_check_state_get(check);
		Eina_Bool state = itemData->personData->isChecked();

		listView->__listController->onSelectItem((Elm_Object_Item*)event_info, listView, (CtCheckState)(!state));
		listView->__updateCheckRelatedInfo();
		return;
	}

	listView->__listController->onSelectItem((Elm_Object_Item*)event_info, listView, CHECK_NONE);
	listView->__updateCheckRelatedInfo();
}
/*
bool CtListView::__checkMFC(int personId)
{
	WHIT();
	std::shared_ptr<CtListDataPerson> dataPerson;
	int mfcIndex = 0;

	while(true){
		if (mfcIndex > 5)
			break;
		dataPerson = __listModel->getAtPersonData(mfcIndex, LIST_QUERY_MFC);
		if(dataPerson == NULL)
			break;
		if(dataPerson->getPersonId() == personId)
			return true;

		mfcIndex++;
	}
	return false;
}
*/
void CtListView::__onSelectCheckAll(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtListView *listView = (CtListView*)data;
	Elm_Object_Item *item = (Elm_Object_Item*)event_info;

	elm_genlist_item_selected_set(item, EINA_FALSE);

	listView->__selectedCheckAll = false;

	if(listView->__loaded == false) {
		WDEBUG("not loaded yet");
		listView->__selectedCheckAll = true;
		return;
	}

	Evas_Object* check = elm_object_item_part_content_get(item, "elm.swallow.end");
	elm_check_state_set(check, !elm_check_state_get(check));

	listView->__listController->onSelectCheckAll(item, listView, (CtCheckState)elm_check_state_get(check));
	listView->__updateCheckRelatedInfo();
}

void CtListView::__onSelectMenu(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtListView *listView = (CtListView*)data;

	listView->destroyPopup();  // Due to some race condition sometime listview menu popup and profile/detail view both appears

	if(listView->__listModel->getMadePersonListStatus()) {//display menu options only id data is received from DB
		listView->__listController->onSelectMenu();
	}
}

void CtListView::__onMyProfileSelectCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	CtListView *listView = (CtListView*)data;
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;

	elm_genlist_item_selected_set(item, EINA_FALSE);
	bool first = false;
	int error = preference_get_boolean(CT_KEY_MYPROFILE_CREATE_FIRST, &first);
	if (error != PREFERENCE_ERROR_NONE)
	{
		WDEBUG("preference_get_boolean failed");
	}
	int err = CONTACTS_ERROR_NONE;
	contacts_list_h myprofileList = NULL;
	int count = 0;

	err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
	WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

	if (myprofileList != NULL) {
		contacts_list_get_count(myprofileList, &count);

		err = contacts_list_destroy(myprofileList, true);
		WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_count() Failed(%d)", err);

		if (count > 0 && !first) {
			WView* myProfileDetailView = new CtMyProfileDetailView();
			listView->getWNaviframe()->push(myProfileDetailView, NULL, NULL);
		} else {
			WView* myProfileInputView = new CtMyProfileInputView(NULL);
			listView->getWNaviframe()->push(myProfileInputView, NULL, NULL);
		}
	}
}

//should be removed api
void CtListView::__onClickRemoveBtn(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
	//Do more job in view
//	CtListDataItem *item = (CtListDataItem*)data;
	//item->listView->__listController->clickRemoveBtnOperation(data, obj,event_info);
}

void CtListView::__onGenlistRealizedCb(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();
}

void CtListView::__updatetGenlistItem()
{
	WENTER();

}

Eina_Unicode CtListView::__hangulCompatibilityToJamo(Eina_Unicode unicode)
{
	int i;

	if(unicode < __hangulCompatibilityChoseong[0] || unicode > __hangulCompatibilityChoseong[13])
		return unicode;


	for(i=0;i<14;i++) {
		if(unicode == __hangulCompatibilityChoseong[i])
			return __hangulJamoChoseong[i];
	}

	return unicode;
}


bool CtListView::__compareCharacter(const char *str1, const char *str2)
{
	int tmp_len;
	Eina_Unicode *unicode;
	Eina_Unicode uniJamo1;
	Eina_Unicode *uniJamo2;
	bool result = false;

	unicode = eina_unicode_utf8_to_unicode(str1, &tmp_len);
	WPRET_VM(!unicode, false, "eina_unicode_utf8_to_unicode failed for %s", str1);

	uniJamo1 = __hangulCompatibilityToJamo(*unicode);
	uniJamo2 = eina_unicode_utf8_to_unicode(str2, &tmp_len);
	if(!uniJamo2) {
		free(unicode);
		WERROR("eina_unicode_utf8_to_unicode failed for %s", str2);
		return false;
	}

	if(uniJamo1 == *uniJamo2 || eina_unicode_strcmp(uniJamo2, unicode) == 0)
		result = true;

	free(unicode);
	free(uniJamo2);

	return result;
}

void CtListView::__linkItemToIndex(Elm_Object_Item *groupItem)
{
	void *data = NULL;
	bool matched = false;
	Eina_List *cusor = NULL;
	int i = 0;
	int count = 0;
	int firstSetIndex = -1;
	WPRET_M(NULL == __fastIndexList, "No fastindex");

	count = eina_list_count(__fastIndexList);

	EINA_LIST_FOREACH(__fastIndexList, cusor, data) {
		//leave rest index item as on unlinked from match item
		if (matched == true && i < count)
			break;

		// skip magnify, star
		if(i < 1) {
			i++;
			continue;
		}

		CtListFastIndexData *indexData = (CtListFastIndexData*)data;

		if(__compareCharacter(indexData->index, __curGroupIndex.c_str()) == true)
			matched = true;

		//link group item to vacant index items until find match item
		Elm_Object_Item *linkedItem = NULL;
		linkedItem = (Elm_Object_Item*)elm_object_item_data_get(indexData->item);
		if(linkedItem == NULL) {
			elm_object_item_data_set(indexData->item, (void*)groupItem);
			//keep first index set
			if(firstSetIndex < 0)
				firstSetIndex = i;
		}
		i++;
	}

	//in case there is unmatch item among the matching letters.
	if(matched == false) {
		if(__unmatchedGroupTitleItem == NULL)
			__unmatchedGroupTitleItem = groupItem;

		//at least '#' need link to first group item what ever

		/* not needed by mgnify, star
		if(firstSetIndex == 0)
			firstSetIndex++;
		 */
		if(firstSetIndex > 0) {
			Eina_List *cusor2 = NULL;
			cusor = NULL;
			cusor = eina_list_nth_list(__fastIndexList, firstSetIndex);
			EINA_LIST_FOREACH(cusor, cusor2, data) {
				CtListFastIndexData *indexData = (CtListFastIndexData*)data;
				elm_object_item_data_set(indexData->item, NULL);
			}
		}
	}
	else
		__unmatchedGroupTitleItem = NULL;
}

void CtListView::__linkItemsManually(Eina_List *indexItem, Elm_Object_Item *genItem)
{
	WENTER();
	if(indexItem && genItem) {
		void *data = eina_list_data_get(indexItem);
		elm_object_item_data_set(static_cast<CtListFastIndexData*>(data)->item, (void*)genItem);
	}
}

void CtListView::__linkItemToRestIndex()
{
	WENTER();

	void *data = NULL;
	Eina_List *cusor = NULL;

	if(__hasFastscroll == false)
		return;

	if(__unmatchedGroupTitleItem)
		__curGroupTitleItem = __unmatchedGroupTitleItem;

	EINA_LIST_FOREACH(__fastIndexList, cusor, data) {
		CtListFastIndexData *indexData = (CtListFastIndexData*)data;
		Elm_Object_Item *linkedItem = NULL;
		linkedItem = (Elm_Object_Item*)elm_object_item_data_get(indexData->item);
		if(linkedItem == NULL) {
			elm_object_item_data_set(indexData->item, (void*)__curGroupTitleItem);
		}
	}

	__unmatchedGroupTitleItem = NULL;

	int value = 0;
	if(CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_FAVORITE_UPDATED, &value) == REGISTRY_SUCCESS) {
		if(value == 1) {
			if(__favoriteTitleItem)
				elm_genlist_item_show(__favoriteTitleItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
			else
				elm_genlist_item_show(__searchbarItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
		}
	}
	else if(CtRegistryMgr::getInstance()->getInt(REGISTRY_KEY_INSERTED_PERSON_ID, &value) == REGISTRY_SUCCESS) {
		Elm_Object_Item *curItem = getGenlistItemByPersonId(value);
		if(curItem)
			elm_genlist_item_show(curItem, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}
}

Elm_Object_Item* CtListView::getGenlistItemByPersonId(int personId)
{
	WDEBUG("personid %d", personId);
	Elm_Object_Item *curItem = elm_genlist_first_item_get(__genlist);
	while(curItem) {
		if(elm_genlist_item_type_get(curItem) == ELM_GENLIST_ITEM_GROUP) {
			curItem = elm_genlist_item_next_get(curItem);
			continue;
		}
		else if(curItem == __searchbarItem) {
			curItem = elm_genlist_item_next_get(curItem);
			continue;
		}
		else if(curItem == __selectAllItem) {
			curItem = elm_genlist_item_next_get(curItem);
			continue;
		}
		else if(curItem == __myProfileItem) {
			curItem = elm_genlist_item_next_get(curItem);
			continue;
		}

		CtListDataItem *dataItem = (CtListDataItem*)elm_object_item_data_get(curItem);
		if(dataItem) {
			if (dataItem->personData) {
				if(dataItem->personData->getPersonId() == personId) {
					WDEBUG("find personid item");
					break;
				}
			}
		}
		curItem = elm_genlist_item_next_get(curItem);
	}

	WDEBUG("curItem %p", curItem);
	return curItem;
}

void CtListView::setOnSingleResult(SingleResultCb handler)
{
	__singleResultCb = std::move(handler);
}

void CtListView::setOnMultiResult(MultiResultCb handler)
{
	__multiResultCb = std::move(handler);
}

void CtListView::sendSingleResult(const char *data, CtListResultType type) const
{
	if (__singleResultCb) {
		__singleResultCb(data, type);
	}
}

void CtListView::sendMultiResult(const char *data[], size_t count) const
{
	if (__multiResultCb) {
		__multiResultCb(data, count);
	}
}

void CtListView::__initFastscrollData()
{
	__curGroupIndex.clear();
	__unmatchedGroupTitleItem = NULL;
	__curGroupTitleItem = NULL;

	Eina_List *cursor = NULL;
	void *data = NULL;

	EINA_LIST_FOREACH(__fastIndexList, cursor, data) {
		CtListFastIndexData *indexData = (CtListFastIndexData*)data;

		elm_object_item_data_set(indexData->item, NULL);
	}
}
void CtListView::__initListItems(void)
{
	WHIT();

	if(__searchbarItem) {
		Elm_Object_Item *curItem = elm_genlist_item_next_get(__searchbarItem);
		// if while searching or nocontact clear all
		if(__onNoContentLayout || elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
			while(curItem) {
				Elm_Object_Item *item2 = elm_genlist_item_next_get(curItem);
				elm_object_item_del(curItem);
				curItem = item2;
			}
			// init grobal variables
			__favoriteTitleItem = NULL;
			__mfcTitleItem = NULL;
			__myProfileItem = NULL;
			__selectAllItem = NULL;

			__selectAllState = CHECK_OFF;
		}
	}
	else {
		elm_genlist_clear(__genlist);
		// init grobal variables
		__favoriteTitleItem = NULL;
		__mfcTitleItem = NULL;
		__myProfileItem = NULL;
		__searchbarItem = NULL;
		__selectAllItem = NULL;

		__selectAllState = CHECK_OFF;
	}

	__initFastscrollData();

	__showTitleButton(true);
}


Elm_Object_Item* CtListView::__addSearchbarItem(void)
{
	if(__hasSearchbar == true) {
		if(__searchbarItem == NULL) {
			WDEBUG("searchbar added");
			__searchbarItem = elm_genlist_item_prepend(__genlist, __searchbarItc, this, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

			if (__searchbarItem) {
				elm_genlist_item_select_mode_set(__searchbarItem, ELM_OBJECT_SELECT_MODE_NONE);

				CtListFastIndexData *indexData = (CtListFastIndexData*)eina_list_nth(__fastIndexList, 0);
				if (indexData) {
					elm_object_item_data_set(indexData->item, (void*)__searchbarItem);
				}
			}
		}
		else {
			WDEBUG("searchbar re link to fastscroll");
			CtListFastIndexData *indexData = (CtListFastIndexData*)eina_list_nth(__fastIndexList, 0);
			if (indexData) {
				elm_object_item_data_set(indexData->item, (void*)__searchbarItem);
			}
		}
	}

	return __searchbarItem;
}

Elm_Object_Item* CtListView::__addSelectAllItem(void)
{
	WHIT();
	if(__hasSelectAll == true) {
		int listCount = __listModel->getPersonListCount(LIST_QUERY_DEFAULT);
		// in remove favorites view, it needs to show MFC
		if (__ctrlType == LIST_CTRL_REMOVE_FAVORITES) {
			listCount += __listModel->getContactsCount(LIST_QUERY_MFC);
		}
		if(__selectAllItem == NULL && listCount > 0) {
			WDEBUG("selectall added");
			CtListDataItem *itemData = new CtListDataItem;
			itemData->personData = NULL;
			itemData->listView = this;
			itemData->item = elm_genlist_item_append(__genlist, __selectAllItc, itemData, NULL, ELM_GENLIST_ITEM_NONE, __onSelectCheckAll, this);
			__selectAllItem = itemData->item;
		}
	}

	return __selectAllItem;
}

Elm_Object_Item* CtListView::__addMyProfileItem(void)
{
	WHIT();
	if (__hasMyProfile && __onSearching == false) {
		if(__myProfileItem == NULL) {
			WDEBUG("my profile added");
			Elm_Object_Item *item = NULL;
			item = elm_genlist_item_append(__genlist, __groupTitleItc, SAFE_STRDUP(V_("IDS_PB_HEADER_ME")), NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			__myProfileItem = elm_genlist_item_append(__genlist, __myProfileItc, this, NULL, ELM_GENLIST_ITEM_NONE, __onMyProfileSelectCb, this);
		}
		else {
			WDEBUG("my profile updated");
			elm_object_item_data_set(__myProfileItem, (void*)this);
			elm_genlist_item_update(__myProfileItem);
		}
	}

	return __myProfileItem;
}

int CtListView::__addFavoriteList(void)
{
	WHIT();
	int favorIndex = 0;
	if(__hasFavoriteList && __onSearching == false) {
		// for get favorite by order
		if(__listModel->getPersonListCount(LIST_QUERY_FAVORITE) > 0) {
			WDEBUG("favorite list added");
			std::shared_ptr<CtListDataPerson> dataPerson;
			CtListFastIndexData *indexData = (CtListFastIndexData*)eina_list_nth(__fastIndexList, 1);

			Elm_Object_Item *curItem = __getStandardItem(LIST_SUB_FAVORITE);

			if(__favoriteTitleItem == NULL)
				__favoriteTitleItem = __addFavoriteTitleItem(curItem);
			if(indexData)
				elm_object_item_data_set(indexData->item, (void*)__favoriteTitleItem);

			curItem = elm_genlist_item_next_get(__favoriteTitleItem);
			while(true){
				dataPerson = __listModel->getAtPersonData(favorIndex, LIST_QUERY_FAVORITE);

				if(dataPerson == NULL)
					break;
				CtListDataItem *itemData = new CtListDataItem;
				itemData->personData = dataPerson;
				itemData->listView = this;
				itemData->item = __addFavoriteItem(curItem, itemData);
				favorIndex++;

				curItem = elm_genlist_item_next_get(itemData->item);
			}

			__deleteRestItems(curItem, false);
		}
		else {
			Elm_Object_Item *curItem = elm_genlist_item_next_get(__favoriteTitleItem);
			elm_object_item_del(__favoriteTitleItem);
			__favoriteTitleItem = NULL;
			__deleteRestItems(curItem, false);

			// if no favorites, search bar replace
			CtListFastIndexData *indexData = (CtListFastIndexData*)eina_list_nth(__fastIndexList, 1);
			if (indexData)
				elm_object_item_data_set(indexData->item, (void*)__searchbarItem);
		}
	}

	return favorIndex;
}

Elm_Object_Item* CtListView::__addFavoriteItem(Elm_Object_Item *curItem, CtListDataItem* itemData)
{
	Elm_Object_Item *item = NULL;
	if(curItem == NULL) {
		// append item
//		WDEBUG("append"); // remove cause of too many printing logs
		item = elm_genlist_item_append(__genlist, __listItc, itemData, NULL, ELM_GENLIST_ITEM_NONE,__onSelectItem, this);
	}
	else {
		if(elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
			// update item
			WDEBUG("update");
			elm_object_item_data_set(curItem, itemData);
			item = curItem;
		}
		else {
			// insert before item
			WDEBUG("insert");
			item = elm_genlist_item_insert_before(__genlist, __listItc, itemData, NULL, curItem, ELM_GENLIST_ITEM_NONE, __onSelectItem, this);
		}
	}
	return item;
}

int CtListView::__addMfcList(void)
{
	WHIT();
	int mfcIndex = 0;
	if(__hasMfcList && __onSearching == false) {
		if (__listModel->getAtPersonData(0, LIST_QUERY_MFC)) {
			WDEBUG("mfc list added");
			std::shared_ptr<CtListDataPerson> dataPerson;

			Elm_Object_Item *curItem = __getStandardItem(LIST_SUB_MFC);

			if(__mfcTitleItem == NULL)
				__mfcTitleItem = __addMfcTitleItem(curItem);

			curItem = elm_genlist_item_next_get(__mfcTitleItem);
			while(true){
				if (mfcIndex > 5)
					break;

				dataPerson = __listModel->getAtPersonData(mfcIndex, LIST_QUERY_MFC);
				if(dataPerson == NULL)
					break;
				CtListDataItem *itemData = new CtListDataItem;
				itemData->personData = dataPerson;
				itemData->listView = this;
				itemData->item = __addMfcItem(curItem, itemData);
				mfcIndex++;

				curItem = elm_genlist_item_next_get(itemData->item);
			}

			__deleteRestItems(curItem, false);
		}
		else {
			Elm_Object_Item *curItem = elm_genlist_item_next_get(__mfcTitleItem);
			elm_object_item_del(__mfcTitleItem);
			__mfcTitleItem = NULL;
			__deleteRestItems(curItem, false);
		}
	}
	return mfcIndex;
}

Elm_Object_Item* CtListView::__addMfcItem(Elm_Object_Item *curItem, CtListDataItem* itemData)
{
	Elm_Object_Item *item = NULL;
	if(curItem == NULL) {
		// insert item
		item = elm_genlist_item_append(__genlist, __listItc, itemData, NULL, ELM_GENLIST_ITEM_NONE,__onSelectItem, this);
	}
	else {
		if(elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
			// update item
			elm_object_item_data_set(curItem, itemData);
			item = curItem;
		}
		else {
			// insert before item
			item = elm_genlist_item_insert_before(__genlist, __listItc, itemData, NULL, curItem, ELM_GENLIST_ITEM_NONE, __onSelectItem, this);
		}
	}
	return item;
}

Elm_Object_Item* CtListView::__addFavoriteTitleItem(Elm_Object_Item *curItem)
{
	WHIT();
	Elm_Object_Item *item = NULL;

	if(curItem == NULL) {
		WDEBUG("favorite group title added");
		item = elm_genlist_item_append(__genlist, __favoriteTitleItc, this, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
	}
	else {
		if(__favoriteTitleItem == curItem) {
			WDEBUG("favorite group title exist");
			item = curItem;
		}
		else {
			WDEBUG("favorite group title insert before");
			item = elm_genlist_item_insert_before(__genlist, __favoriteTitleItc, this, NULL, curItem, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
		}
	}
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item;
}

Elm_Object_Item* CtListView::__addMfcTitleItem(Elm_Object_Item *curItem)
{
	WHIT();
	Elm_Object_Item *item = NULL;

	if(curItem == NULL) {
		WDEBUG("mfc group title added");
		item = elm_genlist_item_append(__genlist, __mfcTitleItc, this, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
	}
	else {
		if(__favoriteTitleItem == curItem) {
			WDEBUG("mfc group title exist");
			item = curItem;
		}
		else {
			WDEBUG("mfc group title insert before");
			item = elm_genlist_item_insert_before(__genlist, __mfcTitleItc, this, NULL, curItem, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
		}
	}
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item;
}

int CtListView::__addSearchList(int offset)
{
	WENTER();
	WPRET_VM(__onSearching == false, 0, "no onSearching");

	int index = 0;
	if (__hasDefaultList == true) {
		WDEBUG("main list added");
		std::shared_ptr<CtListDataPerson> dataPerson;

		Elm_Object_Item *curItem = NULL;

		if(__selectAllItem) {
			WDEBUG("__selectAllItem");
			curItem = __selectAllItem;
			curItem = elm_genlist_item_next_get(curItem);
		}
		else {
			WDEBUG("__searchbarItem");
			curItem = __searchbarItem;
			curItem = elm_genlist_item_next_get(curItem);
		}

		//get searchbar item
		if(offset > 0) {
			int curIndex = 0;
			while(1) {
				curItem = elm_genlist_item_next_get(curItem);
				curIndex++;
				if(curIndex == offset)
					break;
			}
		}

		WDEBUG("1 curitem %p, offset %d", curItem, offset);

		while(true){
			CtListDataItem *itemData = new CtListDataItem;
			dataPerson = __listModel->getAtPersonData(__curAppendIndex, LIST_QUERY_DEFAULT);

			if(dataPerson == NULL) {
				delete itemData;
				break;
			}
			itemData->personData = dataPerson;
			itemData->listView = this;
			itemData->item = __addDefaultItem(curItem, itemData, false);
			__curAppendIndex++;
			index++;

			curItem = elm_genlist_item_next_get(itemData->item);
		}

		WDEBUG("2 curitem %p, offset %d", curItem, offset);

		if(offset > 0 || __curAppendIndex < 20)
			__deleteRestItems(curItem, true);

		if(__curAppendIndex == 0) {
			__showNoContentsLayout();
			__setNoContentsText(V_("IDS_PB_NPBODY_NO_RESULTS_FOUND_ABB"));
		} else {
			__hideNoContentsLayout();
		}

		WLEAVE();
		return __curAppendIndex;
	}
	WLEAVE();
	return 0;
}


int CtListView::__addDefaultList(int offset)
{
	WHIT();
	if(__onSearching == true) {
		return __addSearchList(offset);
	}

	int index = 0;
	if (__hasDefaultList == true) {
		WDEBUG("main list added");
		std::shared_ptr<CtListDataPerson> dataPerson;

		Elm_Object_Item *curItem = __getStandardItem(LIST_SUB_DEFAULT);

	if (__ctrlType == LIST_CTRL_REORDER_FAVORITES && curItem && elm_genlist_item_type_get(curItem) == ELM_GENLIST_ITEM_GROUP)
		curItem = NULL;

		if(offset > 0)
			curItem = NULL;

		if(__listModel->getAtPersonData(__curAppendIndex, LIST_QUERY_DEFAULT)) {
			elm_index_item_clear(__fastScroll);
			__freeFastIndexData();
			__fastIndexList = NULL;
			__appendFastScrollLetter(SYMBOL_MAGNIFIER);
			if(__favoriteTitleItem) {
				__appendFastScrollLetter(SYMBOL_STAR);
			}
		}
		__linkItemsManually(eina_list_nth_list(__fastIndexList, 0), __searchbarItem);
		if(__favoriteTitleItem) {
			__linkItemsManually(eina_list_nth_list(__fastIndexList, 1), __favoriteTitleItem);
		}

		while(true){
			dataPerson = __listModel->getAtPersonData(__curAppendIndex, LIST_QUERY_DEFAULT);

			if(dataPerson == NULL) {
				break;
			}
			CtListDataItem *itemData = new CtListDataItem;
			itemData->personData = dataPerson;
			itemData->listView = this;
			itemData->item = __addDefaultItem(curItem, itemData, true);
			__curAppendIndex++;
			index++;

			curItem = elm_genlist_item_next_get(itemData->item);
		}

		if(index > 0)
			__deleteRestItems(curItem, true);

		if(__curAppendIndex == 0) {
			if(__onSearching) {
				__showNoContentsLayout();
				__setNoContentsText(V_("IDS_PB_NPBODY_NO_RESULTS_FOUND_ABB"));
			}
			else {
				if (__hasDefaultList == true) {
					if(!__onNoContentLayout) {
						Elm_Object_Item *curItem = __getStandardItem(LIST_SUB_DEFAULT);
						while(curItem) {
							Elm_Object_Item *item = elm_genlist_item_next_get(curItem);
							elm_object_item_del(curItem);
							curItem = item;
						}
						__showNoContentsLayout();
					}
					__setNoContentsText(V_("IDS_PB_NPBODY_NO_CONTACTS"), V_("IDS_PB_BODY_AFTER_YOU_CREATE_CONTACTS_THEY_WILL_BE_SHOWN_HERE"));
				}
			}
		} else {
			__hideNoContentsLayout();
		}
		return __curAppendIndex;
	}

	return 0;

}

Elm_Object_Item* CtListView::__addDefaultItem(Elm_Object_Item *curItem, CtListDataItem* itemData, bool needGroupTitle)
{
	WENTER();
	Elm_Object_Item *item = NULL;
	if(needGroupTitle)
		item = __appendGroupItem(curItem, itemData);

	if(item)
		item = elm_genlist_item_next_get(item);
	else
		item = curItem;

	if(item == NULL) {
		// insert item
		//WDEBUG("append");
		item = elm_genlist_item_append(__genlist, __listItc, itemData, NULL, ELM_GENLIST_ITEM_NONE,__onSelectItem, this);
	}
	else {
		if(elm_genlist_item_type_get(item) == ELM_GENLIST_ITEM_GROUP)  {
			// insert before item
			//WDEBUG("insert");
			Elm_Object_Item *prev_item = elm_genlist_item_prev_get(item);
			item = elm_genlist_item_insert_after(__genlist, __listItc, itemData, NULL, prev_item, ELM_GENLIST_ITEM_NONE, __onSelectItem, this);
		}
		else {
			// update item
			//WDEBUG("update");
			CtListDataItem *lastData = (CtListDataItem *)elm_object_item_data_get(item);
			if(lastData)
				delete lastData;
			elm_object_item_data_set(item, itemData);
		}
	}

	if(__topItemPersonId > 0) {
		int person_id = itemData->personData->getPersonId();
		if(__topItemPersonId == person_id)
			elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
	}

	WLEAVE();
	return item;
}


Elm_Object_Item* CtListView::__getStandardItem(CtListSubListType listType)
{
	WDEBUG("list type %d", listType);
	Elm_Object_Item *curItem = NULL;
	if(listType == LIST_SUB_FAVORITE) {
		if(__favoriteTitleItem) {
			WDEBUG("__favoriteTitleItem");
			curItem = __favoriteTitleItem;
		} else if(__myProfileItem) {
			WDEBUG("__myProfileItem");
			curItem = elm_genlist_item_next_get(__myProfileItem);
		} else if(__selectAllItem) {
			WDEBUG("__selectAllItem");
			curItem = elm_genlist_item_next_get(__selectAllItem);
		} else if(__searchbarItem) {
			WDEBUG("__searchbarItem");
			curItem = elm_genlist_item_next_get(__searchbarItem);
		} else {
			WDEBUG("firstItem");
			curItem = elm_genlist_first_item_get(__genlist);
		}
	} else if(listType == LIST_SUB_MFC) {
		if(__mfcTitleItem) {
			WDEBUG("__mfcTitleItem");
			curItem = __mfcTitleItem;
		} else if(__favoriteTitleItem) {
			WDEBUG("__favoriteTitleItem");
			curItem = elm_genlist_item_next_get(__favoriteTitleItem);
			while(curItem && elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
				curItem = elm_genlist_item_next_get(curItem);
			}
		} else if(__myProfileItem) {
			WDEBUG("__myProfileItem");
			curItem = elm_genlist_item_next_get(__myProfileItem);
		} else if(__selectAllItem) {
			WDEBUG("__selectAllItem");
			curItem = elm_genlist_item_next_get(__selectAllItem);
		} else if(__searchbarItem) {
			WDEBUG("__searchbarItem");
			curItem = elm_genlist_item_next_get(__searchbarItem);
		} else {
			WDEBUG("firstItem");
			curItem = elm_genlist_first_item_get(__genlist);
		}
	} else if(listType == LIST_SUB_DEFAULT) {

		if(__onSearching) {
			if(__selectAllItem) {
				WDEBUG("__selectAllItem onSearching");
				curItem = elm_genlist_item_next_get(__selectAllItem);
			} else {
				WDEBUG("__searchbarItem onSearching");
				curItem = elm_genlist_item_next_get(__searchbarItem);
			}
			return curItem;
		}

		if(__mfcTitleItem) {
			WDEBUG("__mfcTitleItem");
			curItem = elm_genlist_item_next_get(__mfcTitleItem);
			while(curItem && elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
				curItem = elm_genlist_item_next_get(curItem);
			}
		} else if(__favoriteTitleItem) {
			WDEBUG("__favoriteTitleItem");
			 curItem = elm_genlist_item_next_get(__favoriteTitleItem);
			while(curItem && elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
				curItem = elm_genlist_item_next_get(curItem);
			}
		} else if(__myProfileItem) {
			WDEBUG("__myProfileItem");
			curItem = elm_genlist_item_next_get(__myProfileItem);
		} else if(__selectAllItem) {
			WDEBUG("__selectAllItem");
			curItem = elm_genlist_item_next_get(__selectAllItem);
		} else if(__searchbarItem) {
			WDEBUG("__searchbarItem");
			curItem = __searchbarItem;
			curItem = elm_genlist_item_next_get(curItem);
		} else {
			WDEBUG("firstItem");
			curItem = elm_genlist_first_item_get(__genlist);
		}
	}

	return curItem;
}

void CtListView::__deleteRestItems(Elm_Object_Item *curItem, bool all)
{
	WDEBUG("all %d", all);
	if(curItem) {
		int index = 0;
		Elm_Object_Item *item = NULL;
		while(curItem) {
			if(all == false) {
				if(elm_genlist_item_type_get(curItem) == ELM_GENLIST_ITEM_GROUP)
					break;
			}
			item = elm_genlist_item_next_get(curItem);
			elm_object_item_del(curItem);
			curItem = item;
			index++;
		}
		WDEBUG("%d items deleted", index);
	}
}


void CtListView::__appendGenlistItems(int offset)
{
	WENTER();

	WDEBUG("offset : %d", offset);

	int mainIndex = 0;
	int mfcIndex = 0;
	int favorIndex = 0;
	std::shared_ptr<CtListDataPerson> dataPerson;

	if(offset == 0) {
		// initial before add items
		__initListItems();

		//add searchbar
		__addSearchbarItem();

		//add select all
	   __addSelectAllItem();

		//add my profile
		__addMyProfileItem();

		//add Favorites list
		favorIndex = __addFavoriteList();

		//add MFC list
		mfcIndex = __addMfcList();

		//add favorite group title
		if(__hasFavGroupTitle)
			__addFavoriteTitleItem(NULL);
	}

	//add main list
	mainIndex = __addDefaultList(offset);

	WDEBUG("%d, %d, %d", favorIndex, mfcIndex, mainIndex);

	if (__onSearching == true || __listModel->getPersonListCount(LIST_QUERY_DEFAULT) == 0)
		__hideFastScroll();
	else if(__hasFastscroll)
		__showFastScroll();

	if(__launched == 1 && __onSearching == false) {
		elm_genlist_item_update(__searchbarItem);
		__launched = -1;
		WDEBUG("launched");
	}

	WLEAVE();
}

Elm_Object_Item *CtListView::__appendGroupItem(Elm_Object_Item *curItem, CtListDataItem* itemData)
{
	WENTER();
	if(__onSearching == true)
		return NULL;

	if(__hasGroupTitle == false)
		return NULL;

	Elm_Object_Item *item = NULL;
	std::string curIndex;

	if (NULL == itemData->personData->getDisplayName())
		curIndex = "#";
	else{
		if(itemData->personData->getDisplayIndex())
			curIndex = itemData->personData->getDisplayIndex();
		else
			return NULL;
	}

	if(__curGroupIndex != curIndex) {
		if(curItem == NULL) {
			WDEBUG("append %s", curIndex.c_str());
			item = elm_genlist_item_append(__genlist, __groupTitleItc, strdup(curIndex.c_str()), NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
		}
		else {
			if(elm_genlist_item_type_get(curItem) != ELM_GENLIST_ITEM_GROUP) {
				WDEBUG("insert %s", curIndex.c_str());
				item = elm_genlist_item_insert_before(__genlist, __groupTitleItc, strdup(curIndex.c_str()), NULL, curItem, ELM_GENLIST_ITEM_GROUP, NULL, NULL);
			}
			else {
				WDEBUG("update %s", curIndex.c_str());
				elm_object_item_data_set(curItem, strdup(curIndex.c_str()));
				item = curItem;
			}
		}
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

		__appendFastScrollLetter(curIndex);

		__curGroupIndex = curIndex;
		__curGroupTitleItem = item;
		//link to index
		__linkItemToIndex(__curGroupTitleItem);
	}
	elm_index_level_go(__fastScroll, 0);

	WLEAVE();
	return item;
}

void CtListView::__onEntryFocused(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	CtListView* listView = (CtListView*) data;
	eext_object_event_callback_add(obj, EEXT_CALLBACK_BACK, listView->__onEntryBackButton, listView);

	if (SAFE_STRLEN(elm_entry_entry_get(obj)) > 0) {
		WDEBUG("on searching");
		return;
	}

	if (listView->__hasCreateButton) {
		listView->__hideCreateButton();
	}
}

void CtListView::__onEntryUnfocused(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	CtListView* listView = (CtListView*) data;

	if (listView->__hasCreateButton) {
		if (elm_entry_is_empty(obj)) {
			listView->__showCreateButton();
		}
	}
}

void CtListView::__onEntryBackButton(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	CtListView* listView = (CtListView*) data;
	eext_object_event_callback_del(obj, EEXT_CALLBACK_BACK, __onEntryBackButton);
	elm_entry_entry_set(obj, NULL);
	elm_object_focus_set(obj, EINA_FALSE);
	listView->__showFastScroll();
}

void CtListView::__onCancelButtonClicked(void *data, Evas_Object *obj, void *event_info)
{
	WENTER();

	CtListView* listView = (CtListView*) data;

	listView->__onShowMemberList = false;

	listView->__curAppendIndex = 0;
	listView->__appendGenlistItems(0);
	// after all item appended, link reset items to fastscroll
	listView->__linkItemToRestIndex();
}

void CtListView::__reorderButtonMouseDownCb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	elm_genlist_reorder_mode_set((Evas_Object *)data, EINA_TRUE);
}

void CtListView::__setReorderItemClassStyle()
{
	WPRET_M(__listItc, "already set");

	__listItc = elm_genlist_item_class_new();
	WPRET_M(!__listItc, "elm_genlist_item_class_new() failed");
	__listItc->item_style = "type1";
	__listItc->func.del = [](void *data, Evas_Object *obj)->void
	{
		delete static_cast<CtListDataItem*>(data);
	};
	__listItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			const char *name = itemData->personData->getDisplayName();
			if (name)
				return elm_entry_utf8_to_markup(name);
			else
				return SAFE_STRDUP(V_("IDS_LOGS_MBODY_UNKNOWN"));

		}
		return NULL;
	};
	__listItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.icon")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *icon = createThumbnail(obj, THUMBNAIL_98, itemData->personData->getImagePath(), true);
			return icon;
		}
		else if (0 == strcmp(part, "elm.swallow.end")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *button = elm_button_add(obj);
			elm_object_style_set(button, "icon_reorder");
			evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
			evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

			evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_DOWN, __reorderButtonMouseDownCb, itemData->listView->__genlist);

			return button;
		}
		else
			return NULL;
	};

}

void CtListView::__setCheckItemClassStyle()
{
	WPRET_M(__listItc, "already set");

	__listItc = elm_genlist_item_class_new();
	WPRET_M(!__listItc, "elm_genlist_item_class_new() failed");
	__listItc->item_style = "type1";
	__listItc->func.del = [](void *data, Evas_Object *obj)->void
	{
		delete static_cast<CtListDataItem*>(data);
	};
	__listItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			if (SAFE_STRLEN(itemData->listView->__listModel->getSearchString()) > 0){
				std::string name = ContactsCommon::getHighlightText(itemData->personData->getDisplayName(), itemData->listView->__listModel->getSearchString());
				if (!name.empty())
					return strdup(name.c_str());
				else
					return SAFE_STRDUP(V_("IDS_LOGS_MBODY_UNKNOWN"));
			}
			else{
				const char *name = NULL;
				name = itemData->personData->getDisplayName();
				if(name)
					return elm_entry_utf8_to_markup(name);
				else
					return SAFE_STRDUP(V_("IDS_LOGS_MBODY_UNKNOWN"));
			}
		}
		return NULL;
	};
	__listItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.icon")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *icon = createThumbnail(obj, THUMBNAIL_98, itemData->personData->getImagePath(), true);
			return icon;
		}
		else if (0 == strcmp(part, "elm.swallow.end")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *check = elm_check_add(obj);
			elm_check_state_set(check, itemData->personData->isChecked());
			evas_object_repeat_events_set(check, EINA_TRUE);
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_freeze_events_set (check, EINA_TRUE);
			return check;
		}
		else
			return NULL;
	};

}

void CtListView::__setGroupTitleItemClassStyle()
{
	WPRET_M(__groupTitleItc, "already set");

	__groupTitleItc = elm_genlist_item_class_new();
	WPRET_M(!__groupTitleItc, "elm_genlist_item_class_new() failed");

	__groupTitleItc->item_style = "group_index";

	__groupTitleItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		char *groupIndex = (char*)data;
		if (0 == strcmp(part, "elm.text") && groupIndex && *groupIndex)
			return strdup(groupIndex);
		else
			return NULL;
	};
	__groupTitleItc->func.del = [](void *data, Evas_Object *obj)->void
	{
		char *groupIndex = (char*)data;
		free(groupIndex);
	};
	__groupTitleItc->func.content_get = NULL;

}

void CtListView::__setFavoriteTitleItemClassStyle()
{
	WPRET_M(__favoriteTitleItc, "already set");

	__favoriteTitleItc = elm_genlist_item_class_new();
	WPRET_M(!__favoriteTitleItc, "elm_genlist_item_class_new() failed");

	__favoriteTitleItc->item_style = "group_index";

	__favoriteTitleItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if (0 == strcmp(part, "elm.text"))
			return strdup(V_("IDS_PB_HEADER_FAVOURITES"));
		else
			return NULL;
	};
	__favoriteTitleItc->func.del = NULL;
	__favoriteTitleItc->func.content_get = NULL;
}

void CtListView::__setMfcTitleItemClassStyle()
{
	WPRET_M(__mfcTitleItc, "already set");

	__mfcTitleItc = elm_genlist_item_class_new();
	WPRET_M(!__mfcTitleItc, "elm_genlist_item_class_new() failed");

	__mfcTitleItc->item_style = "group_index";

	__mfcTitleItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if (0 == strcmp(part, "elm.text"))
			return strdup(V_("IDS_PB_HEADER_MOST_FREQUENT_CONTACTS_ABB2"));
		else
			return NULL;
	};
	__mfcTitleItc->func.del = NULL;
	__mfcTitleItc->func.content_get = NULL;

}

void CtListView::__setDefaultItemClassStyle()
{
	WENTER();
	WPRET_M(__listItc, "already set");

	__listItc = elm_genlist_item_class_new();
	WPRET_M(!__listItc, "elm_genlist_item_class_new() failed");
	__listItc->item_style = "type1";
	__listItc->func.del = [](void *data, Evas_Object *obj)->void
	{
		delete static_cast<CtListDataItem*>(data);
	};
	__listItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			if(SAFE_STRLEN(itemData->listView->__listModel->getSearchString()) > 0){
				std::string name = ContactsCommon::getHighlightText(itemData->personData->getDisplayName(), itemData->listView->__listModel->getSearchString());
				if (!name.empty())
					return strdup(name.c_str());
				else
					return SAFE_STRDUP(V_("IDS_LOGS_MBODY_UNKNOWN"));
			}
			else{
				const char *name = NULL;
				name = itemData->personData->getDisplayName();
				if(name)
					return elm_entry_utf8_to_markup(name);
				else
					return SAFE_STRDUP(V_("IDS_LOGS_MBODY_UNKNOWN"));
			}
		}
		return NULL;
	};
	__listItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.icon")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *icon = createThumbnail(obj, THUMBNAIL_98, itemData->personData->getImagePath(), true);
			return icon;
		}
		return NULL;
	};

}

char *CtListView::__getMyProfileNumber() const
{
	telephony_handle_list_s list;
	int err = telephony_init(&list);
	WPRET_VM(err != TELEPHONY_ERROR_NONE, nullptr, "telephony_init() failed(%d)", err);

	int sim_count = list.count;
	char *ret = nullptr;

	for (int i=0; i < sim_count; ++i) {
		char *number = nullptr;

		err = telephony_sim_get_subscriber_number(list.handle[i], &number);
		if (TELEPHONY_ERROR_NONE == err && number && *number) {
			ret = number;
			break;
		}
		free(number);
	}

	err = telephony_deinit(&list);
	WPWARN(err != TELEPHONY_ERROR_NONE, "telephony_deinit() failed(%d)", err);

	return ret;
}

void CtListView::__setMyProfileItemClassStyle()
{
	WENTER();

	WPRET_M(__myProfileItc, "already set");

	__myProfileItc = elm_genlist_item_class_new();
	WPRET_M(!__myProfileItc, "elm_genlist_item_class_new() failed");
	__myProfileItc->item_style = "type1";
	__myProfileItc->func.del = NULL;
	__myProfileItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		CtListView *self = static_cast<CtListView*>(data);
		WPRET_VM(!self, NULL, "invalid this pointer");
		int err = CONTACTS_ERROR_NONE;
		contacts_list_h myprofileList = NULL;
		int count = 0;
		contacts_record_h record = NULL;
		char* displayName = NULL;

		if(!strcmp(part, "elm.text")) {
			err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

			contacts_list_get_count(myprofileList, &count);
			if (count > 0) {
				err = contacts_list_get_current_record_p(myprofileList, &record);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);

				err = contacts_record_get_str(record, _contacts_my_profile.display_name, &displayName);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str() Failed(%d)", err);

				if (displayName) {
					ctCommonRemoveNewline(displayName);
					if (*displayName) {
						return displayName;
					}

					free(displayName);
				}
			} else {
				char *number = self->__getMyProfileNumber();
				if (number && *number) {
					return number;
				}

				free(number);
			}

			err = contacts_list_destroy(myprofileList, true);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_destroy() Failed(%d)", err);

			return SAFE_STRDUP(V_("IDS_PB_MBODY_SET_MY_PROFILE"));
		}
		return NULL;
	};
	__myProfileItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.icon")) {
			char* imagePath = NULL;
			int err = CONTACTS_ERROR_NONE;
			contacts_list_h myprofileList = NULL;
			int count = 0;
			contacts_record_h record = NULL;

			err = contacts_db_get_all_records(_contacts_my_profile._uri, 0, 0, &myprofileList);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_db_get_all_records() Failed(%d)", err);

			err = contacts_list_get_count(myprofileList, &count);
			WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_count() Failed(%d)", err);
			if (count > 0) {
				err = contacts_list_get_current_record_p(myprofileList, &record);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_list_get_current_record_p() Failed(%d)", err);

				err = contacts_record_get_str_p(record, _contacts_my_profile.image_thumbnail_path, &imagePath);
				WPWARN(CONTACTS_ERROR_NONE != err, "contacts_record_get_str_p() Failed(%d)", err);
			}

			Evas_Object *icon = createThumbnail(obj, THUMBNAIL_98, imagePath, true);
			contacts_list_destroy(myprofileList, true);
			return icon;
		}
		return NULL;
	};
}

Evas_Object* CtListView::__setNoContentsLayout(Evas_Object* parent)
{
	Evas_Object* nocontents = elm_layout_add(parent);
	elm_layout_theme_set(nocontents, "layout", "nocontents", "default");
	return nocontents;
}

void CtListView::__setNoContentsText(std::string text, std::string helptext)
{
	if (!__noContentLayout) {
		return;
	}
	elm_object_part_text_set(__noContentLayout, "elm.text",  text.c_str());
	elm_object_part_text_set(__noContentLayout, "elm.help.text", helptext.c_str());
	elm_layout_signal_emit(__noContentLayout, "align.center", "elm");
}

void CtListView::__hideNoContentsLayout()
{
	if (!__noContentLayout || !__onNoContentLayout) {
		return;
	}
	__onNoContentLayout = false;
	evas_object_size_hint_weight_set(__noContentLayout, 0.0, 0.0);
	evas_object_size_hint_align_set(__noContentLayout, 0.0, 0.0);
	evas_object_hide(__noContentLayout);
	evas_object_size_hint_weight_set(__genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_content_min_limit(__genlist, EINA_FALSE, EINA_FALSE);
}

void CtListView::__showNoContentsLayout()
{
	if (!__noContentLayout || __onNoContentLayout) {
		return;
	}
	__onNoContentLayout = true;
	evas_object_size_hint_weight_set(__noContentLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(__noContentLayout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(__noContentLayout);
	elm_scroller_content_min_limit(__genlist, EINA_FALSE, EINA_TRUE);
	evas_object_size_hint_weight_set(__genlist, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(__genlist, EVAS_HINT_FILL, 0.0);
}

void CtListView::__setSelectAllItemClassStyle()
{
	WHIT();
	WPRET_M(__selectAllItc, "already set");

	__selectAllItc = elm_genlist_item_class_new();
	WPRET_M(!__selectAllItc, "elm_genlist_item_class_new() failed");
	__selectAllItc->item_style = "type1";
	__selectAllItc->func.del = [](void *data, Evas_Object *obj)->void
	{
		delete static_cast<CtListDataItem*>(data);
	};
	__selectAllItc->func.text_get = [](void* data, Evas_Object* obj, const char* part)->char*
	{
		if(!strcmp(part, "elm.text")) {
			return SAFE_STRDUP(V_("IDS_PB_MBODY_SELECT_ALL"));
		}
		return NULL;
	};
	__selectAllItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.end")) {
			CtListDataItem* itemData = (CtListDataItem*)data;
			Evas_Object *check = elm_check_add(obj);
			elm_check_state_set(check, (Eina_Bool)itemData->listView->__selectAllState);
			evas_object_repeat_events_set(check, EINA_TRUE);
			evas_object_propagate_events_set(check, EINA_FALSE);
			evas_object_freeze_events_set (check, EINA_TRUE);
			itemData->listView->__selectAllObj = check;

			return check;
		}
		else
			return NULL;
	};
}

void CtListView::__onSipUpCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtListView *listView = (CtListView*)data;
	listView->__sipStatus = true;
}

void CtListView::__onSipDownCb(void *data, Evas_Object *obj, void *event_info)
{
	WHIT();

	CtListView *listView = (CtListView*)data;
	listView->__sipStatus = false;
}

void CtListView::__setSearchbarItemClassStyle()
{
	WHIT();
	WPRET_M(__searchbarItc, "already set");

	__searchbarItc = elm_genlist_item_class_new();
	WPRET_M(!__searchbarItc, "elm_genlist_item_class_new() failed");
	__searchbarItc->item_style = "full";
	__searchbarItc->func.del = NULL;
	__searchbarItc->func.content_get = [](void* data, Evas_Object* obj, const char* part)->Evas_Object*
	{
		if (0 == strcmp(part, "elm.swallow.content")) {
			CtListView* listView = (CtListView*)data;

			static Elm_Entry_Filter_Limit_Size filter;
			WEditfield *editfield = new WEditfield();
			editfield->create(listView->__genlist, NULL);

			Evas_Object *entry = editfield->getEntry();

			elm_object_domain_translatable_part_text_set(entry, "elm.guide", PACKAGE, "IDS_PB_NPBODY_SEARCH");

			const char *search = listView->__listModel->getSearchString();
			if(search) {
				elm_entry_entry_set(entry, search);
			}

			evas_object_smart_callback_add(entry, "changed", listView->__onChangedSearchEditField, listView);
			evas_object_smart_callback_add(entry, "preedit,changed", listView->__onChangedSearchEditField, listView);
			evas_object_smart_callback_add(entry, "activated", listView->__onSearchbarActivatedCb, listView);
			evas_object_smart_callback_add(entry, "focused", listView->__onEntryFocused, listView);
			evas_object_smart_callback_add(entry, "unfocused", listView->__onEntryUnfocused, listView);

			Evas_Object *clearBtn = editfield->getClearButton();
			evas_object_smart_callback_add(clearBtn, "clicked", listView->__onChangedSearchEditField, listView);

			elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
			filter.max_char_count = CT_SEARCHBAR_MAX_CHAR;
			elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &filter);

			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);

			return editfield->getEvasObj();
		} else {
			return NULL;
		}
	};
}

void CtListView::__setItemClassStyle()
{
	WDEBUG("__viewType %d", __viewType);

	// set searchbar item class
	__setSearchbarItemClassStyle();

	// set group item class
	__setGroupTitleItemClassStyle();

	// set favorite title item class
	__setFavoriteTitleItemClassStyle();

	// set mfc title item class
	__setMfcTitleItemClassStyle();

	// set my profile item class
	__setMyProfileItemClassStyle();

	// set item class
	switch(__viewType){
	case LIST_VIEW_DEFAULT :
		__setDefaultItemClassStyle();
		break;
	case LIST_VIEW_CHECK :
		__setSelectAllItemClassStyle();
		__setCheckItemClassStyle();
		break;
	case LIST_VIEW_REORDER :
		__setReorderItemClassStyle();
		break;
	}
}

void CtListView::__initViewCondition(CtListViewType viewType, CtListCtrlType ctrlType)
{
	__hasSearchbar = true;
	__hasGroupTitle = true;
	__hasFastscroll = true;
	__hasDefaultList = true;
	__hasFavoriteList = true;
	__hasMfcList = true;

	WDEBUG("viewType %d, ctrlType %d",viewType, ctrlType);
	char buf[CT_TEXT_SHORT_LEN] = {0,};
	switch(viewType) {
	case LIST_VIEW_CHECK:
		snprintf(buf, sizeof(buf), ctGetContactsString("IDS_PB_HEADER_PD_SELECTED_ABB3"), 0);
		setTitle(buf);
		__hasTitle = true;
		__hasDoneCancel = true;
		__hasFavoriteList = true;
		__hasMfcList = true;
		break;
	case LIST_VIEW_REORDER:
		setTitle(V_("IDS_PB_OPT_REORDER"));
		__hasTitle = true;
		__hasDoneCancel = true;
		break;
	default:
		__hasCreateButton = true;
		break;
	}

	switch(ctrlType) {
	case LIST_CTRL_DEFAULT:
		__hasMenu = true;
		__hasMyProfile = true;
		break;
	case LIST_CTRL_DELETE_CONTACTS:
	case LIST_CTRL_SHARE_CONTACTS:
		__hasToolbar = true;
		__hasSelectAll = true;
		__hasFavoriteList = true;
		__hasMfcList = true;
		break;
	case LIST_CTRL_ADD_FAVORITES:
		__hasSelectAll = true;
		__hasFavoriteList = false;
		__hasMfcList = false;
		break;
	case LIST_CTRL_REORDER_FAVORITES:
		__hasSearchbar = false;
		__hasFastscroll = false;
		__hasGroupTitle = false;
		__hasFavGroupTitle = true;
		__hasFavoriteList = false;
		__hasMfcList = false;
		break;
	case LIST_CTRL_REMOVE_FAVORITES:
		__hasSearchbar = false;
		__hasFastscroll = false;
		__hasGroupTitle = false;
		__hasSelectAll = true;
		__hasDefaultList = false;
		__hasFavoriteList = true;
		__hasMfcList = true;
		break;
	default:
		break;
	}
}

const int CtListView::getListTopItemPersonId()
{
	Evas_Coord x, y;
	Elm_Object_Item *item = NULL;
	int nthItem = -1;
	int person_id = -1;
	if (__fastLayout) {
		evas_object_geometry_get(__fastLayout, &x, &y, NULL, NULL);
		WDEBUG("layout_fastscroll x = %d, y = %d", x, y);
		if (__genlist) {
			item = elm_genlist_at_xy_item_get(__genlist, x, y+40, NULL);
			if(item) {
				while (elm_genlist_item_type_get(item) == ELM_GENLIST_ITEM_GROUP) {
					item = elm_genlist_item_next_get(item);
				}
				if(__searchbarItem == item 	||__myProfileItem == item) {
					WDEBUG("top of list is searchbar or myprofile item");
					return -1;
				}

				int countFavorite = __listModel->getContactsCount(LIST_QUERY_FAVORITE);
				if(countFavorite>0) ++countFavorite;
				int countMFC = __listModel->getContactsCount(LIST_QUERY_MFC);
				if(countMFC>0) ++countMFC;
				nthItem = elm_genlist_item_index_get (item);
				WDEBUG("nthItem %d", nthItem);
				if(nthItem <=  (1+countFavorite+countMFC) )
					return -1;

				CtListDataItem *dataItem = (CtListDataItem*)elm_object_item_data_get(item);
				if(dataItem) {
					person_id = dataItem->personData->getPersonId();
				}
			}
		}
	}
	WDEBUG("person_id %d", person_id);

	return person_id;
}

void CtListView::setOnProgressOperation(bool onProgress)
{
	WDEBUG("onProgress %d", onProgress);
	__onProgressOperation = onProgress;
}

bool CtListView::__checkValidSpecialCharForNumber(char character)
{
	if(character=='*'
		||character=='#'
		||character=='+'
		||character==';'
		||character==','
		||character=='-')
		return true;
	else
		return false;
}

bool CtListView::__checkValidCharForNumber(char character)
{
	if(isdigit(character) || __checkValidSpecialCharForNumber(character))
		return true;
	else
		return false;
}

char* CtListView::__getSearchHighlightNumber(const char *src)
{

	WPRET_VM(!src, NULL, "src string is null.");
	if(SAFE_STRLEN(__listModel->getSearchString()) <= 0)
		return SAFE_STRDUP(src);


	int pos1 = -1;
	int pos2 = -1;
	int pos = 0;
	const char *s = src;
	const char *r = __listModel->getSearchString();

	const char *match = strstr(s, r);
	if(match) {
		pos1 = match - s;
		pos2 = pos1 + strlen(r);
	}
	else {
		for (s=src;'\0' != *s;s++) {
			if (__checkValidCharForNumber(*s)) {
				WDEBUG("a %s", s);

				if (*s == *r || __checkValidSpecialCharForNumber(*s)) {
					if (-1 == pos1) {
						pos1 = pos;
					}
					pos2 = pos+1;

					if(__checkValidSpecialCharForNumber(*s) == false)
						r++;

					if ('\0' == *r)
						break;
				}
				else {
					r =  __listModel->getSearchString();
					pos1 = -1;
					pos2 = -1;
					WDEBUG("b %s", r);
				}
			}
			pos++;
		}
	}

	if (0 <= pos1 && 0 <= pos2)
	{
		char display[CT_TEXT_MAX_LEN] = {0};
		char first[CT_TEXT_MAX_LEN] = {0};
		char middle[CT_TEXT_MAX_LEN] = {0};
		char color_set_middle[CT_TEXT_MAX_LEN] = {0};
		int r = 0, g = 0, b = 0, a = 255;

		snprintf(first, pos1+1, "%s", src);
		snprintf(middle, (pos2 - pos1)+1, "%s", (char *)(src+pos1));
		snprintf(color_set_middle, CT_TEXT_MAX_LEN, "<color=#%02x%02x%02x%02x>%s</color>", r, g, b, a, middle);
		snprintf(display, sizeof(display), "%s%s%s", first, color_set_middle, (char *)(src+pos2));

		return strdup(display);
	}
	return SAFE_STRDUP(src);
}
