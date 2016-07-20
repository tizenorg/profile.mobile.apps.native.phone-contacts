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

#include "WidgetDatabase.h"
#include "Utils/Logger.h"

#include <app.h>
#include <sqlite3.h>
#include <string>
#include <sys/stat.h>

#define STMT_BUF_LEN 256

#define DB_FILENAME "widget.db"
#define TABLE_ITEMS "items"
#define TABLE_WIDGETS "widgets"

#define COLUMN_ID           "id"
#define COLUMN_DATA_TYPE    "data_type"
#define COLUMN_DATA_ID      "data_id"
#define COLUMN_WIDGET_ID    "widget_id"

namespace
{
	const char createStmt[] = "CREATE TABLE " TABLE_WIDGETS "("
		COLUMN_WIDGET_ID    " INTEGER PRIMARY KEY AUTOINCREMENT);"

		"CREATE TABLE " TABLE_ITEMS "("
		COLUMN_ID           " INTEGER PRIMARY KEY AUTOINCREMENT, "
		COLUMN_DATA_TYPE    " INTEGER, "
		COLUMN_DATA_ID      " INTEGER, "
		COLUMN_WIDGET_ID    " INTEGER, "
		"FOREIGN KEY(" COLUMN_WIDGET_ID ") REFERENCES " TABLE_WIDGETS "(" COLUMN_WIDGET_ID "));";

	/************************** Item statements *******************************/
	const char selectStmt[] = "SELECT * FROM " TABLE_ITEMS
		" WHERE " COLUMN_WIDGET_ID " = %d;";

	const char insertStmt[] = "INSERT INTO " TABLE_ITEMS "("
		COLUMN_DATA_TYPE    ", "
		COLUMN_DATA_ID      ", "
		COLUMN_WIDGET_ID    ") VALUES(%d, %d, %d);";

	const char deleteStmt[] = "DELETE FROM " TABLE_ITEMS
		" WHERE " COLUMN_ID " = %d;";

	/************************** Widget statements *****************************/
	const char insertWidgetStmt[] = "INSERT INTO " TABLE_WIDGETS
		" DEFAULT VALUES;";

	const char deleteWidgetStmt[] = "DELETE FROM " TABLE_ITEMS
		" WHERE " COLUMN_WIDGET_ID " = %d;"

		"DELETE FROM " TABLE_WIDGETS
		" WHERE " COLUMN_WIDGET_ID " = %d;";
}

WidgetDatabase::WidgetDatabase()
	: m_Handle(nullptr), m_WidgetId(0)
{
}

WidgetDatabase::~WidgetDatabase()
{
	sqlite3_close(m_Handle);
}

bool WidgetDatabase::initialize(unsigned widgetId)
{
	char *path = app_get_data_path();
	RETVM_IF(!path, false, "app_get_data_path() failed.");

	std::string filename(path);
	filename.append(DB_FILENAME);
	free(path);

	struct stat buf;
	bool initialized = (stat(filename.c_str(), &buf) == 0);

	int err = sqlite3_open(filename.c_str(), &m_Handle);
	RETVM_IF(err != SQLITE_OK, false, "sqlite3_open() failed. %s", sqlite3_errstr(err));
	execute("PRAGMA foreign_keys = ON;");

	if (!initialized) {
		execute(createStmt);
	}

	m_WidgetId = widgetId ? widgetId : insertWidget();
	return true;
}

unsigned WidgetDatabase::getWidgetId() const
{
	return m_WidgetId;
}

void WidgetDatabase::fetch(const std::function<void(WidgetItem)> &callback)
{
	char stmt[STMT_BUF_LEN];
	snprintf(stmt, sizeof(stmt), selectStmt, m_WidgetId);

	sqlite3_stmt *prepStmt = nullptr;
	sqlite3_prepare_v2(m_Handle, stmt, -1, &prepStmt, nullptr);

	while (sqlite3_step(prepStmt) == SQLITE_ROW) {
		callback(WidgetItem(
			sqlite3_column_int(prepStmt, 0),
			(Common::ActionType) sqlite3_column_int(prepStmt, 1),
			sqlite3_column_int(prepStmt, 2)
		));
	}

	sqlite3_finalize(prepStmt);
}

int WidgetDatabase::insert(const WidgetItem &item)
{
	char stmt[STMT_BUF_LEN];
	snprintf(stmt, sizeof(stmt), insertStmt,
			item.getActionType(), item.getDataId(), m_WidgetId);

	if (execute(stmt)) {
		return sqlite3_last_insert_rowid(m_Handle);
	}

	return 0;
}

bool WidgetDatabase::remove(const WidgetItem &item)
{
	char stmt[STMT_BUF_LEN];
	snprintf(stmt, sizeof(stmt), deleteStmt, item.getId());
	return execute(stmt);
}

bool WidgetDatabase::removeWidget()
{
	char stmt[STMT_BUF_LEN];
	snprintf(stmt, sizeof(stmt), deleteWidgetStmt, m_WidgetId, m_WidgetId);
	if (execute(stmt)) {
		m_WidgetId = 0;
		return true;
	}

	return false;
}

unsigned WidgetDatabase::insertWidget()
{
	if (execute(insertWidgetStmt)) {
		return sqlite3_last_insert_rowid(m_Handle);
	}

	return 0;
}

bool WidgetDatabase::execute(const char *stmt)
{
	char *err = nullptr;
	sqlite3_exec(m_Handle, stmt, nullptr, nullptr, &err);
	if (err) {
		ERR("sqlite3_exec() failed. %s", err);
		sqlite3_free(err);
		return false;
	}

	return true;
}
