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

#ifndef UTILS_TRACER_H
#define UTILS_TRACER_H

#include <time.h>

namespace Utils
{
	/**
	 * @brief RAII logger. Writes logs when created and destroyed. Outputs elapsed time.
	 */
	class Tracer
	{
	public:
		Tracer(const char *tag, const char *file, const char *func, int line);
		~Tracer();

	private:
		const char *m_Tag;
		const char *m_File;
		const char *m_Func;
		timespec m_startTime;
	};
}

#endif /* UTILS_TRACER_H */
