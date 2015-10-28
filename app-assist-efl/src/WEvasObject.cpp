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

#include "WEvasObject.h"
#include "WDebugInternal.h"


class __WEvasObjectImpl : public IWEvasObject
{
public:
	__WEvasObjectImpl(Evas_Object* obj):__evasObj(obj) { }

public:
	virtual Evas_Object* getEvasObj() { return __evasObj; }

	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
	{
		auto p = (__WEvasObjectImpl*)data;
		delete p;
	}

	std::shared_ptr<IWEvasObject> __selfPtr;
	Evas_Object* __evasObj;
};

std::weak_ptr<IWEvasObject> WEvasObject_getWeakPtr( Evas_Object* obj )
{
	auto pImpl = new __WEvasObjectImpl(obj);
	std::shared_ptr<IWEvasObject> sp( pImpl,[](IWEvasObject* p){} );
	pImpl->__selfPtr = sp;

	evas_object_event_callback_add( obj, EVAS_CALLBACK_DEL, __WEvasObjectImpl::__objDelCb, pImpl);

	return std::weak_ptr<IWEvasObject>(pImpl->__selfPtr);
}

