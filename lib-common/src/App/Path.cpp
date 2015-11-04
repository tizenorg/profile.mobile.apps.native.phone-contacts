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

#include "App/Path.h"

#include <app.h>

const std::string &App::getLocaleDir()
{
	static std::string localeDir;
	if (localeDir.empty()) {
		#ifdef LOCALEDIR
			localeDir = LOCALEDIR;
		#else
			localeDir = getResourcePath("locale/");
		#endif
	}

	return localeDir;
}

const std::string &App::getResourceDir()
{
	static std::string resDir;
	if (resDir.empty()) {
		#ifdef RESDIR
			resDir = RESDIR;
		#else
			char *path = app_get_resource_path();
			resDir = path;
			free(path);
		#endif
	}

	return resDir;
}

std::string App::getResourcePath(const std::string &relativePath)
{
	return std::string(getResourceDir()).append(relativePath);
}
