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

#ifndef UI_THUMBNAIL_H
#define UI_THUMBNAIL_H

#include "Ui/Control.h"

namespace Ui
{
	class Thumbnail : public Control
	{
	public:
		enum Size
		{
			SizeSmall,
			SizeMedium,
			SizeLarge
		};

		/**
		 * @brief Create thumbnail
		 * @param[in]   size    Thumbnail size
		 */
		Thumbnail(Size size);

		/**
		 * @brief Allows method overload instead of shadowing
		 */
		using Control::create;

		/**
		 * @brief Create thumbnail
		 * @param[in]   parent  Thumbnail parent
		 * @param[in]   size    Thumbnail size
		 * @param[in]   path    Thumbnail Image path or nullptr for default image
		 * @return Created thumbnail
		 */
		static Thumbnail *create(Evas_Object *parent, Size size, const char *path = nullptr);

		/**
		 * @brief Set thumbnail image path
		 * @param[in]   path    Image path or nullptr for default image
		 */
		void setImagePath(const char *path);

	protected:
		virtual Evas_Object *onCreate(Evas_Object *parent) override;

	private:
		Size m_Size;
		Evas_Object *m_Image;
	};
}

#endif /* UI_THUMBNAIL_H */
