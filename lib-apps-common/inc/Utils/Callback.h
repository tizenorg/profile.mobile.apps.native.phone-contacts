/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef UTILS_CALLBACK_H
#define UTILS_CALLBACK_H

#include "Utils/CallbackImpl.h"

/**
 * @brief Make C-style callback from non-static class method with object as first parameter.
 * @details Generates static member function by adding void* first parameter.
 *          Generated function forwards calls to specified method on object passed as first parameter.
 * @par example
 * @code
 *
 * class SomeView : public View
 * {
 *     ...
 * private:
 *     void onButtonPressed(Evas_Object *obj, void *eventInfo);
 * }
 *
 * void SomeView::someMethod(Evas_Object *parent)
 * {
 *     Evas_Object *button = elm_button_add(parent);
 *     evas_object_smart_callback_add(button, "clicked",
 *             makeCallback(&SomeView::onButtonPressed), this);
 *     ...
 * }
 *
 * @endcode
 */
#define makeCallback(method) \
	::Utils::Callback<decltype(method)>::makeWithFirstParam<method>()

/**
 * @brief Make C-style callback from non-static class method with object as last parameter.
 * @see makeCallback()
 */
#define makeCallbackWithLastParam(method) \
	::Utils::Callback<decltype(method)>::makeWithLastParam<method>()

#endif /* UTILS_CALLBACK_H */
