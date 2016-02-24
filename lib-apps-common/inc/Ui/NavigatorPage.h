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

#ifndef UI_NAVIGATOR_PAGE_H
#define UI_NAVIGATOR_PAGE_H

#include <Elementary.h>
#include <tizen.h>

namespace Ui
{
	/**
	 * @brief Navigator page interface
	 */
	class EXPORT_API NavigatorPage
	{
	public:
		virtual ~NavigatorPage() { }

		/**
		 * @brief Set page title.
		 * @param[in]   title   Page title
		 */
		virtual void setTitle(const char *title) = 0;

		/**
		 * @brief Set page style.
		 * @param[in]   style   Page style
		 */
		virtual void setStyle(const char *style) = 0;

		/**
		 * @brief Set page part content.
		 * @param[in]   content Content to set to page part
		 */
		virtual void setContent(const char *part, Evas_Object *content) = 0;
	};
}

#endif /* UI_NAVIGATOR_PAGE_H */
