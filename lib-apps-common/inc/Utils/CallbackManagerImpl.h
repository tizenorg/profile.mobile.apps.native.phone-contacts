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

#ifndef UTILS_CALLBACK_MANAGER_IMPL_H
#define UTILS_CALLBACK_MANAGER_IMPL_H

#include "Utils/CallbackManager.h"
#include <algorithm>

namespace Utils
{
	template <typename... Args>
	bool CallbackManager<Args...>::isEmpty() const
	{
		return m_Callbacks.empty();
	}

	template <typename... Args>
	void CallbackManager<Args...>::addCallback(Callback callback)
	{
		m_Callbacks.push_back(callback);
	}

	template <typename... Args>
	void CallbackManager<Args...>::removeCallback(Callback callback)
	{
		auto it = std::find(m_Callbacks.begin(), m_Callbacks.end(), callback);
		if (it != m_Callbacks.end()) {
			m_Callbacks.erase(it);
		}
	}

	template <typename... Args>
	void CallbackManager<Args...>::invokeCallbacks(Args... args)
	{
		/* FIXME: This might fail if removeCallback() or addCallback() is called during invocation */
		for (auto &&callback : m_Callbacks) {
			callback.first(callback.second, args...);
		}
	}
}

#endif /* UTILS_CALLBACK_MANAGER_IMPL_H */
