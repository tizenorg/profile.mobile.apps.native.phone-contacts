/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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


#include "CommonThumbnail.h"
#include "ContactsCommon.h"
#include "ContactsThumbnail.h"

const char *groupDefaultThumbnail[] = {
	GROUP_DEFAULT_THUMBNAIL_166,
	GROUP_DEFAULT_THUMBNAIL_98,
	GROUP_DEFAULT_THUMBNAIL_240
};

const char *groupThumbnail[] = {
	GROUP_THUMBNAIL_166,
	GROUP_THUMBNAIL_98,
	GROUP_THUMBNAIL_240
};

const int thumbnailSize[] = {
	166,
	98,
	240
};

Evas_Object *createThumbnail(Evas_Object *parent, ThumbnailType thumbnailType, const char *thumbnailPath, bool setSize)
{
	Evas_Object *thumbnail = NULL;
	static const std::string edjPath = ContactsCommon::getResourcePath(COMMON_THUMBNAIL_EDJ);

	if(!thumbnailPath || (*thumbnailPath == '\0')) {

		thumbnail = elm_image_add(parent);
		elm_image_file_set(thumbnail, edjPath.c_str(), groupDefaultThumbnail[thumbnailType]);
		if (setSize) {
			evas_object_size_hint_min_set(thumbnail,
				ContactsCommon::getScaledWidth(thumbnailSize[thumbnailType]),
				ContactsCommon::getScaledHeight(thumbnailSize[thumbnailType]));
		}

	} else {
		thumbnail = elm_layout_add(parent);
		elm_layout_file_set(thumbnail, edjPath.c_str(), groupThumbnail[thumbnailType]);

		Evas_Object *image = elm_image_add(thumbnail);
		elm_image_file_set(image, thumbnailPath, nullptr);
		elm_image_aspect_fixed_set(image, EINA_TRUE);
		elm_image_fill_outside_set(image, EINA_TRUE);
		if (setSize) {
			evas_object_size_hint_min_set(image,
				ContactsCommon::getScaledWidth(thumbnailSize[thumbnailType]),
				ContactsCommon::getScaledHeight(thumbnailSize[thumbnailType]));
		}
		elm_object_part_content_set(thumbnail, PART_SWALLOW_IMAGE, image);
	}

	return thumbnail;
}
