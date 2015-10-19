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

#include "Utils/Tracer.h"
#include <dlog.h>

using namespace Utils;

void subtractTime(const timespec &a, const timespec &b, timespec &res)
{
	static const size_t ratio(1000000000);

	res.tv_sec = b.tv_sec - a.tv_sec;
	res.tv_nsec = b.tv_nsec - a.tv_nsec;
	if (res.tv_nsec < 0)
	{
		--res.tv_sec;
		res.tv_nsec += ratio;
	}
}

Tracer::Tracer(const char *tag, const char *file, const char *func, int line)
	: m_Tag(tag), m_File(file), m_Func(func)
{
	dlog_print(DLOG_DEBUG, m_Tag, "%s: %s(%d) <<< called", m_File, m_Func, line);
	clock_gettime(CLOCK_REALTIME, &m_startTime);
}

Tracer::~Tracer()
{
	timespec endTime;
	clock_gettime(CLOCK_REALTIME, &endTime);

	timespec res;
	subtractTime(m_startTime, endTime, res);

	dlog_print(DLOG_DEBUG, m_Tag, "%s: %s >>> returned in %d.%03d seconds",
		m_File, m_Func, res.tv_sec, res.tv_nsec / 1000000);
}
