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

#ifndef _PH_COMMON_H_
#define _PH_COMMON_H_

#include "WDefine.h"
#include <Elementary.h>
#include <string>

/**
 * @brief Frees std list
 */
#define STD_LIST_FREE(target) \
	if(target)\
	{\
		for (auto iter = target->begin(); iter != target->end(); ++iter)\
		{\
			delete *iter;\
		}\
		delete target;\
		target = NULL; \
	}

/**
 * @brief Common methods
 */
class WAPP_ASSIST_EXPORT PhCommon
{
public:
	PhCommon();
	virtual ~PhCommon();

public:
	/**
	 * @brief Creates image.
	 *
	 * @param[in]   parent      The parent object
	 * @param[in]   edj         The edj file name that will be used as image source
	 * @param[in]   imagePath   The group that the image belongs in edje file. Or the image location path if the edj file is absent
	 *
	 * @return created image object
	 */
	static Evas_Object *createImage(Evas_Object *parent, const char *edj, const char *imagePath);

	/**
	 * @brief Creates layout.
	 *
	 * @param[in]   parent      The parent object
	 * @param[in]   edj         The edj file name that will be used as layout source
	 * @param[in]   layoutName  The group that the layout belongs in edje file
	 *
	 * @remark If @a edj and @a layoutName are NULL then default layout will be created.
	 *
	 * @return created layout object
	 */
	static Evas_Object *createLayout(Evas_Object *parent, const char *edj, const char *layoutName);

	/**
	 * @brief Creates button.
	 *
	 * @param[in]   parent          The parent object
	 * @param[in]   buttonStyle     The name of the style to use on the button
	 * @param[in]   buttonText      The text to be displayed on the button
	 * @param[in]   clickedCb       The smart callback on clicked event
	 * @param[in]   userData        The data to be passed to the callback function
	 *
	 * @return created button object
	 */
	static Evas_Object *createButton(Evas_Object *parent, const char *buttonStyle, const char *buttonText, Evas_Smart_Cb clickedCb, void *userData);

	/**
	 * @brief Launches call settings.
	 *
	 */
	static void launchCallSetting();

	/**
	 * @brief Adds speed dial number.
	 *
	 * @param[in]   speedNumber Number on the dialer
	 * @param[in]   numberId    ID of number to assign to @a speedNumber
	 *
	 * @return true on success, false if number with this @a numberId already exists.
	 */
	static bool addSpeedDialNumber(int speedNumber, int numberId);

	/**
	 * @brief Converts to markup string.
	 *
	 * @param[in]   source  the string (in UTF-8) to be converted
	 *
	 * @return the converted markup string.
	 */
	static std::string convert2MarkupString(const char *source);
};

#endif /* _PH_COMMON_H_ */

