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


const char* address = DEFAULT_SERVER_ADDRESS;
const char* log_level = "2";
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


statistics directory_count(const char* dir);


typedef char* (* prepare_function)(const char* path, const char* path_abs, const char* login, const char* password);


char* deleter_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password);

char* explorer_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password);

char* deleter_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password);

char* explorer_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password);


std::string directory_list_html(
		const char* dir, const char* dir_abs, const char* login, const char* password,
		prepare_function prepare_dir, prepare_function prepare_file
);



int starts_with(const char* str, const char* prefix);

char* path_dirname(const char* path);

const char* path_basename(const char* path);

char* getcwd();


inline void http_redirect_to(struct mg_connection* connection, const char* url_fmt, ...)
{
	char mem[256], * buf = mem;
	va_list ap;
	size_t len;
	va_start(ap, url_fmt);
	len = mg_vasprintf(&buf, sizeof(mem), url_fmt, ap);
	va_end(ap);
	
	mg_printf(connection, "HTTP/1.1 307 %s\r\nLocation: %s\r\nContent-Length: 0\r\n\r\n", mg_http_status_code_str(307), buf);
	
	if (buf != mem) free(buf);
}


inline char* http_get_ret_path(struct mg_http_message* msg)
{
	char* ret_path = new char[msg->query.len]{ };
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len - 1);
	
	if (!*ret_path)
	{
		delete[] ret_path;
		return new char[]{ "/explorer/" };
	}
	
	int last = mg_url_decode(ret_path, msg->query.len - 1, ret_path, msg->query.len - 1, 1);
	ret_path[last] = 0;
	
	return ret_path;
}


inline char* http_get_ret_path_raw(struct mg_http_message* msg)
{
	char* ret_path = new char[msg->query.len]{ };
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len - 1);
	
	if (!*ret_path)
	{
		delete[] ret_path;
		return new char[]{ "?return_to=%2fexplorer%2f" };
	}
	
	size_t ret_path_len = strlen(ret_path);
	char* ret_path_url = new char[ret_path_len * 3]{ };
	mg_url_encode(ret_path, ret_path_len, ret_path_url, ret_path_len * 3 - 1);
	
	char* ret_path_raw = new char[msg->query.len + ret_path_len * 2]{ };
	sprintf(ret_path_raw, "?return_to=%s", ret_path_url);
	
	delete[] ret_path;
	delete[] ret_path_url;
	
	return ret_path_raw;
}


inline void handle_http_message(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	if (mg_http_match_uri(msg, "/") || mg_http_match_uri(msg, "/index.html") || mg_http_match_uri(msg, "/index"))
	{
		char* ret_path = http_get_ret_path_raw(msg);
		
		if (*ret_path)
			mg_http_reply(connection, 200, "Content-Type: text/html\r\n", login_page_html, ret_path, ret_path);
		else
			mg_http_reply(connection, 200, "Content-Type: text/html\r\n", login_page_html, "", "");
		
		delete[] ret_path;
	}
	else if (mg_http_match_uri(msg, "/login"))
	{
		char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
		mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
		mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
		
		
		auto conn = mariadb_connect_to_db(database_user_password);
		auto db_password = mariadb_user_get_password(conn.get(), login);
		if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
		{
			char* ret_path = http_get_ret_path(msg);
			
			http_redirect_to(connection, ret_path);
			
			delete[] ret_path;
		}
		else
		{
			char* ret_path = http_get_ret_path_raw(msg);
			
			mg_http_reply(
					connection, 200, "Content-Type: text/html\r\n",
					invalid_credentials_page_html,
					"Invalid credentials.<br/>Please register or contact support to recover your account.",
					ret_path, login, password, ret_path, login, password
			);
			
			delete[] ret_path;
		}
	}
	else if (mg_http_match_uri(msg, "/register"))
	{
		char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
		mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
		mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
		
		auto conn = mariadb_connect_to_db(database_user_password);
		if (!mariadb_user_insert(conn.get(), login, password))
		{
			char* ret_path = http_get_ret_path_raw(msg);
			
			mg_http_reply(
					connection, 200, "Content-Type: text/html\r\n",
					invalid_credentials_page_html,
					"User already exists.<br/>Try out another username.",
					ret_path, "", "", ret_path, login, password
			);
			
			delete[] ret_path;
		}
		else
		{
			char* ret_path = http_get_ret_path(msg);
			
			system((std::string("mkdir -p './") + login + "/'").c_str());
			http_redirect_to(connection, ret_path);
			
			delete[] ret_path;
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
				
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			if (S_ISREG(st.st_mode))
			{
				struct mg_http_serve_opts opts{ };
				mg_http_serve_file(connection, msg, path.c_str(), &opts);
				
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			auto dir_rel_dirname = path_dirname(dir_rel);
			
			mg_http_reply(
					connection, 200, "Content-Type: text/html\r\n", explorer_page_html,
					dir_rel_dirname, dir_rel_dirname, login, password,
					dir_rel_dirname, dir_rel_dirname, path_basename(dir_rel),
					dir_rel, dir_rel, login, password, dir_rel,
					dir_rel, dir_rel, login, password, dir_rel,
					directory_list_html(
							dir_rel, dir, login, password,
							explorer_directory_prepare_html, explorer_file_prepare_html
					).c_str()
			);
			
			delete[] dir_rel_dirname;
			delete[] dir_rel;
			delete[] dir;
		}
		else
		{
			char* curr_url = new char[msg->uri.len * 3]{ };
			mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3 - 1);
			
			http_redirect_to(connection, "/?return_to=%s", curr_url);
			
			delete[] curr_url;
		}
	}
	else if (starts_with(msg->uri.ptr, "/deleter/"))
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
			
			strscanf(uri, "/deleter/%s", &dir_rel);
			sprintf(dir, "%s%s", login, dir_rel);
			
			delete[] uri;
			
			std::string path("./");
			path += dir;
			
			struct stat st{ };
			if (::stat(path.c_str(), &st) < 0)
			{
				mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
				
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			if (S_ISREG(st.st_mode))
			{
				struct mg_http_serve_opts opts{ };
				mg_http_serve_file(connection, msg, path.c_str(), &opts);
				
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			auto dir_rel_dirname = path_dirname(dir_rel);
			
			mg_http_reply(
					connection, 200, "Content-Type: text/html\r\n", deleter_page_html,
					dir_rel, dir_rel, dir_rel, login, password, dir_rel,
					directory_list_html(
							dir_rel, dir, login, password,
							deleter_directory_prepare_html, deleter_file_prepare_html
					).c_str()
			);
			
			delete[] dir_rel_dirname;
			delete[] dir_rel;
			delete[] dir;
		}
		else
		{
			char* curr_url = new char[msg->uri.len * 3]{ };
			mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3 - 1);
			
			http_redirect_to(connection, "/?return_to=%s", curr_url);
			
			delete[] curr_url;
		}
	}
	else if (starts_with(msg->uri.ptr, "/delete/"))
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
			
			strscanf(uri, "/delete/%s", &dir_rel);
			sprintf(dir, "%s%s", login, dir_rel);
			
			delete[] uri;
			
			std::string path("./");
			path += dir;
			
			struct stat st{ };
			if (::stat(path.c_str(), &st) < 0)
			{
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			system(("rm -rf '" + path + "'").c_str());
			
			delete[] dir_rel;
			delete[] dir;
		}
	}
	else if (starts_with(msg->uri.ptr, "/uploader/"))
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
			
			strscanf(uri, "/uploader/%s", &dir_rel);
			sprintf(dir, "%s%s", login, dir_rel);
			
			delete[] uri;
			
			std::string path("./");
			path += dir;
			
			struct stat st{ };
			if (::stat(path.c_str(), &st) < 0 && S_ISDIR(st.st_mode))
			{
				mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
				
				delete[] dir;
				delete[] dir_rel;
				return;
			}
			
			mg_http_reply(
					connection, 200, "Content-Type: text/html\r\n", uploader_page_html,
					dir_rel, dir_rel, login, password, dir_rel, login, password
			);
			
			delete[] dir_rel;
			delete[] dir;
		}
		else
		{
			char* curr_url = new char[msg->uri.len * 3]{ };
			mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3 - 1);
			
			http_redirect_to(connection, "/?return_to=%s", curr_url);
			
			delete[] curr_url;
		}
	}
	else if (starts_with(msg->uri.ptr, "/upload/"))
	{
		char* login = new char[1]{ }, * password = new char[1]{ }, * filename = new char[1]{ };
		long long pos = -1;
		
		struct mg_http_part filepart{ };
		size_t ofs = 0;
		while ((ofs = mg_http_next_multipart(msg->body, ofs, &filepart)) > 0)
		{
			MG_INFO((
					        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
							        filepart.name.len, filepart.name.ptr, filepart.filename.len,
							        filepart.filename.ptr, filepart.body.len
			        ));
			if (!strncmp(filepart.name.ptr, "login", filepart.name.len))
			{
				delete[] login;
				login = strndup(filepart.body.ptr, filepart.body.len);
			}
			else if (!strncmp(filepart.name.ptr, "password", filepart.name.len))
			{
				delete[] password;
				password = strndup(filepart.body.ptr, filepart.body.len);
			}
			else if (!strncmp(filepart.name.ptr, "start", filepart.name.len))
			{
				auto tmp = strndup(filepart.body.ptr, filepart.body.len);
				MG_INFO(("[start] = %s", tmp));
				pos = strtoll(tmp, nullptr, 10);
				delete[] tmp;
			}
			else if (!strncmp(filepart.name.ptr, "name", filepart.name.len))
			{
				delete[] filename;
				filename = strndup(filepart.body.ptr, filepart.body.len);
			}
			else if (!strncmp(filepart.name.ptr, "file", filepart.name.len) && *filename && pos >= 0)
			{
				auto conn = mariadb_connect_to_db(database_user_password);
				auto db_password = mariadb_user_get_password(conn.get(), login);
				if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
				{
					char* dir_rel;
					auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
					
					char* uri = new char[msg->uri.len + 1];
					strncpy(uri, msg->uri.ptr, msg->uri.len);
					uri[msg->uri.len] = 0;
					
					strscanf(uri, "/upload/%s", &dir_rel);
					sprintf(dir, "%s%s", login, dir_rel);
					
					delete[] uri;
					
					std::string path("./");
					path += dir;
					
					struct stat st{ };
					if (::stat(path.c_str(), &st) < 0)
					{
						delete[] dir;
						delete[] dir_rel;
						delete[] login;
						delete[] password;
						delete[] filename;
						return;
					}
					
					path += "/";
					path += filename;
					
					FILE* file = fopen(path.c_str(), "ab");
					fseek(file, pos, SEEK_SET);
					fwrite(filepart.body.ptr, sizeof(char), filepart.body.len, file);
					fclose(file);
					
					mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
					
					delete[] dir;
					delete[] dir_rel;
				}
				else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
			}
			else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
		}
		delete[] login;
		delete[] password;
		delete[] filename;
	}
	else mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
}


void client_handler(struct mg_connection* connection, int ev, void* ev_data, void* fn_data)
{
	if (ev == MG_EV_HTTP_MSG)
	{
		const char* database_user_password = *static_cast<const char**>(fn_data);
		auto* msg = static_cast<mg_http_message*>(ev_data);
		handle_http_message(connection, msg, database_user_password);
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
	
	auto cwd = getcwd();
	
	MG_INFO(("Mongoose v" MG_VERSION));
	MG_INFO(("Server listening on : [%s]", address));
	MG_INFO(("Web root directory  : [file://%s/]", cwd));
	
	delete[] cwd;
	
	while (s_signo == 0) mg_mgr_poll(&manager, 1000);
	
	mg_mgr_free(&manager);
	MG_INFO(("Exiting due to signal [%d]...", s_signo));
}

void server_destroy_database(const char* database_user_password) { mariadb_drop_db(database_user_password); }


void mariadb_create_db(const char* db_user_password)
{
	MG_INFO(("Creating database " DB_NAME " ..."));
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
	MG_INFO(("Dropping database " DB_NAME " ..."));
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
	MG_INFO(("Connecting to database " DB_NAME " ..."));
	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();
		
		sql::SQLString url("jdbc:mariadb://localhost:3306/" DB_NAME);
		sql::Properties properties(
				{ { "user", DB_USER_NAME },
				  { "password", db_user_password } }
		);
		
		auto conn = driver->connect(url, properties);
		return std::unique_ptr<sql::Connection>(conn);
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
	
	MG_INFO(("Creating table " TABLE_NAME " in database " DB_NAME " ..."));
	
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
	
	MG_INFO(("Updating user '%s' in table " TABLE_NAME " of database " DB_NAME " ...", login));
	
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
	
	MG_INFO(("Saving user '%s' into table " TABLE_NAME " of database " DB_NAME " ...", login));
	
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
	
	MG_INFO(("Querying %s's password from table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("SELECT password FROM " TABLE_NAME " WHERE login=?;"));
		
		stmnt->setString(1, login);
		
		auto res = std::unique_ptr<sql::ResultSet>(stmnt->executeQuery());
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

const char* get_filename_ext(const char* filename)
{
	const char* dot = nullptr;
	for (const char* dot_tmp = filename; *dot_tmp; ++dot_tmp)
		if (*dot_tmp == '.')
			dot = dot_tmp;
	
	if (!dot || dot == filename) return "";
	return dot + 1;
}

char* explorer_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	sprintf(html, explorer_dir_html, dir, dir, login, password, dir, dir, path_basename(dir), st.files, st.folders);
	return html;
}

char* explorer_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password)
{
	struct stat st{ };
	stat((std::string("./") + file_abs).c_str(), &st);
	char* html = new char[static_strlen(explorer_file_html) + 2088];
	auto ext = get_filename_ext(file);
	sprintf(html, explorer_file_html, file, file, login, password, file, file, ext, ext, path_basename(file), st.st_size);
	return html;
}

char* deleter_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	char* dir_dirname = path_dirname(dir);
	sprintf(html, deleter_dir_html, dir, dir, login, password, dir_dirname, login, password, path_basename(dir), st.files, st.folders);
	delete[] dir_dirname;
	return html;
}


char* deleter_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password)
{
	struct stat st{ };
	stat((std::string("./") + file_abs).c_str(), &st);
	char* html = new char[static_strlen(explorer_file_html) + 2088];
	auto ext = get_filename_ext(file);
	char* file_dirname = path_dirname(file);
	sprintf(
			html, deleter_file_html, file, file, login, password, file_dirname, login, password, ext, ext, path_basename(file),
			st.st_size
	);
	delete[] file_dirname;
	return html;
}

std::string directory_list_html(
		const char* dir, const char* dir_abs, const char* login, const char* password,
		prepare_function prepare_dir, prepare_function prepare_file
)
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
			auto html = prepare_dir(
					(std::string(dir) + "/" + entry->d_name).c_str(),
					(std::string(dir_abs) + "/" + entry->d_name).c_str(),
					login, password
			);
			
			result += html;
			
			delete[] html;
		}
		if (entry->d_type == DT_REG)
		{
			auto html = prepare_file(
					(std::string(dir) + "/" + entry->d_name).c_str(),
					(std::string(dir_abs) + "/" + entry->d_name).c_str(),
					login, password
			);
			
			result += html;
			
			delete[] html;
		}
	}
	closedir(dirp);
	
	std::string html(result.size() + static_strlen(explorer_dir_content_html), 0);
	
	sprintf(html.data(), explorer_dir_content_html, result.c_str());
	
	return html.c_str();
}


int starts_with(const char* str, const char* prefix)
{
	for (; *prefix; ++prefix, ++str)
		if (*prefix != *str)
			return false;
	return true;
}

char* path_dirname(const char* path)
{
	char* accesible_path = ::strdup(path);
	char* slash = nullptr;
	for (char* tmp = accesible_path; *tmp; ++tmp)
		if (*tmp == '/')
			slash = tmp;
	
	if (!slash) return new char[1]{ };
	*slash = 0;
	return accesible_path;
}

const char* path_basename(const char* path)
{
	const char* slash = nullptr;
	for (const char* tmp = path; *tmp; ++tmp)
		if (*tmp == '/')
			slash = tmp;
	
	if (!slash) return "";
	return slash + 1;
}

char* getcwd()
{
	char* cwd = new char[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	cwd[PATH_MAX - 1] = 0;
	return cwd;
}