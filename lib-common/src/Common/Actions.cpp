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

#include "Common/Actions.h"
#include "Utils/Range.h"

#include <cstring>
#include <string>

#define ACTION_SCHEME_COUNT 2

using namespace Utils;

namespace
{
	struct {
		const char *operation;
		Range<const char *> schemes[ACTION_SCHEME_COUNT];
	} actions[] = {
		/* ActionCall    = */ { APP_CONTROL_OPERATION_CALL,    { makeRange("tel:"),    { } } },
		/* ActionMessage = */ { APP_CONTROL_OPERATION_COMPOSE, { makeRange("sms:"),    makeRange("mmsto:") } },
		/* ActionEmail   = */ { APP_CONTROL_OPERATION_COMPOSE, { makeRange("mailto:"), { } } },
		/* ActionUrl     = */ { APP_CONTROL_OPERATION_VIEW,    { makeRange("http://"), makeRange("https://") } }
	};
}

App::AppControl Common::requestAction(ActionType actionType, const char *value)
{
	auto action = actions[actionType];
	std::string uri;
	if (value) {

		bool hasScheme = false;
		for (auto &&scheme : action.schemes) {
			if (scheme && strncmp(value, scheme.begin(), scheme.count() - 1) == 0) {
				hasScheme = true;
				break;
			}
		}

		if (!hasScheme) {
			uri.append(action.schemes[0].begin());
		}

		uri.append(value);
	}

	return App::AppControl(action.operation, nullptr, uri.c_str());
}
