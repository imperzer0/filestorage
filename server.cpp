// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#include "server.h"
#include "mongoose.c"
#include "mariadb/conncpp.hpp"
#include "constants.hpp"
#include "strscan.c"
#include "resources.hpp"


const char* address = "http://0.0.0.0:80";
const char* log_level = "2";
const char* directory = ".";
const char* pattern = "#.html";
int hexdump = 0;

// Handle interrupts, like Ctrl-C
static int s_signo = 0;

static void signal_handler(int signo) { s_signo = signo; }


void mariadb_create_db(const char* db_user_password);

void mariadb_drop_db(const char* db_user_password);

std::unique_ptr<sql::Connection> mariadb_connect_to_db(const char* db_user_password);

void mariadb_create_table(sql::Connection* conn);

int mariadb_user_insert(sql::Connection* conn, const char* login, const char* password);

void mariadb_user_update(sql::Connection* conn, const char* login, const char* password);

const char* mariadb_user_get_password(sql::Connection* conn, const char* login);

typedef struct
{
	size_t files;
	size_t folders;
	size_t total;
	char* filename;
} statistics;


typedef const char* (* prepare_function)(const char* dir);

statistics directory_count(const char* dir);

const char* directory_list_html(const char* dir, const char* dir_abs, const char* login, const char* password);


int starts_with(const char* str, const char* prefix);

const char* path_dirname(const char* path);

const char* path_basename(const char* path);

void client_handler(struct mg_connection* connection, int ev, void* ev_data, void* fn_data)
{
	if (ev == MG_EV_HTTP_MSG)
	{
		const char* database_user_password = *static_cast<const char**>(fn_data);
		auto* msg = static_cast<mg_http_message*>(ev_data);
		if (mg_http_match_uri(msg, "/") || mg_http_match_uri(msg, "/index.html") || mg_http_match_uri(msg, "/index"))
			mg_http_reply(connection, 200, "Content-Type: text/html\r\n", login_page_html);
		else if (mg_http_match_uri(msg, "/login"))
		{
			char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
			mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
			mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
			
			auto conn = mariadb_connect_to_db(database_user_password);
			auto db_password = mariadb_user_get_password(conn.get(), login);
			if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
				mg_http_reply(connection, 308, "Location: /explorer/\r\n", "");
			else
				mg_http_reply(
						connection, 200, "Content-Type: text/html\r\n",
						invalid_credentials_page_html,
						"Invalid credentials.<br/>Please register or contact support to recover your account.",
						login, password, login, password
				);
			
		}
		else if (mg_http_match_uri(msg, "/register"))
		{
			char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
			mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
			mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
			
			auto conn = mariadb_connect_to_db(database_user_password);
			if (!mariadb_user_insert(conn.get(), login, password))
				mg_http_reply(
						connection, 200, "Content-Type: text/html\r\n",
						invalid_credentials_page_html, "User already exists.", "", "", login, password
				);
			else
			{
				system((std::string("mkdir -p './") + login + "/'").c_str());
				mg_http_reply(connection, 308, "Location: /explorer/\r\n", login_page_html);
			}
		}
		else if (starts_with(msg->uri.ptr, "/explorer/"))
		{
			char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
			mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
			mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
			
			auto conn = mariadb_connect_to_db(database_user_password);
			auto db_password = mariadb_user_get_password(conn.get(), login);
			if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
			{
				char* dir_rel;
				auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
				
				char* uri = new char[msg->uri.len + 1];
				strncpy(uri, msg->uri.ptr, msg->uri.len);
				uri[msg->uri.len] = 0;
				
				strscanf(uri, "/explorer/%s", &dir_rel);
				sprintf(dir, "%s%s", login, dir_rel);
				
				delete[] uri;
				
				std::string path("./");
				path += dir;
				
				struct stat st{ };
				if (::stat(path.c_str(), &st) < 0)
				{
					mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
					return;
				}
				
				if (S_ISREG(st.st_mode))
				{
					struct mg_http_serve_opts opts{ };
					mg_http_serve_file(connection, msg, path.c_str(), &opts);
					return;
				}
				
				auto dir_rel_dirname = path_dirname(dir_rel);
				
				mg_http_reply(
						connection, 200, "Content-Type: text/html\r\n", explorer_page_html,
						dir_rel_dirname, dir_rel_dirname, login, password,
						dir_rel_dirname, dir_rel_dirname, path_basename(dir_rel),
						dir_rel, dir_rel, login, password, dir_rel,
						directory_list_html(dir_rel, dir, login, password)
				);
				
				delete[] dir_rel;
				delete[] dir;
			}
			else mg_http_reply(connection, 308, "Location: /\r\n", login_page_html);
		}
		else mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
		
	}
}

static struct mg_mgr manager{ };
static struct mg_connection* connection;

void server_initialize(const char* database_user_password)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	mg_log_set(log_level);
	mg_mgr_init(&manager);
	
	mariadb_create_db(database_user_password);
	auto conn = mariadb_connect_to_db(database_user_password);
	mariadb_create_table(conn.get());
}

void server_run(const char* database_user_password)
{
	if (!(connection = mg_http_listen(&manager, address, client_handler, &database_user_password)))
	{
		MG_ERROR(("Cannot start listening on %s. Use 'http://ADDR:PORT' or just ':PORT'", address));
		exit(EXIT_FAILURE);
	}
	
	if (hexdump) connection->is_hexdumping = 1;
	
	MG_INFO(("Mongoose v" MG_VERSION));
	MG_INFO(("Server listening on : [%s]", address));
	MG_INFO(("Web root directory  : [%s]", directory));
	
	while (s_signo == 0) mg_mgr_poll(&manager, 1000);
	
	mg_mgr_free(&manager);
	MG_INFO(("Exiting due to signal [%d]...", s_signo));
}

void server_destroy_database(const char* database_user_password) { mariadb_drop_db(database_user_password); }


void mariadb_create_db(const char* db_user_password)
{
	MG_DEBUG(("Creating database " DB_NAME " ..."));
	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();
		
		sql::SQLString url("jdbc:mariadb://localhost:3306/");
		sql::Properties properties(
				{ { "user", DB_USER_NAME },
				  { "password", db_user_password } }
		);
		
		std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
		
		std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
		
		stmnt->executeQuery("CREATE DATABASE " DB_NAME);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Database creation failed: %s", e.what())); }
}

void mariadb_drop_db(const char* db_user_password)
{
	MG_DEBUG(("Dropping database " DB_NAME " ..."));
	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();
		
		sql::SQLString url("jdbc:mariadb://localhost:3306/");
		sql::Properties properties(
				{ { "user", DB_USER_NAME },
				  { "password", db_user_password } }
		);
		
		std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
		
		std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
		
		stmnt->executeQuery("DROP DATABASE " DB_NAME);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Database dropping failed: %s", e.what())); }
}

std::unique_ptr<sql::Connection> mariadb_connect_to_db(const char* db_user_password)
{
	MG_DEBUG(("Connecting to database " DB_NAME " ..."));
	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();
		
		sql::SQLString url("jdbc:mariadb://localhost:3306/" DB_NAME);
		sql::Properties properties(
				{ { "user", DB_USER_NAME },
				  { "password", db_user_password } }
		);
		
		return std::unique_ptr<sql::Connection>(driver->connect(url, properties));
	}
	catch (sql::SQLException& e) { MG_ERROR(("Table creation failed: %s", e.what())); }
	return nullptr;
}

void mariadb_create_table(sql::Connection* conn)
{
	if (!conn)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Creating table " TABLE_NAME " in database " DB_NAME " ..."));
	
	try
	{
		std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
		
		stmnt->executeQuery(
				"CREATE TABLE " TABLE_NAME " "
				"( login varchar(" MACRO_STR(MAX_LOGIN) ") NOT NULL PRIMARY KEY,"
				"password varchar(" MACRO_STR(MAX_PASSWORD) ") NOT NULL );"
		);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Table creation failed: %s", e.what())); }
}

int mariadb_user_insert(sql::Connection* conn, const char* login, const char* password)
{
	if (!conn)
	{
		MG_ERROR(("Connection is nullptr."));
		return 0;
	}
	
	MG_DEBUG(("Updating user '%s' in table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(
				conn->prepareStatement("INSERT INTO " TABLE_NAME " ( login, password ) values( ?,? );")
		);
		
		stmnt->setString(1, login);
		stmnt->setString(2, password);
		
		stmnt->executeQuery();
		return 1;
	}
	catch (sql::SQLException& e) { MG_ERROR(("User update failed: %s", e.what())); }
	return 0;
}

void mariadb_user_update(sql::Connection* conn, const char* login, const char* password)
{
	if (!conn)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Saving user '%s' into table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("UPDATE " TABLE_NAME " SET password=? WHERE login=?;"));
		
		stmnt->setString(1, password);
		stmnt->setString(2, login);
		
		stmnt->executeQuery();
	}
	catch (sql::SQLException& e) { MG_ERROR(("User creation failed: %s", e.what())); }
}

const char* mariadb_user_get_password(sql::Connection* conn, const char* login)
{
	if (!conn)
	{
		MG_ERROR(("Connection is nullptr."));
		return nullptr;
	}
	
	MG_DEBUG(("Querying %s's password from table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT password FROM " TABLE_NAME " WHERE login=?;"));
		
		stmnt->setString(1, login);
		
		auto res = stmnt->executeQuery();
		if (res->next()) return res->getString("password").c_str();
	}
	catch (sql::SQLException& e) { MG_ERROR(("Failed to get user password: %s.", e.what())); }
	return nullptr;
}


static consteval size_t static_strlen(const char* str)
{
	size_t len = 0;
	for (; *str; ++len, ++str);
	return len;
}

statistics directory_count(const char* dir)
{
	statistics result{ };
	DIR* dirp = opendir((std::string("./") + dir).c_str());
	if (!dirp) return result;
	
	struct dirent* entry;
	while ((entry = readdir(dirp)))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		if (entry->d_type == DT_DIR) ++result.folders, ++result.total;
		if (entry->d_type == DT_REG) ++result.files, ++result.total;
	}
	
	closedir(dirp);
	return result;
}

const char* directory_prepare_html(char* dir, const char* dir_abs, const char* login, const char* password)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	sprintf(html, explorer_dir_html, dir, dir, login, password, dir, dir, basename(dir), st.files, st.folders);
	return html;
}

const char* get_filename_ext(const char* filename)
{
	const char* dot = nullptr;
	for (const char* dot_tmp = filename; *dot_tmp; ++dot_tmp)
		if (*dot_tmp == '.')
			dot = dot_tmp;
	
	if (!dot || dot == filename) return "";
	return dot + 1;
}

const char* file_prepare_html(char* file, const char* file_abs, const char* login, const char* password)
{
	struct stat st{ };
	stat((std::string("./") + file_abs).c_str(), &st);
	char* html = new char[static_strlen(explorer_file_html) + 2088];
	auto ext = get_filename_ext(file);
	sprintf(html, explorer_file_html, file, file, login, password, file, file, ext, ext, basename(file), st.st_size);
	return html;
}

const char* directory_list_html(const char* dir, const char* dir_abs, const char* login, const char* password)
{
	if (directory_count(dir_abs).total <= 0)
		return explorer_dir_empty_html;
	
	std::string path = std::string("./") + dir_abs;
	DIR* dirp = opendir(path.c_str());
	if (dirp == nullptr)
		return explorer_dir_empty_html;
	
	struct dirent* entry;
	
	std::string result;
	
	while ((entry = readdir(dirp)))
	{
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
		if (entry->d_type == DT_DIR)
		{
			char* fullpath = new char[strlen(dir) + 1 + strlen(entry->d_name)]{ };
			sprintf(fullpath, "%s/%s", dir, entry->d_name);
			
			char* fullpath_abs = new char[strlen(dir_abs) + 1 + strlen(entry->d_name)]{ };
			sprintf(fullpath_abs, "%s/%s", dir_abs, entry->d_name);
			
			result += directory_prepare_html(fullpath, fullpath_abs, login, password);
		}
		if (entry->d_type == DT_REG)
		{
			char* fullpath = new char[strlen(dir) + 1 + strlen(entry->d_name)]{ };
			sprintf(fullpath, "%s/%s", dir, entry->d_name);
			
			char* fullpath_abs = new char[strlen(dir_abs) + 1 + strlen(entry->d_name)]{ };
			sprintf(fullpath_abs, "%s/%s", dir_abs, entry->d_name);
			
			result += file_prepare_html(fullpath, fullpath_abs, login, password);
		}
	}
	closedir(dirp);
	
	char* html = new char[result.size() + static_strlen(explorer_dir_content_html)]{ };
	
	sprintf(html, explorer_dir_content_html, result.c_str());
	
	return html;
}


int starts_with(const char* str, const char* prefix)
{
	for (; *prefix; ++prefix, ++str)
		if (*prefix != *str)
			return false;
	return true;
}

const char* path_dirname(const char* path)
{
	char* accesible_path = ::strdup(path);
	char* slash = nullptr;
	for (char* dot_tmp = accesible_path; *dot_tmp; ++dot_tmp)
		if (*dot_tmp == '/')
			slash = dot_tmp;
	
	if (!slash) return "";
	*slash = 0;
	return accesible_path;
}

const char* path_basename(const char* path)
{
	const char* slash = nullptr;
	for (const char* dot_tmp = path; *dot_tmp; ++dot_tmp)
		if (*dot_tmp == '/')
			slash = dot_tmp;
	
	if (!slash) return "";
	return slash + 1;
}