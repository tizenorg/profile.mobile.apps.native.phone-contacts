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

#ifndef _WGENGRID_ITEM_H_
#define _WGENGRID_ITEM_H_

#include <Evas.h>
#include <Elementary.h>
#include <functional>
#include "WDefine.h"

class WControl;
/**
* @class	WGengridItem
* @brief	This class is
*
*
* The %WGengridItem is
*/
class WAPP_ASSIST_EXPORT WGengridItem
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WGengridItem();
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WGengridItem();
public:
	/**
	 * Override this method to create Elm_Gengrid_Item_Class structure.
	 * It is used to append item to the gengrid.
	 *
	 * @return	the pointer of Elm_Gengrid_Item_Class created by elm_gengrid_item_class_new.
	 * 			It should be free by elm_gengrid_item_class_free if it is not used anymore.
	 * @see getItemClassStatic()
	 */
	virtual Elm_Gengrid_Item_Class* getItemClassNew();
	/**
	 * Override this method to create Elm_Gengrid_Item_Class structure.
	 * It is used to append item to the gengrid.
	 *
	 * @return	the pointer of Elm_Gengrid_Item_Class instantiated as static variable.
	 * 			It is not needed to be free.
	 * @see getItemClassNew()
	 */
	virtual Elm_Gengrid_Item_Class* getItemClassStatic();
	/**
	 * Gets Elm_Object_Item of this item instance after it is appened to gengrid.
	 *
	 * @return	the Elm_Object_Item
	 * @see setElmObjectItem()
	 */
	Elm_Object_Item* getElmObjectItem();
	/**
	 * Sets Elm_Object_Item, which is returned when this item is appended to gengrid.
	 *
	 * @param[in]	objItem	The pointer of Elm_Object_Item
	 *
	 * @see getElmObjectItem()
	 */
	void setElmObjectItem( Elm_Object_Item* objItem );
	/**
	 * This is static callback function to be used to append item to the gengrid.
	 * Then this will call onSelect method when this callback is invoked.
	 *
	 * @param[in]	data	gengrid select callback parameter
	 * @param[in]	obj	gengrid select callback parameter
 	 * @param[in]	event_info	gengrid select callback parameter
	 *
	 * @see onSelect()
	 */
	static void selectCb(void *data, Evas_Object *obj, void *event_info);

protected:
	/**
	 * Override this method to do something when gengrid is selected.
	 * Before this, "selectCb" should be set as select callback of gengrid when this item is appended.
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
	WDECLARE_PRIVATE_IMPL(WGengridItem);
	WDISABLE_COPY_AND_ASSIGN(WGengridItem);

};

#define WGENGRID_ITEM_DEL() [](void* data, Evas_Object* obj) \
		{ \
			WGengridItem* item = static_cast<WGengridItem*>(data); \
			delete item; \
		}

#endif // _WGENGRID_ITEM_H_
