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

#include "WPopup.h"
#include <Evas.h>
#include <efl_extension.h>
#include "WDebugInternal.h"
#include <string>

#define __MAX_BUTTON_NUM	3

class __WPopupImpl
{
public:
	__WPopupImpl();
	~__WPopupImpl();
public:
	//
	std::string __title;
	std::string __text;
	std::string __textDomain;
	WControl* __content;

	std::function<Evas_Object* (Evas_Object* parent)> __contentCreateCb;
	std::function<void(Evas_Object* popup)> __morePropertiesCb;
	std::function<void (bool* destroyPopup)> __backCb;

	std::function<void (bool* destroyPopup)> __vButtonCb[__MAX_BUTTON_NUM];
	std::function<void (Evas_Object* button)>* buttonPropertyCb[__MAX_BUTTON_NUM];

	std::string __listButtonLabel[__MAX_BUTTON_NUM];
	int __count;

};
__WPopupImpl::__WPopupImpl()
{
	__content = NULL;
	__count = 0;
	for( int i=0; i< __MAX_BUTTON_NUM; i++)
	{
		buttonPropertyCb[i] = NULL;
	}
}

__WPopupImpl::~__WPopupImpl()
{
	for( int i=0; i< __MAX_BUTTON_NUM; i++)
	{
		delete buttonPropertyCb[i];
	}
}


WPopup::WPopup() : __pv( new __WPopupImpl() )
{
	WHIT();
}
WPopup::WPopup( const char* title, const char* textContent ) : __pv( new __WPopupImpl() )
{
	if( title )
		__pv->__title = title;
	if( textContent )
		__pv->__text = textContent;
}

WPopup::~WPopup()
{
	WHIT();
	delete __pv;
}

void WPopup::setTitle( const char* title )
{
	if(title) {
		__pv->__title = title;
	} else {
		__pv->__title.clear();
	}

	Evas_Object *popup = getEvasObj();
	if(popup) {
		elm_object_part_text_set(popup, "title,text", __pv->__title.c_str());
	}
}

void WPopup::setContent( const char* text )
{
	if( text )
		__pv->__text = text;
	else
		__pv->__text.clear();
	if( __pv->__content )
	{
		__pv->__content->destroy();
		__pv->__content = NULL;
	}
}

void WPopup::setContent( WControl* control )
{
	__pv->__text = "";
	__pv->__content = control;
}

void WPopup::setContent( const std::function<Evas_Object* (Evas_Object* parent)> contentCreateCb )
{
	__pv->__text = "";
	if( __pv->__content )
	{
		__pv->__content->destroy();
		__pv->__content = NULL;
	}
	__pv->__contentCreateCb = contentCreateCb;
}

void WPopup::setTextTranslatable( const char* textDomain )
{
	if( textDomain )
		__pv->__textDomain = textDomain;
	else
		__pv->__textDomain.clear();
}

#define BUTTON_CALLBACK(_buttonIndex) \
[](void* data, Evas_Object* obj, void* eventInfo) {\
	WPopup* self = (WPopup*)data;\
	if (self->__pv->__vButtonCb[_buttonIndex]) { \
		bool destroyPopup = true; \
		self->__pv->__vButtonCb[_buttonIndex](&destroyPopup); \
		if( destroyPopup ) \
			evas_object_del(self->getEvasObj());\
	}\
	else \
		evas_object_del(self->getEvasObj());\
}

bool WPopup::addButton(const char* buttonText, const std::function<void (bool* destroyPopup)> buttonCb, const std::function<void (Evas_Object* button)> setPropertyCb )
{
	if( __pv->__count >= __MAX_BUTTON_NUM )
	{
		WERROR("Over the max button number! =%d", __pv->__count);
		return false;
	}
	__pv->__listButtonLabel[__pv->__count] = buttonText;
	__pv->__vButtonCb[__pv->__count] = buttonCb;
	if( setPropertyCb )
		__pv->buttonPropertyCb[__pv->__count] = new std::function<void (Evas_Object* button)>( setPropertyCb );

	__pv->__count++;
	return true;
}

void WPopup::setOnBack( const std::function<void (bool* destroyPopup)> backCb )
{
	__pv->__backCb = backCb;
}

void WPopup::setOnMoreProperties( const std::function<void(Evas_Object* popup)>& morePropertiesCb )
{
	__pv->__morePropertiesCb = morePropertiesCb;
}

Evas_Object* WPopup::onCreate(Evas_Object* parent, void* param)
{
	WHIT();

	Evas_Object* popup = elm_popup_add( parent );

	// It make the width to fit to screen width in portrait mode. 2014.09.30 modified as EFL notice.
	elm_popup_align_set( popup, ELM_NOTIFY_ALIGN_FILL, 1.0 );

	if( __pv->__backCb )
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, [](void *data, Evas_Object *obj, void *event_info){
			WPopup* p = (WPopup*)data;
			bool destroyPopup = true;
			p->__pv->__backCb( &destroyPopup );
			if( destroyPopup )
			{
				p->destroy();
			}
		}, this);
	else
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	if( __pv->__morePropertiesCb )
		__pv->__morePropertiesCb( popup );

	if (!__pv->__title.empty())
	{
		if (!__pv->__textDomain.empty())
			elm_object_domain_translatable_part_text_set(popup, "title,text", __pv->__textDomain.c_str(), __pv->__title.c_str());
		else
			elm_object_part_text_set(popup, "title,text", __pv->__title.c_str() );
	}

	if (!__pv->__text.empty())
	{
		if (!__pv->__textDomain.empty())
			elm_object_domain_translatable_part_text_set(popup, NULL, __pv->__textDomain.c_str(), __pv->__text.c_str());
		else
			elm_object_text_set(popup, __pv->__text.c_str());
	}
	else if( __pv->__content)
	{
		__pv->__content->create(popup, NULL);
		elm_object_content_set(popup, __pv->__content->getEvasObj());
	}
	else if( __pv->__contentCreateCb )
	{
		Evas_Object* obj = __pv->__contentCreateCb( popup );
		elm_object_content_set(popup, obj);
	}

	for( int i=0; i< __pv->__count; i++ )
	{
		Evas_Object* button = elm_button_add(popup);
		elm_object_style_set(button, "popup");
		if (!__pv->__textDomain.empty())
			elm_object_domain_translatable_part_text_set(button, NULL, __pv->__textDomain.c_str(), __pv->__listButtonLabel[i].c_str() );
		else
			elm_object_text_set(button, __pv->__listButtonLabel[i].c_str() );

		char partname[100] = {0};
		snprintf(partname, sizeof(partname) - 1, "button%d", i + 1);
		elm_object_part_content_set(popup, partname, button);
		//
		switch ( i ) {
			case 0:
				evas_object_smart_callback_add(button, "clicked", BUTTON_CALLBACK(0), this);
				break;
			case 1:
				evas_object_smart_callback_add(button, "clicked", BUTTON_CALLBACK(1), this);
				break;
			case 2:
				evas_object_smart_callback_add(button, "clicked", BUTTON_CALLBACK(2), this);
				break;
			default:
				break;
		}

		if( __pv->buttonPropertyCb[i] )
		{
			(*__pv->buttonPropertyCb[i])( button );
		}
		evas_object_show( button);
	}

	evas_object_show(popup);

	return popup;

}

void WPopup::onDestroy()
{
}

