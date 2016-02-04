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

#ifndef __CT_LIST_VIEW_H_
#define __CT_LIST_VIEW_H_

#include <string>
#include <vector>
#include <list>
#include <memory>

#include "WNaviframe.h"

#include "ContactsCommon.h"
#include "IContactsTabPageView.h"
#include "CtType.h"
#include "CtView.h"
#include "ICtChangeListener.h"
#include "CtListController.h"
#include "CtThread.h"

class CtListModel;
class CtListDataPerson;

class CtListBasicController;
class CtListUgMultiPickController;
class CtListUgSinglePickController;

/**
 * @brief Specifies type of the sublist.
 */
typedef enum {
	LIST_SUB_DEFAULT,       /**< Main list */
	LIST_SUB_FAVORITE,      /**< Favorite list */
	LIST_SUB_MFC,           /**< MFC list */
}CtListSubListType;

/**
 * @brief Specifies selected mode of the list.
 */
enum CtListSelectMode {
	LIST_SELECT_SINGLE,             /**< Single mode */
	LIST_SELECT_SINGLE_OR_CREATE,   /**< Single mode with Create contact button */
	LIST_SELECT_MULTIPLE            /**< Multiple mode */
};

/**
 * @brief Specifies type of the resulting data.
 */
enum CtListResultType {
	LIST_RESULT_ID,                 /**< Contact ids */
	LIST_RESULT_NUMBER,             /**< Contact numbers */
	LIST_RESULT_NUMBER_FOR_CALL,    /**< Contact numbers */
	LIST_RESULT_NUMBER_FOR_MESSAGE, /**< Contact numbers */
	LIST_RESULT_EMAIL,              /**< Contact emails */
	LIST_RESULT_VCARD,              /**< Vcard path */
	LIST_RESULT_NUMBER_OR_EMAIL     /**< Contact numbers. If there are no any then contact emails */
};

/**
 * @brief Contacts list view.
 */
class WAPP_ASSIST_EXPORT CtListView : public CtView, public IContactsTabPageView, public ICtChangeListener
{
public:
	typedef std::function<void(const char*, CtListResultType)> SingleResultCb;
	typedef std::function<void(const char*[], size_t)> MultiResultCb;

	/**
	 * @brief Constructor.
	 *
	 * @param[in]   vcardPath   Vcard path
	 */
	CtListView(const char *vcardPath);

	/**
	 * @brief Constructor.
	 *
	 * @param[in]   selectMode   Selected mode
	 * @param[in]   resultType   Result type
	 * @param[in]   limit        Maximum of checked contacts. If 0 then no limit.
	 */
	CtListView(CtListSelectMode selectMode, CtListResultType resultType, int limit);

	/**
	 * @brief Constructor.
	 *
	 * @param[in]   viewType    Contacts list view type
	 * @param[in]   dataType    The type of the contacts to be in the list
	 * @param[in]   ctrlType    Request type of the list
	 */
	CtListView(CtListViewType viewType, CtListDataType dataType, CtListCtrlType ctrlType);

	/**
	 * @brief Gets class name.
	 *
	 * @return class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Gets parent window.
	 *
	 * @return Parent window evas object.
	 */
	Evas_Object* getViewWindow(void);

	/**
	 * @brief Gets naviframe.
	 *
	 * @return Naviframe evas object.
	 */
	Evas_Object* getViewNaviframe(void);

	/**
	 * @brief Gets the WNaviframe class instance.
	 *
	 * @return the pointer to the WNaviframe class instance.
	 *
	 * @see getNaviframe();
	 */
	WNaviframe* getWNaviframe(void);

	/**
	 * @brief Gets top item's person id.
	 *
	 * @return Person id. If the top item is searchbar, my profile, favorite contact or mfc contact item then -1.
	 */
	const int getListTopItemPersonId(void);

	/**
	 * @brief Sets top item on the list.
	 *
	 * @param[in]   personId    Id of the contact which will be on the top
	 */
	void setTopItemOnList(const int personId) {__topItemPersonId = personId;}

	/**
	 * @brief Sets on progress state.
	 *
	 * @param[in]   onProgress  Is on progress or not
	 */
	void setOnProgressOperation(bool onProgress);

	/**
	 * @brief Gets the genlist item by person id.
	 *
	 * @param[in]   personId    Person id of the contact
	 *
	 * @return Genlist's item
	 */
	Elm_Object_Item* getGenlistItemByPersonId(int personId);

	/**
	 * @brief Sets the single result's callback.
	 *
	 * @param[in]   handler Single result's callback.
	 */
	void setOnSingleResult(SingleResultCb handler);

	/**
	 * @brief Sets the multi result's callback.
	 *
	 * @param[in]   handler Multi result's callback.
	 */
	void setOnMultiResult(MultiResultCb handler);

	/**
	 * @brief Sends the result of single selection.
	 *
	 * @param[in]   data    Resulting data to be send.
	 * @param[in]   type    Type of the resulting data
	 */
	void sendSingleResult(const char *data, CtListResultType type) const;

	/**
	 * @brief Sends the result of multi selection.
	 *
	 * @param[in]   data    Resulting array data to be send.
	 * @param[in]   count   Size of the array
	 */
	void sendMultiResult(const char *data[], size_t count) const;

protected:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual void onEvent(int eventType);
	virtual void onSetAsPageContent(Elm_Object_Item* parentNaviItem);
	virtual void onTabSelect(Elm_Object_Item *naviItem);
	virtual void onTabUnselect(Elm_Object_Item *naviItem);
	virtual bool onTabPop();
	virtual void onTabScrollStart() {return;}
	virtual void onTabScrollStop() {return;}
	virtual bool onPop();

protected:
	virtual void onLanguageChanged();

private:

	virtual void onDbChanged(const char* viewUri);
	virtual void onCompleteGetContacts(int offset);
	virtual void onConditionChagned();

	typedef struct {
		int position;
		int skipped_count;
		char index[10];
		Elm_Object_Item *item;
	} CtListFastIndexData;


	struct CtListDataItem {
		Elm_Object_Item* item;
		CtListView *listView;
		std::shared_ptr<CtListDataPerson> personData;
	};

	void __resetCheckitems();
	void __initialize(void);
	void __hideFastScroll(void);
	void __showFastScroll(void);
	void __freeFastIndexData(void);
	void __appendFastScrollLetter(const std::string &str);
	void __showNoContent(void);
	void __showGenlist(void);
	void __updatetGenlistItem(void);
	void __showTitleButton(bool show);
	void __initViewCondition(CtListViewType viewType, CtListCtrlType ctrlType);
	void __appendGenlistItems(int offset);
	char* __getSearchHighlightNumber(const char *src);
	void __setItemClassStyle();
	void __updateCheckRelatedInfo();
	void __updateCheckAllRelatedInfo();
	void __initFastscrollData();

	//item style
	void __setReorderItemClassStyle(void);
	void __setCheckItemClassStyle(void);
	void __setDefaultItemClassStyle(void);
	void __setGroupTitleItemClassStyle(void);
	void __setMyProfileItemClassStyle(void);
	void __setSelectAllItemClassStyle(void);
	void __setSearchbarItemClassStyle(void);
	void __setFavoriteTitleItemClassStyle(void);
	void __setMfcTitleItemClassStyle(void);
#if 0
	void __setGalSearchItemClassStyle(void);
	void __setGalSearchTitleItemClassStyle(void);
	void __setGalSearchMoreItemClassStyle(void);
	void __setGalSearchProgressItemClassStyle(void);
#endif
	static Evas_Object* __createToolbar(Evas_Object *navi);

	//Group Index
	Eina_Unicode __hangulCompatibilityToJamo(Eina_Unicode unicode);
	bool __compareCharacter(const char *str1, const char *str2);
	void __linkItemsManually(Eina_List *indexItem, Elm_Object_Item *genItem);
	void __linkItemToIndex(Elm_Object_Item *groupItem);
	void __linkItemToRestIndex();


	Evas_Object* __createGenlist(Evas_Object* parent);
	Evas_Object* __createFastscroll(Evas_Object* parent);
	Evas_Object* __createSearchbar(Evas_Object* parent);
	Evas_Object* __createNoContent(Evas_Object* parent);
	Evas_Object* __createFloatBtn(Evas_Object* parent);
	void __cleanSearchbar(void);
	void __hideCreateButton(void);
	void __showCreateButton(void);
	Evas_Object* __setNoContentsLayout(Evas_Object* parent);
	void __setNoContentsText(std::string text, std::string helptext="");
	void __hideNoContentsLayout();
	void __showNoContentsLayout();

#if 0
	void __clearGalSearchItems();
#endif

private:

	char *__getDelPopupText();

	//view evas obj item callbacks
	static void __onSelectItem(void *data, Evas_Object *obj, void *event_info);
	static void __onMyProfileSelectCb(void *data, Evas_Object *obj, void *event_info);
	static void __onGenlistRealizedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onChangedSearchEditField(void *data, Evas_Object *obj, void *event_info);
	static void __onSearchbarActivatedCb(void *data, Evas_Object *obj, void *event_info);
	static void __onSipUpCb(void *data, Evas_Object *obj, void *event_info);
	static void __onSipDownCb(void *data, Evas_Object *obj, void *event_info);
	static void __onNaviTitleCancel(void *data, Evas_Object *obj, void *event_info);
	static void __onNaviTitleBack(void *data, Evas_Object *obj, void *event_info);
	static void __onNaviTitleDone(void *data, Evas_Object *obj, void *event_info);
	static void __onClickRemoveBtn(void *data, Evas_Object *obj, void *event_info);
	static void __onToolbarDone(void *data, Evas_Object *obj, void *event_info);
	static void __onSelectCheckAll(void *data, Evas_Object *obj, void *event_info);
	static void __onSelectMenu(void *data, Evas_Object *obj, void *event_info);
	static void __onSelectFloatBtn(void *data, Evas_Object *obj, void *event_info);
	static void __onEntryFocused(void *data, Evas_Object *obj, void *event_info);
	static void __onEntryUnfocused(void *data, Evas_Object *obj, void *event_info);
	static void __onEntryBackButton(void *data, Evas_Object *obj, void *event_info);
	static void __onCancelButtonClicked(void *data, Evas_Object *obj, void *event_info);

	static void __genlistReoderCb(void *data, Evas_Object *obj, void *event_info);

	static void __focusInCb(void *data, Evas_Object *obj, void *event_info);

#if 0
	static void __onSelectGalSearchMore(void *data, Evas_Object *obj, void *event_info);
	static Eina_Bool __galSearchTimeoutCb(void* data);
#endif

	//bool __checkMFC(int personId);
	//fast index callbacks
	static void __fastIndexChanged(void *data, Evas_Object *obj, void *event_info);
	static void __fastIndexSelected(void *data, Evas_Object *obj, void *event_info);

	static bool __checkValidSpecialCharForNumber(char character);
	static bool __checkValidCharForNumber(char character);

	static void __onGenlistScrollStartCb(void *data, Evas_Object *obj, void *event_info);
	static void __onGenlistScrollStopCb(void *data, Evas_Object *obj, void *event_info);
	static void __onNaviTransitionFinishedCb(void *data, Evas_Object *obj, void *event_info);

	static Eina_Bool __loadImageTimerCb(void *user_data);

	static void __onInitMyProfileThreadRunCb(void *data);
	static void __onInitMyProfileThreadEndCb(void *data);
	static Eina_Bool __onInitMyProfileIdlerCb(void *data);

	static void __reorderButtonMouseDownCb(void *data, Evas *evas, Evas_Object *obj, void *event_info);

	static Eina_Bool __createNocontentsPopupCb(void* data);

	static void __onSearchGetContactsThreadRunCb(void *data);
	static void __onSearchGetContactsThreadEndCb(void *data);

	char *__getMyProfileNumber() const;
	char* __getMsisdnFromSim();
	void __initMyProfile(void);

	void __initListItems(void);
	Elm_Object_Item* __addSearchbarItem(void);
	Elm_Object_Item* __addSelectAllItem(void);
	Elm_Object_Item* __addMyProfileItem(void);
	Elm_Object_Item* __addFavoriteTitleItem(Elm_Object_Item *curItem);
	Elm_Object_Item* __addFavoriteItem(Elm_Object_Item *curItem, CtListDataItem* itemData);
	Elm_Object_Item* __addMfcTitleItem(Elm_Object_Item *curItem);
	Elm_Object_Item* __addMfcItem(Elm_Object_Item *curItem, CtListDataItem* itemData);
	Elm_Object_Item* __addDefaultItem(Elm_Object_Item *curItem, CtListDataItem* itemData, bool needGroupTitle);
	Elm_Object_Item* __appendGroupItem(Elm_Object_Item *curItem, CtListDataItem* itemData);

	void __deleteRestItems(Elm_Object_Item *curItem, bool all);

	int __addFavoriteList(void);
	int __addMfcList(void);
	int __addDefaultList(int offset);
	int __addSearchList(int offset);

	Elm_Object_Item* __getStandardItem(CtListSubListType listType);

private:
	virtual ~CtListView();

private:
	Evas_Object* __baseLayout;
	Evas_Object* __fastLayout;
	Evas_Object* __fastScroll;
	Evas_Object* __genlist;
	Evas_Object* __selectAllObj;
	Evas_Object* __compose_btn;
	Evas_Object* __noContentLayout;

	Elm_Object_Item* __searchbarItem;
	Elm_Object_Item* __selectAllItem;
	Elm_Object_Item* __favoriteTitleItem;
	Elm_Object_Item* __mfcTitleItem;
	Elm_Object_Item* __myProfileItem;

#if 0
	Elm_Object_Item* __galSearchMoreItem;
	Ecore_Timer *__galSearchTimer;
#endif

	CtListController* __listController;
	CtListModel* __listModel;
	CtListViewType __viewType;
	CtListCtrlType __ctrlType;

	//for group title
	Elm_Object_Item *__curGroupTitleItem;
	Elm_Object_Item *__unmatchedGroupTitleItem;
	std::string __curGroupIndex;

	bool __onSearching;
	bool __onShowGroupButton;
	bool __onShowGroupList;
	bool __onShowMemberList;

	bool __sipStatus;
	bool __launching;
	bool __genlistScrolling;
	int __curAppendIndex;
	int __launched;
	bool __loaded;
	bool __selectedCheckAll;
	bool __onProgressOperation;
	bool __onNoContentLayout;

	int __updateQueue;
	Eina_List *__fastIndexList;
	std::vector<std::shared_ptr<CtListDataPerson>> __vcPersonData;

	typedef struct {
		int personId;
		int prevPersonId;
		int nextPersonId;
	} CtListFavoriteReorderData;
	std::vector <CtListFavoriteReorderData> __reorderData;

	//itc member
	Elm_Genlist_Item_Class* __listItc;
	Elm_Genlist_Item_Class* __groupTitleItc;
	Elm_Genlist_Item_Class* __myProfileItc;
	Elm_Genlist_Item_Class* __selectAllItc;
	Elm_Genlist_Item_Class* __searchbarItc;
	Elm_Genlist_Item_Class* __favoriteTitleItc;
	Elm_Genlist_Item_Class* __mfcTitleItc;

#if 0
	Elm_Genlist_Item_Class* __galSearchItc;
	Elm_Genlist_Item_Class* __galSearchTitleItc;
	Elm_Genlist_Item_Class* __galSearchMoreItc;
	Elm_Genlist_Item_Class* __galSearchProgressItc;
#endif

	//view condition
	bool __hasTitle;
	bool __hasSearchbar;
	bool __hasFastscroll;
	bool __hasToolbar;
	bool __hasCreateButton;
	bool __hasSelectAll;
	bool __hasTotalCount;
	bool __hasGroupTitle;
	bool __hasMenu;
	bool __hasDoneCancel;
	bool __hasFavGroupTitle;
	bool __hasMyProfile;
	bool __enableDone;
	bool __hasFavoriteList;
	bool __hasMfcList;
	bool __hasDefaultList;
	bool __hasGroupButton;

	CtCheckState __selectAllState;
	int __selectedPersonId;
	int __topItemPersonId;

#if 0
	int __galSearchOffset;
	bool __onUpdateGalSearch;
#endif

	CtThread *__myProfileThread;
	Ecore_Idler *__myProfileIdler;

	Ecore_Timer *__loadImageTimer;
	CtThread *__searchGetContactThread;

	void* __detailDllHandle;
	void* __cmDllHandle;

	bool __background;
	bool __isFirstGroupSync;
	bool __isFirstGroupMemberSync;

	SingleResultCb __singleResultCb;
	MultiResultCb __multiResultCb;

	friend class CtListBasicController;
	friend class CtListUgMultiPickController;
	friend class CtListUgSinglePickController;
	friend class CtListUgVcardViewController;
};

#endif
