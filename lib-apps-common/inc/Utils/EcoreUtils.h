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

#ifndef UTILS_ECORE_UTILS_H
#define UTILS_ECORE_UTILS_H

#include <Ecore.h>
#include <functional>
#include <tizen.h>

namespace Utils
{
	/**
	 * @brief Job to be executed in main loop.
	 */
	typedef std::function<void()> Job;

	/**
	 * @brief Execute a functional object in main loop.
	 * @param[in]   job     Job to execute
	 * @return Ecore job handle.
	 */
	EXPORT_API Ecore_Job *createJob(Job job);
}

#endif /* UTILS_ECORE_UTILS_H */
