// This file is part of RSS Guard.

//
// Copyright (C) 2011-2017 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QtGlobal>

#define SERVICE_CODE_STD_RSS    "std-rss"
#define SERVICE_CODE_TT_RSS     "tt-rss"
#define SERVICE_CODE_OWNCLOUD   "owncloud"
#define SERVICE_CODE_INOREADER  "inoreader"

#define ARGUMENTS_LIST_SEPARATOR  "\n"

#define ADBLOCK_ADBLOCKED_PAGE                "adblockedpage"
#define ADBLOCK_HOWTO_FILTERS                 "http://adblockplus.org/en/filters"
#define ADBLOCK_UPDATE_DAYS_INTERVAL          5
#define ADBLOCK_ICON_ACTIVE                   "adblock"
#define ADBLOCK_ICON_DISABLED                 "adblock-disabled"
#define IS_IN_ARRAY(offset, array)            ((offset >= 0) && (offset < array.count()))
#define ADBLOCK_CUSTOMLIST_NAME               "customlist.txt"
#define ADBLOCK_LISTS_SUBDIRECTORY            "adblock"
#define ADBLOCK_EASYLIST_URL                  "https://easylist-downloads.adblockplus.org/easylist.txt"
#define DEFAULT_SQL_MESSAGES_FILTER           "0 > 1"
#define MAX_MULTICOLUMN_SORT_STATES           3
#define ENCLOSURES_OUTER_SEPARATOR            '#'
#define ECNLOSURES_INNER_SEPARATOR            '&'
#define URI_SCHEME_FEED_SHORT                 "feed:"
#define URI_SCHEME_FEED                       "feed://"
#define URI_SCHEME_HTTP                       "http://"
#define RELEASES_LIST                         "https://api.github.com/repos/martinrotter/rssguard/releases"
#define DEFAULT_LOCALE                        "en"
#define DEFAULT_FEED_ENCODING                 "UTF-8"
#define DEFAULT_FEED_TYPE                     "RSS"
#define URL_REGEXP \
  "^(http|https|feed|ftp):\\/\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&amp;:/~\\+#]*[\\w\\-\\@?^=%&amp;/~\\+#])?$"
#define USER_AGENT_HTTP_HEADER                "User-Agent"
#define TEXT_TITLE_LIMIT                      30
#define RESELECT_MESSAGE_THRESSHOLD           500
#define ICON_SIZE_SETTINGS                    16
#define NO_PARENT_CATEGORY                    -1
#define NO_PARENT_CATEGORY_STR                ""
#define ID_RECYCLE_BIN                        -2
#define TRAY_ICON_BUBBLE_TIMEOUT              20000
#define CLOSE_LOCK_TIMEOUT                    500
#define DOWNLOAD_TIMEOUT                      15000
#define MESSAGES_VIEW_DEFAULT_COL             170
#define MESSAGES_VIEW_MINIMUM_COL             36
#define FEEDS_VIEW_COLUMN_COUNT               2
#define FEED_DOWNLOADER_MAX_THREADS           6
#define DEFAULT_DAYS_TO_DELETE_MSG            14
#define ELLIPSIS_LENGTH                       3
#define MIN_CATEGORY_NAME_LENGTH              1
#define DEFAULT_AUTO_UPDATE_INTERVAL          15
#define AUTO_UPDATE_INTERVAL                  60000
#define STARTUP_UPDATE_DELAY                  30000
#define TIMEZONE_OFFSET_LIMIT                 6
#define CHANGE_EVENT_DELAY                    250
#define FLAG_ICON_SUBFOLDER                   "flags"
#define SEACRH_MESSAGES_ACTION_NAME           "search"
#define HIGHLIGHTER_ACTION_NAME               "highlighter"
#define SPACER_ACTION_NAME                    "spacer"
#define SEPARATOR_ACTION_NAME                 "separator"
#define FILTER_WIDTH                          150
#define FILTER_RIGHT_MARGIN                   5
#define FEEDS_VIEW_INDENTATION                10
#define ACCEPT_HEADER_FOR_FEED_DOWNLOADER     "application/atom+xml,application/xml;q=0.9,text/xml;q=0.8,*/*;q=0.7"
#define MIME_TYPE_ITEM_POINTER                "rssguard/itempointer"
#define DOWNLOADER_ICON_SIZE                  48
#define NOTIFICATION_ICON_SIZE                32
#define GOOGLE_SEARCH_URL                     "https://www.google.com/search?q=%1&ie=utf-8&oe=utf-8"
#define GOOGLE_SUGGEST_URL                    "http://suggestqueries.google.com/complete/search?output=toolbar&hl=en&q=%1"
#define ENCRYPTION_FILE_NAME                  "key.private"
#define RELOAD_MODEL_BORDER_NUM               10
#define EXTERNAL_TOOL_SEPARATOR               "###"
#define EXTERNAL_TOOL_PARAM_SEPARATOR         "|||"

#define MAX_ZOOM_FACTOR     5.0f
#define MIN_ZOOM_FACTOR     0.25f
#define DEFAULT_ZOOM_FACTOR 1.0f
#define ZOOM_FACTOR_STEP    0.1f

#define INTERNAL_URL_MESSAGE                  "http://rssguard.message"
#define INTERNAL_URL_BLANK                    "http://rssguard.blank"
#define INTERNAL_URL_MESSAGE_HOST             "rssguard.message"
#define INTERNAL_URL_BLANK_HOST               "rssguard.blank"

#define FEED_INITIAL_OPML_PATTERN             "feeds-%1.opml"

#define FEED_REGEX_MATCHER                    "<link[^>]+type=\\\"application/(atom|rss)\\+xml\\\"[^>]*>"
#define FEED_HREF_REGEX_MATCHER               "href\\=\\\"[^\\\"]+\\\""

#define PLACEHOLDER_UNREAD_COUNTS   "%unread"
#define PLACEHOLDER_ALL_COUNTS      "%all"

#define BACKUP_NAME_SETTINGS    "config"
#define BACKUP_SUFFIX_SETTINGS  ".ini.backup"
#define BACKUP_NAME_DATABASE    "database"
#define BACKUP_SUFFIX_DATABASE  ".db.backup"

#define APP_DB_MYSQL_DRIVER           "QMYSQL"
#define APP_DB_MYSQL_INIT             "db_init_mysql.sql"
#define APP_DB_MYSQL_TEST             "MySQLTest"
#define APP_DB_MYSQL_PORT             3306

#define APP_DB_SQLITE_DRIVER          "QSQLITE"
#define APP_DB_SQLITE_INIT            "db_init_sqlite.sql"
#define APP_DB_SQLITE_PATH            "database/local"
#define APP_DB_SQLITE_FILE            "database.db"

// Keep this in sync with schema versions declared in SQL initialization code.
#define APP_DB_SCHEMA_VERSION         "10"
#define APP_DB_UPDATE_FILE_PATTERN    "db_update_%1_%2_%3.sql"
#define APP_DB_COMMENT_SPLIT          "-- !\n"
#define APP_DB_NAME_PLACEHOLDER       "##"

#define APP_CFG_PATH        "config"
#define APP_CFG_FILE        "config.ini"

#define APP_QUIT_INSTANCE   "-q"
#define APP_IS_RUNNING      "app_is_running"
#define APP_SKIN_USER_FOLDER "skins"
#define APP_SKIN_DEFAULT    "vergilius"
#define APP_SKIN_METADATA_FILE "metadata.xml"
#define APP_STYLE_DEFAULT   "Fusion"
#define APP_THEME_DEFAULT   "Faenza"
#define APP_NO_THEME        ""
#define APP_THEME_SUFFIX    ".png"

#ifndef QSL

// Thin macro wrapper for literal strings.
// They are much more memory efficient and faster.
// Use it for all literals except for two cases:
//  a) Methods which take QLatin1String (use QLatin1String for literal argument too),
//  b) Construction of empty literals "", use QString() instead of QStringLiteral("").
#define QSL(x) QStringLiteral(x)
#endif

#ifndef QL1S

// Macro for latin strings. Latin strings are
// faster than QStrings created from literals.
#define QL1S(x) QLatin1String(x)
#endif

#ifndef QL1C

// Macro for latin chars.
#define QL1C(x) QLatin1Char(x)
#endif

// Indexes of columns as they are DEFINED IN THE TABLE for MESSAGES.
#define MSG_DB_ID_INDEX                 0
#define MSG_DB_READ_INDEX               1
#define MSG_DB_DELETED_INDEX            2
#define MSG_DB_IMPORTANT_INDEX          3
#define MSG_DB_FEED_TITLE_INDEX         4
#define MSG_DB_TITLE_INDEX              5
#define MSG_DB_URL_INDEX                6
#define MSG_DB_AUTHOR_INDEX             7
#define MSG_DB_DCREATED_INDEX           8
#define MSG_DB_CONTENTS_INDEX           9
#define MSG_DB_PDELETED_INDEX           10
#define MSG_DB_ENCLOSURES_INDEX         11
#define MSG_DB_ACCOUNT_ID_INDEX         12
#define MSG_DB_CUSTOM_ID_INDEX          13
#define MSG_DB_CUSTOM_HASH_INDEX        14
#define MSG_DB_FEED_CUSTOM_ID_INDEX     15

// Indexes of columns as they are DEFINED IN THE TABLE for CATEGORIES.
#define CAT_DB_ID_INDEX           0
#define CAT_DB_PARENT_ID_INDEX    1
#define CAT_DB_TITLE_INDEX        2
#define CAT_DB_DESCRIPTION_INDEX  3
#define CAT_DB_DCREATED_INDEX     4
#define CAT_DB_ICON_INDEX         5
#define CAT_DB_ACCOUNT_ID_INDEX   6
#define CAT_DB_CUSTOM_ID_INDEX    7

// Indexes of columns as they are DEFINED IN THE TABLE for FEEDS.
#define FDS_DB_ID_INDEX               0
#define FDS_DB_TITLE_INDEX            1
#define FDS_DB_DESCRIPTION_INDEX      2
#define FDS_DB_DCREATED_INDEX         3
#define FDS_DB_ICON_INDEX             4
#define FDS_DB_CATEGORY_INDEX         5
#define FDS_DB_ENCODING_INDEX         6
#define FDS_DB_URL_INDEX              7
#define FDS_DB_PROTECTED_INDEX        8
#define FDS_DB_USERNAME_INDEX         9
#define FDS_DB_PASSWORD_INDEX         10
#define FDS_DB_UPDATE_TYPE_INDEX      11
#define FDS_DB_UPDATE_INTERVAL_INDEX  12
#define FDS_DB_TYPE_INDEX             13
#define FDS_DB_ACCOUNT_ID_INDEX       14
#define FDS_DB_CUSTOM_ID_INDEX        15

// Indexes of columns for feed models.
#define FDS_MODEL_TITLE_INDEX           0
#define FDS_MODEL_COUNTS_INDEX          1

#if defined(Q_OS_LINUX)
#define OS_ID   "Linux"
#elif defined(Q_OS_OSX)
#define OS_ID   "Mac OS X"
#elif defined(Q_OS_WIN)
#define OS_ID   "Windows"
#else
#define OS_ID   ""
#endif

#if defined(Q_OS_LINUX)
#define APP_DESKTOP_SOURCE_ENTRY_FILE "rssguard.desktop.autostart"
#define APP_DESKTOP_ENTRY_FILE "rssguard.desktop"

#define APP_DESKTOP_ENTRY_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/autostart")
#define APP_LANG_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/translations")
#define APP_SKIN_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/skins")
#define APP_INFO_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/information")
#define APP_THEME_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/icons")
#define APP_SQL_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/sql")
#define APP_ICON_PATH QApplication::applicationDirPath() + QString("/../share/pixmaps/rssguard.png")
#define APP_ICON_PLAIN_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/icons/rssguard_plain.png")
#define APP_INITIAL_FEEDS_PATH QApplication::applicationDirPath() + QString("/../share/rssguard/initial_feeds")
#elif defined(Q_OS_OSX)
#define APP_LANG_PATH QApplication::applicationDirPath() + QString("/../Resources/translations")
#define APP_SKIN_PATH QApplication::applicationDirPath() + QString("/../Resources/skins")
#define APP_INFO_PATH QApplication::applicationDirPath() + QString("/../Resources/information")
#define APP_THEME_PATH QApplication::applicationDirPath() + QString("/../Resources/icons")
#define APP_SQL_PATH QApplication::applicationDirPath() + QString("/../Resources/sql")
#define APP_ICON_PATH QApplication::applicationDirPath() + QString("/../Resources/icons/rssguard.png")
#define APP_INFO_PATH QApplication::applicationDirPath() + QString("/../Resources/information")
#define APP_ICON_PLAIN_PATH QApplication::applicationDirPath() + QString("/../Resources/icons/rssguard_plain.png")
#define APP_INITIAL_FEEDS_PATH QApplication::applicationDirPath() + QString("/../Resources/initial_feeds")
#elif defined(Q_OS_WIN)
#define APP_LANG_PATH QApplication::applicationDirPath() + QString("/translations")
#define APP_SKIN_PATH QApplication::applicationDirPath() + QString("/skins")
#define APP_INFO_PATH QApplication::applicationDirPath()
#define APP_THEME_PATH QApplication::applicationDirPath() + QString("/icons")
#define APP_SQL_PATH QApplication::applicationDirPath() + QString("/sql")
#define APP_ICON_PATH QApplication::applicationDirPath() + QString("/rssguard.png")
#define APP_ICON_PLAIN_PATH QApplication::applicationDirPath() + QString("/rssguard_plain.png")
#define APP_INITIAL_FEEDS_PATH QApplication::applicationDirPath() + QString("/initial_feeds")
#endif

#endif // DEFINITIONS_H
