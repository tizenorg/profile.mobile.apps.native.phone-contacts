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

#ifndef CONTACTS_RESULT_PROVIDER_H
#define CONTACTS_RESULT_PROVIDER_H

#include "ResultObject.h"

#include <functional>
#include <tizen.h>

namespace Contacts
{
	class EXPORT_API ResultProvider
	{
	public:
		/**
		 * @brief Insert callback
		 * @param[in] Result object
		 */
		typedef std::function<void(ResultPtr)> InsertCallback;

		/**
		 * @brief Change callback
		 * @param[in] Result object
		 */
		typedef std::function<void(ResultPtr)> UpdateCallback;

		/**
		 * @brief Delete callback
		 */
		typedef std::function<void()> DeleteCallback;

		virtual ~ResultProvider() { }

		/**
		 * @return result list
		 */
		virtual ResultList getResultList() const = 0;

		/**
		 * @brief Set insert callback
		 * @param[in]   callback    Create callback
		 */
		virtual void setInsertCallback(InsertCallback callback) = 0;

		/**
		 * @brief Unset insert callback
		 */
		virtual void unsetInsertCallback() = 0;

		/**
		 * @brief Set update callback
		 * @param[in]   result      ResultObject object
		 * @param[in]   callback    Update callback
		 */
		virtual void setUpdateCallback(const ResultObject &result, UpdateCallback callback) = 0;

		/**
		 * @brief Unset update callback
		 * @param[in]   result      ResultObject object
		 */
		virtual void unsetUpdateCallback(const ResultObject &result) = 0;

		/**
		 * @brief Set delete callback
		 * @param[in]   result      ResultObject object
		 * @param[in]   callback    Delete callback
		 */
		virtual void setDeleteCallback(const ResultObject &result, DeleteCallback callback) = 0;

		/**
		 * @brief Unset delete callback
		 * @param[in]   result      ResultObject object
		 */
		virtual void unsetDeleteCallback(const ResultObject &result) = 0;
	};
}

#endif /* CONTACTS_RESULT_PROVIDER_H */
