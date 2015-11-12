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

#include "Ui/Thumbnail.h"
#include "Ui/Scale.h"

#include "App/Path.h"
#include "CommonThumbnail.h"

using namespace Ui;

namespace
{
	const struct
	{
		const char *groupMask;
		const char *groupDefault;
	} params[] = {
		{ GROUP_THUMBNAIL_SMALL, GROUP_THUMBNAIL_DEFAULT_SMALL },
		{ GROUP_THUMBNAIL_MEDIUM, GROUP_THUMBNAIL_DEFAULT_MEDIUM },
		{ GROUP_THUMBNAIL_LARGE, GROUP_THUMBNAIL_DEFAULT_LARGE }
	};

	const std::string layoutPath = App::getResourcePath(COMMON_THUMBNAIL_EDJ);
}

Thumbnail::Thumbnail(Size size)
	: m_Size(size), m_Image(nullptr)
{
}

Thumbnail *Thumbnail::create(Evas_Object *parent, Size size, const char *path)
{
	Thumbnail *thumbnail = new Thumbnail(size);
	thumbnail->create(parent);
	thumbnail->setImagePath(path);
	return thumbnail;
}

void Thumbnail::setImagePath(const char *path)
{
	if (path && *path) {
		elm_image_file_set(m_Image, path, nullptr);
	} else {
		elm_image_file_set(m_Image, layoutPath.c_str(), params[m_Size].groupDefault);
	}
}

Evas_Object *Thumbnail::onCreate(Evas_Object *parent)
{
	static int sizes[] = {
		getScaledValue(THUMBNAIL_SMALL_SIZE),
		getScaledValue(THUMBNAIL_MEDIUM_SIZE),
		getScaledValue(THUMBNAIL_LARGE_SIZE)
	};

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, layoutPath.c_str(), params[m_Size].groupMask);

	m_Image = elm_image_add(layout);
	elm_image_aspect_fixed_set(m_Image, EINA_TRUE);
	elm_image_fill_outside_set(m_Image, EINA_TRUE);

	int size = sizes[m_Size];
	evas_object_size_hint_min_set(m_Image, size, size);
	elm_object_part_content_set(layout, "elm.swallow.content", m_Image);

	return layout;
}
