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

#ifndef UTILS_CALLBACK_MANAGER_H
#define UTILS_CALLBACK_MANAGER_H

#include <vector>

namespace Utils
{
	/**
	 * @brief Provides possibility to store and invoke several callbacks at once.
	 * @param[in]   Args    Callback arguments types
	 */
	template <typename... Args>
	class CallbackManager
	{
	public:
		/**
		 * @brief Callback prototype.
		 */
		typedef void(*CallbackType)(void *, Args...);

		/**
		 * @brief Callback and data pair.
		 */
		typedef std::pair<CallbackType, void *> Callback;

		/**
		 * @return Whether there is no added callbacks.
		 */
		bool isEmpty() const;

		/**
		 * @brief Add new callback.
		 * @param[in]   callback    Callback and data pair.
		 */
		void addCallback(Callback callback);

		/**
		 * @brief Remove added callback.
		 * @param[in]   callback    Callback and data pair.
		 */
		void removeCallback(Callback callback);

		/**
		 * @brief Invoke all added callbacks.
		 */
		void invokeCallbacks(Args... args);

	private:
		typedef std::vector<Callback> Callbacks;
		Callbacks m_Callbacks;
	};
}

#include "Utils/CallbackManagerImpl.h"

#endif /* UTILS_CALLBACK_MANAGER_H */
