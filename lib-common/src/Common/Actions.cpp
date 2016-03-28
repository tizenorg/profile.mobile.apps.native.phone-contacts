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

#include "Common/Actions.h"
#include <string>

namespace
{
	struct {
		const char *operation;
		const char *scheme;
	} actions[] = {
		/* ActionCall    = */ { APP_CONTROL_OPERATION_CALL, "tel:" },
		/* ActionMessage = */ { APP_CONTROL_OPERATION_COMPOSE, "sms:" },
		/* ActionEmail   = */ { APP_CONTROL_OPERATION_COMPOSE, "mailto:" },
		/* ActionUrl     = */ { APP_CONTROL_OPERATION_VIEW, "" }
	};
}

App::AppControl Common::requestAction(ActionType actionType, const char *value)
{
	auto action = actions[actionType];
	std::string uri = action.scheme;
	if (value) {
		uri.append(value);
	}

	return App::AppControl(action.operation, nullptr, uri.c_str());
}
