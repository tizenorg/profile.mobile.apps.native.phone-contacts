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

#ifndef _CONTACTS_THUMBNAIL_H_
#define _CONTACTS_THUMBNAIL_H_

#include <Elementary.h>
#include "WDefine.h"

enum ThumbnailType {
	THUMBNAIL_166, /**< thumbnail with size 166 pixels */
	THUMBNAIL_98, /**< thumbnail with size 98 pixels */
	THUMBNAIL_240 /**< thumbnail with size 240 pixels */
};

/**
 * @brief Create thumbnail
 * @param[in]   parent          Parent object
 * @param[in]   thumbnailType   Type of thumbnail
 * @param[in]   thumbnailPath   Path to thumbnail
 * @param[in]   setSize         Set size according thumbnail type if true
 * @return The pointer to created evas object
 */
WAPP_ASSIST_EXPORT Evas_Object *createThumbnail(Evas_Object *parent, ThumbnailType thumbnailType, const char *thumbnailPath, bool setSize=false);


#endif /* _CONTACTS_THUMBNAIL_H_ */
