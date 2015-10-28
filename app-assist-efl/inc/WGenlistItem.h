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

#ifndef _WGENLIST_ITEM_H_
#define _WGENLIST_ITEM_H_

#include <Evas.h>
#include <Elementary.h>
#include <functional>
#include "WDefine.h"

class WControl;
/**
* @class	WGenlistItem
* @brief	This class is
*
*
* The %WGenlistItem is
*/
class WAPP_ASSIST_EXPORT WGenlistItem
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WGenlistItem();
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WGenlistItem();
public:
	/**
	 * Override this method to create Elm_Genlist_Item_Class structure.
	 * It is used to append item to the genlist.
	 *
	 * @return	the pointer of Elm_Genlist_Item_Class created by elm_genlist_item_class_new.
	 * 			It should be free by elm_genlist_item_class_free if it is not used anymore.
	 * @see getItemClassStatic()
	 */
	virtual Elm_Genlist_Item_Class* getItemClassNew();
	/**
	 * Override this method to create Elm_Genlist_Item_Class structure.
	 * It is used to append item to the genlist.
	 *
	 * @return	the pointer of Elm_Genlist_Item_Class instantiated as static variable.
	 * 			It is not needed to be free.
	 * @see getItemClassNew()
	 */
	virtual Elm_Genlist_Item_Class* getItemClassStatic();
	/**
	 * Gets Elm_Object_Item of this item instance after it is appened to genlist.
	 *
	 * @return	the Elm_Object_Item
	 * @see setElmObjectItem()
	 */
	Elm_Object_Item* getElmObjectItem();
	/**
	 * Sets Elm_Object_Item, which is returned when this item is appended to genlist.
	 *
	 * @param[in]	objItem	The pointer of Elm_Object_Item
	 *
	 * @see getElmObjectItem()
	 */
	void setElmObjectItem( Elm_Object_Item* objItem );
	/**
	 * This is static callback function to be used to append item to the genlist.
	 * Then this will call onSelect( Elm_Object_Item* item ) method when this callback is invoked.
	 *
	 * @param[in]	data	genlist select callback parameter
	 * @param[in]	obj	genlist select callback parameter
 	 * @param[in]	event_info	genlist select callback parameter
	 *
	 * @see onSelect( Elm_Object_Item* item )
	 */
	static void selectCb(void *data, Evas_Object *obj, void *event_info);

protected:
	/**
	 * Override this method to do something when genlist is selected.
	 * Before this, "selectCb" should be set as select callback of genlist when this item is appended.
	 *
 	 * @param[in]	objItem	Elm_Object_Item of this instance
 	 *
	 * @see selectCb()
	 */
	virtual void onSelect( Elm_Object_Item* objItem );
	/**
	 * Attaches the pop-up to the view, which is the container view of this item.
	 *
	 * @param[in]	popup	The pointer to the pop-up instance to be attached
	 *
	 * @return	true the view is found and it is attached successfully.
	 */
	bool attachToView( WControl* popup );

private:
	WDECLARE_PRIVATE_IMPL(WGenlistItem);
	WDISABLE_COPY_AND_ASSIGN(WGenlistItem);

};

#define WGENLIST_ITEM_DEL() [](void* data, Evas_Object* obj) \
		{ \
			WGenlistItem* item = static_cast<WGenlistItem*>(data); \
			delete item; \
		}

#endif // _WGENLIST_ITEM_H_
