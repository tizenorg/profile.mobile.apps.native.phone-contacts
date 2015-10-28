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

#ifndef _PH_LOG_GENLIST_HELPER_H_
#define _PH_LOG_GENLIST_HELPER_H_

#include "Elementary.h"
#include "WControl.h"
#include <functional>
#include <vector>

/**
 * @brief Provides access to logs genlist and its item data.
 */
class PhLogGenlistHelper : public WControl
{
public:
	/**
	 * @brief Creates log genlist helper.
	 *
	 * @param[in]   parent  Genlist parent
	 */
	PhLogGenlistHelper(Evas_Object *parent);

public:
	/**
	 * @brief Provides genlist item data.
	 */
	typedef struct {
		Elm_Object_Item *it;
		const void *data;
		const void *p;
		void *contents;
		Ecore_Timer *thumbnailUpdateTimer;
	} ItemData;

	/**
	 * @brief Provides genlist checked item data.
	 */
	typedef struct {
		Elm_Object_Item *it;
		const void *data;
		const void *p;
		Eina_Bool isChecked;
		Evas_Object *checkButton;
	} CheckedItemData;

	/**
	 * @brief Swipe direction type.
	 */
	typedef enum{
		SWIPE_TO_RIGHT,
		SWIPE_TO_LEFT
	}SwipeDirection;

public:
	/**
	 * @brief Gets genlist object.
	 *
	 * @return genlist object.
	 */
	Evas_Object *getEvasObj() { return __genlist; }

	/**
	 * @brief Appends item to the genlist with new ItemData.
	 *
	 * @param[in]   itc         The item class for the item
	 * @param[in]   data        The item data
	 * @param[in]   parent      The parent item
	 * @param[in]   type        The item type
	 * @param[in]   func        The convenience function that is called when the item is selected
	 * @param[in]   func_data   The data passed to @a func
	 *
	 * @return a handle to the added item, otherwise NULL if it is not possible.
	 *
	 * @see elm_genlist_item_append
	 */
	Elm_Object_Item *appendItem(
						const Elm_Genlist_Item_Class *itc,
						const void *data,
						Elm_Object_Item *parent,
						Elm_Genlist_Item_Type type,
						Evas_Smart_Cb func,
						const void *func_data);

	/**
	 * @brief Appends checked item to the genlist with new CheckedItemData.
	 *
	 * @param[in]   itc         The item class for the item
	 * @param[in]   data        The item data
	 * @param[in]   parent      The parent item
	 * @param[in]   type        The item type
	 * @param[in]   func        The convenience function that is called when the item is selected
	 * @param[in]   func_data   The data passed to @a func
	 * @param[in]   isChecked   Should item be checked or not
	 *
	 * @return a handle to the added item, otherwise NULL if it is not possible.
	 *
	 * @see elm_genlist_item_append
	 */
	Elm_Object_Item *appendCheckItem(
							const Elm_Genlist_Item_Class *itc,
							const void *data,
							Elm_Object_Item *parent,
							Elm_Genlist_Item_Type type,
							Evas_Smart_Cb func,
							const void *func_data,
							Eina_Bool isChecked = EINA_FALSE);

	/**
	 * @brief Finishes appending of the item and scrolls genlist to the first item.
	 */
	void appendFinish();

private:
	virtual Evas_Object* onCreate( Evas_Object* parent, void* param );
	virtual void onDestroy();

	static void __swipeStartCb(void *data, Elm_Object_Item *it, const char *em, const char *src);
	static void __swipeInvokeCb(void *data, Elm_Object_Item *it, const char *em, const char *src);
	static void __swipeCancleCb(void *data, Elm_Object_Item *it, const char *em, const char *src);
	static void __genlistRealizedCb(void *data, Evas_Object *obj, void *event_info);
	static void __genlistUnrealizedCb(void *data, Evas_Object *obj, void *event_info);
	static void __languageChangedCb(void *data, Evas_Object *obj, void *event_info);
	static void __longPressedCb(void *data, Evas_Object *obj, void *event_info);
	static Eina_Bool __scrollTimerCb(void *data);
	static void __scrollCb(void *data, Evas_Object *obj, void *event_info);
	static void __mouseUpCb(void *data, Evas *evas, Evas_Object *obj, void *event_info);

	Evas_Object* __createGenlist(void);
private:
	PhLogGenlistHelper();
	virtual ~PhLogGenlistHelper();

private:
	Evas_Object* __genlist;
	Evas_Object* __parent;
	std::function<void(SwipeDirection, void*)> __swipeHandler;
	Ecore_Timer* __scrollTimer;
	bool __isClearRequested;

	int __smartAppendIndex;
};

#endif //_PH_LOG_GENLIST_HELPER_H_

