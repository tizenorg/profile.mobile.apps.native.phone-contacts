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

#ifndef _PH_LOG_DROP_DOWN_CTX_POPUP_H_
#define _PH_LOG_DROP_DOWN_CTX_POPUP_H_

#include <functional>
#include <vector>

#include <Evas.h>
#include <Elementary.h>

#include "WControl.h"
//#include "WNaviframe.h"

class PhLogDropDownCtxPopup : public WControl
{
public:
	PhLogDropDownCtxPopup();

	Elm_Object_Item* appendItem( const char *label, const std::function< void()>& selectCb );
	void showPopup();

protected:
	virtual ~PhLogDropDownCtxPopup();
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onDestroy();

	static void __popupSelectCb( void* data, Evas_Object* obj, void* event_info );

private:
	Evas_Object* __parentButton;

	std::vector<std::function< void()>> __vSelectItemCb;
};

#endif //_PH_LOG_DROP_DOWN_CTX_POPUP_H_
