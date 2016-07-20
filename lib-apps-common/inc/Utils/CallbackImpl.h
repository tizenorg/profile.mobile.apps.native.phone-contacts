/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CALLBACK_IMPL_H
#define CALLBACK_IMPL_H

namespace Utils
{
	template <typename F>
	class Callback;

	template <typename R, typename... A>
	class Callback<R(A...)>
	{
	public:
		template <class C, R(C::*Method)(A...)>
		static R stubWithFirstParam(void *object, A... args)
		{
			return (static_cast<C*>(object)->*Method)(args...);
		}

		template <class C, R(C::*Method)(A...)>
		static R stubWithLastParam(A... args, void *object)
		{
			return (static_cast<C*>(object)->*Method)(args...);
		}
	};

	template <typename R, typename C, typename... A>
	class Callback<R(C::*)(A...)>
	{
	public:
		typedef R(*WithFirstParam)(void*, A...);
		typedef R(*WithLastParam)(A..., void*);

		template <R(C::*Method)(A...)>
		static WithFirstParam makeWithFirstParam()
		{
			return &Callback<R(A...)>::template stubWithFirstParam<C, Method>;
		}

		template <R(C::*Method)(A...)>
		static WithLastParam makeWithLastParam()
		{
			return &Callback<R(A...)>::template stubWithLastParam<C, Method>;
		}
	};
}

#endif /* CALLBACK_IMPL_H */
