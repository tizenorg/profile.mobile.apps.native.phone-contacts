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

#ifndef COMMON_ACTIONS_H
#define COMMON_ACTIONS_H

#include "App/AppControl.h"

namespace Common
{
	/**
	 * @brief External actions that can be performed using certain model values.
	 */
	enum ActionType
	{
		ActionCall,     /**< Make a telephony call using value as a number */
		ActionMessage,  /**< Compose an SMS using value as recipient */
		ActionEmail,    /**< Compose an E-mail using value as an address */
		ActionUrl       /**< Open webpage in browser using value as URL */
	};

	/**
	 * @brief Request to perform an external action using specified value.
	 * @param[in]   action  Action to perform
	 * @param[in]   value   Action parameter (depending on the action)
	 * @return AppControl request wrapper.
	 */
	EXPORT_API App::AppControl requestAction(ActionType actionType, const char *value);
}

#endif /* COMMON_ACTIONS_H */
