// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#include "server.h"
#include "mongoose.c"
#include "constants.hpp"
#include "strscan.c"
#include "resources.hpp"
#include "database.h"
#include "sha256.hpp"
#include "config_script.h"
#include "Gen_QR.h"


const char* address = DEFAULT_SERVER_ADDRESS;
const char* log_level = "2";
int hexdump = 0;

static struct mg_mgr manager{ };
static struct mg_connection* server_connection;

struct user_credentials
{
	std::string login;
	std::string password;
	time_t last_access = 0;
};

static std::map<uint64_t, user_credentials> session_cookies;

// Handle interrupts, like Ctrl-C
static int s_signo = 0;

static void signal_handler(int signo) { s_signo = signo; }


typedef struct
{
	size_t files;
	size_t folders;
	size_t total;
	char* filename;
} statistics;

typedef struct
{
	const char* data;
	uint64_t len;
	uint64_t pos;
} str_buf_fd;


typedef char* (* prepare_function)(const char* path, const char* path_abs);

inline statistics directory_count(const char* dir);

inline char* explorer_file_prepare_html(const char* file, const char* file_abs);

inline char* explorer_directory_prepare_html(const char* dir, const char* dir_abs);

inline std::string directory_list_html(const char* dir, const char* dir_abs, prepare_function prepare_dir, prepare_function prepare_file);


inline int starts_with(const char* str, const char* prefix);

inline char* path_dirname(const char* path);

inline const char* path_basename(const char* path);

inline char* getcwd();

inline void http_redirect_to_session(struct mg_connection* connection, uint64_t session_cookie, const char* url_fmt, ...);

inline void http_redirect_to(struct mg_connection* connection, const char* url_fmt, ...);

inline char* http_get_ret_path(struct mg_http_message* msg);

inline char* http_get_ret_path_encoded(struct mg_http_message* msg);

inline uint64_t http_get_session_cookie(mg_http_message* msg);

inline uint64_t session_cookie_generate(const char* login, const char* password);

inline bool session_cookie_is_valid(uint64_t session_cookie);

inline user_credentials session_cookie_get_user_credentials(uint64_t session_cookie);

inline void http_send_resource_file(struct mg_connection* connection, struct mg_http_message* msg, const char* rcdata, size_t rcsize);


inline void handle_index_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_favicon_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_login_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_register_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_explorer_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_delete_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_uploader_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_upload_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_mkdir_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_move_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_extension_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_qr_html(struct mg_connection* connection, struct mg_http_message* msg);


inline void send_error_html(struct mg_connection* connection, int code, const char* color)
{
	mg_http_reply(
			connection, code, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(error_html),
			color, color, color, color, code, mg_http_status_code_str(code)
	);
}


inline void handle_http_message(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	if (mg_http_match_uri(msg, "/") || mg_http_match_uri(msg, "/index.html") || mg_http_match_uri(msg, "/index"))
		handle_index_html(connection, msg);
	else if (mg_http_match_uri(msg, "/favicon.ico") || mg_http_match_uri(msg, "/favicon"))
		handle_favicon_html(connection, msg);
	else if (mg_http_match_uri(msg, "/login"))
		handle_login_html(connection, msg, database_user_password);
	else if (mg_http_match_uri(msg, "/register"))
		handle_register_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/explorer/"))
		handle_explorer_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/delete/"))
		handle_delete_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/uploader/"))
		handle_uploader_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/upload/"))
		handle_upload_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/mkdir/"))
		handle_mkdir_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/move/"))
		handle_move_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/extension/"))
		handle_extension_html(connection, msg);
	else if (starts_with(msg->uri.ptr, "/qr/") || starts_with(msg->uri.ptr, "/qr"))
		handle_qr_html(connection, msg);
	else send_error_html(connection, 404, "rgba(147, 0, 0, 0.90)");
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

void server_initialize(const char* database_user_password)
{
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	mg_log_set(log_level);
	mg_mgr_init(&manager);
	
	mariadb_create_db(database_user_password);
	auto connection = mariadb_connect_to_db(database_user_password);
	if (connection)
		mariadb_create_table(connection.get());
	else
		MG_ERROR(("Cannot connect to database using database_user_password. Please check your mariadb configuration and try again."));
	
	config_initialization_thread();
}

void server_run(const char* database_user_password)
{
	if (!(server_connection = mg_http_listen(&manager, address, client_handler, &database_user_password)))
	{
		MG_ERROR(("Cannot start listening on %s. Use 'http://ADDR:PORT' or just ':PORT'", address));
		exit(EXIT_FAILURE);
	}
	
	if (hexdump) server_connection->is_hexdumping = 1;
	
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



inline void handle_index_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	char* ret_path = http_get_ret_path_encoded(msg);
	
	if (*ret_path)
		mg_http_reply(connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(index_html), ret_path, ret_path);
	else
		mg_http_reply(connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(index_html), "", "");
	
	delete[] ret_path;
}


inline void handle_favicon_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	http_send_resource_file(connection, msg, reinterpret_cast<const char*>(favicon_ico), favicon_ico_len);
}

inline void handle_login_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
	mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
	mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
	
	auto conn = mariadb_connect_to_db(database_user_password);
	if (!conn)
	{
		send_error_html(connection, 501, "rgba(147, 120, 0, 0.90)");
		return;
	}
	auto db_password = mariadb_user_get_password(conn.get(), login);
	if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
	{
		char* ret_path = http_get_ret_path(msg);
		
		system((std::string("mkdir -p './") + login + "/'").c_str());
		http_redirect_to_session(connection, session_cookie_generate(login, password), ret_path);
		
		delete[] ret_path;
	}
	else
	{
		char* ret_path = http_get_ret_path_encoded(msg);
		
		mg_http_reply(
				connection, 200, "Content-Type: text/html\r\n",
				reinterpret_cast<const char*>(invalid_credentials_html),
				"Invalid credentials.<br/>Please register or contact support to recover your account.",
				ret_path, login, password, ret_path, login, password
		);
		
		delete[] ret_path;
	}
}

inline void handle_register_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	char login[MAX_LOGIN]{ }, password[MAX_PASSWORD]{ };
	mg_http_get_var(&msg->body, "login", login, MAX_LOGIN);
	mg_http_get_var(&msg->body, "password", password, MAX_PASSWORD);
	
	auto conn = mariadb_connect_to_db(database_user_password);
	if (!conn)
	{
		send_error_html(connection, 501, "rgba(147, 120, 0, 0.90)");
		return;
	}
	if (!mariadb_user_insert(conn.get(), login, password))
	{
		char* ret_path = http_get_ret_path_encoded(msg);
		
		mg_http_reply(
				connection, 200, "Content-Type: text/html\r\n",
				reinterpret_cast<const char*>(invalid_credentials_html),
				"User already exists.<br/>Try out another username.",
				ret_path, "", "", ret_path, login, password
		);
		
		delete[] ret_path;
	}
	else
	{
		char* ret_path = http_get_ret_path(msg);
		
		system((std::string("mkdir -p './") + login + "/'").c_str());
		http_redirect_to_session(connection, session_cookie_generate(login, password), ret_path);
		
		delete[] ret_path;
	}
}

inline void handle_explorer_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	if (session_cookie_is_valid(session_cookie))
	{
		auto user_credentials = session_cookie_get_user_credentials(session_cookie);
		
		char* dir_rel = nullptr;
		auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
		
		char* uri = new char[msg->uri.len + 1];
		strncpy(uri, msg->uri.ptr, msg->uri.len);
		uri[msg->uri.len] = 0;
		
		strscanf(uri, "/explorer/%s", &dir_rel);
		size_t len = (dir_rel ? strlen(dir_rel) + 1 : 0);
		mg_url_decode(dir_rel, len, dir_rel, len, 1);
		sprintf(dir, "%s%s", user_credentials.login.c_str(), (dir_rel ? dir_rel : ""));
		
		delete[] uri;
		
		std::string path("./");
		path += dir;
		
		struct stat st{ };
		if (::stat(path.c_str(), &st) < 0)
		{
			send_error_html(connection, 404, "rgba(147, 0, 0, 0.90)");
			
			delete[] dir;
			delete[] dir_rel;
			return;
		}
		
		if (S_ISREG(st.st_mode))
		{
			struct mg_http_serve_opts opts{ };
			std::string extra_header;
			
			if (st.st_size > MAX_INLINE_FILE_SIZE)
			{
				extra_header = "Content-Disposition: attachment; filename=\"";
				extra_header += path_basename(path.c_str());
				extra_header += "\"\r\n";
				
				opts.extra_headers = extra_header.c_str();
			}
			
			mg_http_serve_file(connection, msg, path.c_str(), &opts);
			
			delete[] dir;
			delete[] dir_rel;
			return;
		}
		
		auto dir_rel_dirname = path_dirname(dir_rel);
		
		mg_http_reply(
				connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(explorer_html),
				dir_rel_dirname, dir_rel_dirname,
				dir_rel_dirname, user_credentials.login.c_str(), dir_rel_dirname, path_basename(dir_rel),
				dir_rel, dir_rel, dir_rel,
				dir_rel, dir_rel,
				directory_list_html(
						dir_rel, dir,
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
		mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3);
		
		http_redirect_to(connection, "/?return_to=%s", curr_url);
		
		delete[] curr_url;
	}
}

inline void handle_delete_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	if (session_cookie_is_valid(session_cookie))
	{
		auto user_credentials = session_cookie_get_user_credentials(session_cookie);
		
		char* dir_rel = nullptr;
		auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
		
		char* uri = new char[msg->uri.len + 1];
		strncpy(uri, msg->uri.ptr, msg->uri.len);
		uri[msg->uri.len] = 0;
		
		strscanf(uri, "/delete/%s", &dir_rel);
		size_t len = (dir_rel ? strlen(dir_rel) + 1 : 0);
		mg_url_decode(dir_rel, len, dir_rel, len, 1);
		sprintf(dir, "%s%s", user_credentials.login.c_str(), (dir_rel ? dir_rel : ""));
		
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
		
		mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
		
		delete[] dir_rel;
		delete[] dir;
	}
}

inline void handle_uploader_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	if (session_cookie_is_valid(session_cookie))
	{
		auto user_credentials = session_cookie_get_user_credentials(session_cookie);
		
		char* dir_rel = nullptr;
		auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
		
		char* uri = new char[msg->uri.len + 1];
		strncpy(uri, msg->uri.ptr, msg->uri.len);
		uri[msg->uri.len] = 0;
		
		strscanf(uri, "/uploader/%s", &dir_rel);
		size_t len = (dir_rel ? strlen(dir_rel) + 1 : 0);
		mg_url_decode(dir_rel, len, dir_rel, len, 1);
		sprintf(dir, "%s%s", user_credentials.login.c_str(), (dir_rel ? dir_rel : ""));
		
		delete[] uri;
		
		std::string path("./");
		path += dir;
		
		struct stat st{ };
		if (::stat(path.c_str(), &st) < 0 && S_ISDIR(st.st_mode))
		{
			send_error_html(connection, 404, "rgba(147, 0, 0, 0.90)");
			
			delete[] dir;
			delete[] dir_rel;
			return;
		}
		
		mg_http_reply(
				connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(uploader_html),
				dir_rel, dir_rel, dir_rel, dir_rel, dir_rel, dir_rel,
				dir_rel, dir_rel, dir_rel, dir_rel
		);
		
		delete[] dir_rel;
		delete[] dir;
	}
	else
	{
		char* curr_url = new char[msg->uri.len * 3]{ };
		mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3);
		
		http_redirect_to(connection, "/?return_to=%s", curr_url);
		
		delete[] curr_url;
	}
}

inline void handle_upload_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	char* filename = new char[1]{ }, * sha = new char[1]{ };
	long long pos = -1;
	
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "start", form_part.name.len))
		{
			auto tmp = strndup(form_part.body.ptr, form_part.body.len);
			MG_INFO(("[start] = %s", tmp));
			pos = strtoll(tmp, nullptr, 10);
			delete[] tmp;
		}
		else if (!strncmp(form_part.name.ptr, "name", form_part.name.len))
		{
			delete[] filename;
			filename = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "sha", form_part.name.len))
		{
			delete[] sha;
			sha = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "file", form_part.name.len) && *filename && pos >= 0)
		{
			if (session_cookie_is_valid(session_cookie))
			{
				auto user_credentials = session_cookie_get_user_credentials(session_cookie);
				
				if (sha256(std::string(form_part.body.ptr, form_part.body.len)) != sha)
				{
					delete[] filename;
					delete[] sha;
					
					mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Fail");
					return;
				}
				
				char* dir_rel = nullptr;
				auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
				
				char* uri = new char[msg->uri.len + 1];
				strncpy(uri, msg->uri.ptr, msg->uri.len);
				uri[msg->uri.len] = 0;
				
				strscanf(uri, "/upload/%s", &dir_rel);
				size_t len = (dir_rel ? strlen(dir_rel) + 1 : 0);
				mg_url_decode(dir_rel, len, dir_rel, len, 1);
				sprintf(dir, "%s%s", user_credentials.login.c_str(), (dir_rel ? dir_rel : ""));
				
				delete[] uri;
				
				std::string path("./");
				path += dir;
				
				struct stat st{ };
				if (::stat(path.c_str(), &st) < 0)
				{
					delete[] dir;
					delete[] dir_rel;
					delete[] filename;
					delete[] sha;
					
					mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Fail");
					return;
				}
				
				path += "/";
				path += filename;
				
				FILE* file = fopen(path.c_str(), "ab");
				fseek(file, pos, SEEK_SET);
				fwrite(form_part.body.ptr, sizeof(char), form_part.body.len, file);
				fclose(file);
				
				mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
				
				delete[] dir;
				delete[] dir_rel;
			}
			else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
		}
		else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
	}
	delete[] filename;
	delete[] sha;
}

inline void handle_mkdir_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "folder", form_part.name.len))
		{
			if (session_cookie_is_valid(session_cookie))
			{
				auto user_credentials = session_cookie_get_user_credentials(session_cookie);
				
				char* dir_rel = nullptr;
				auto dir = new char[msg->uri.len + MAX_LOGIN]{ };
				
				char* uri = new char[msg->uri.len + 1];
				strncpy(uri, msg->uri.ptr, msg->uri.len);
				uri[msg->uri.len] = 0;
				
				strscanf(uri, "/mkdir/%s", &dir_rel);
				size_t len = (dir_rel ? strlen(dir_rel) + 1 : 0);
				mg_url_decode(dir_rel, len, dir_rel, len, 1);
				sprintf(dir, "%s%s", user_credentials.login.c_str(), (dir_rel ? dir_rel : ""));
				
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
				
				path += "/";
				path.append(form_part.body.ptr, form_part.body.len);
				
				system(("mkdir -p '" + path + "'").c_str());
				
				mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
				
				delete[] dir;
				delete[] dir_rel;
			}
			else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
		}
		else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
	}
}

inline void handle_move_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "to", form_part.name.len))
		{
			if (session_cookie_is_valid(session_cookie))
			{
				auto user_credentials = session_cookie_get_user_credentials(session_cookie);
				
				char* file_rel = nullptr;
				auto file = new char[msg->uri.len + MAX_LOGIN]{ };
				
				char* uri = new char[msg->uri.len + 1];
				strncpy(uri, msg->uri.ptr, msg->uri.len);
				uri[msg->uri.len] = 0;
				
				strscanf(uri, "/move/%s", &file_rel);
				size_t file_len = (file_rel ? strlen(file_rel) + 1 : 0);
				mg_url_decode(file_rel, file_len, file_rel, file_len, 1);
				sprintf(file, "%s%s", user_credentials.login.c_str(), (file_rel ? file_rel : ""));
				
				delete[] uri;
				
				std::string path_from("./");
				path_from += file;
				
				struct stat st{ };
				if (::stat(path_from.c_str(), &st) < 0)
				{
					delete[] file;
					delete[] file_rel;
					mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Fail");
					return;
				}
				
				std::string path_to("./");
				path_to += user_credentials.login;
				char* tmp = new char[form_part.body.len + 1]{ };
				mg_url_decode(form_part.body.ptr, form_part.body.len, tmp, form_part.body.len + 1, 1);
				path_to += tmp;
				delete[] tmp;
				
				system(("mkdir -p '" + path_to + "'").c_str());
				
				system(("mv -f '" + path_from + "' '" + path_to + "/'").c_str());
				
				mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
				
				delete[] file;
				delete[] file_rel;
			}
			else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
		}
		else mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Invalid");
	}
}

inline void handle_extension_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	uint64_t session_cookie = http_get_session_cookie(msg);
	
	auto user_credentials = session_cookie_get_user_credentials(session_cookie);
	
	char* uri = new char[msg->uri.len + 1];
	strncpy(uri, msg->uri.ptr, msg->uri.len);
	uri[msg->uri.len] = 0;
	
	char* extension = nullptr, * dir = nullptr;
	
	strscanf(uri, "/extension/%s/%s", &extension, &dir);
	size_t len = (dir ? strlen(dir) + 1 : 0);
	mg_url_decode(dir, len, dir, len, 1);
	
	delete[] uri;
	
	refresh_config_script();
	auto res = call_lua_extension(
			{ .login = user_credentials.login, .session_cookie = session_cookie, .valid = session_cookie_is_valid(session_cookie),
					.name = extension, .argument = (dir ? dir : "") }
	);
	
	mg_http_reply(connection, res.response_code, "Content-Type: text/html\r\n", res.page.c_str());
	
	delete[] dir;
}

inline void handle_qr_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	char* data = new char[msg->query.len]{ }, bg[8]{ }, fg[8]{ };
	mg_http_get_var(&msg->query, "data", data, msg->query.len);
	mg_http_get_var(&msg->query, "bg", bg, 8);
	mg_http_get_var(&msg->query, "fg", fg, 8);
	
	
	std::string bgs, fgs;
	
	if (*bg)
		bgs = bg;
	else
		bgs = "#ffffff";
	
	if (*fg)
		fgs = fg;
	else
		fgs = "#000000";
	
	
	mg_http_reply(
			connection, 200, "Content-Type: image/svg+xml;\r\n",
			To_svg_string(generate_qr_code(data), 0, bgs.c_str(), fgs.c_str()).c_str()
	);
	
	delete[] data;
}


#include "database.cpp"
#include "templates.hpp"


inline static consteval size_t static_strlen(const char* str)
{
	size_t len = 0;
	for (; *str; ++len, ++str);
	return len;
}

inline statistics directory_count(const char* dir)
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

inline const char* get_filename_ext(const char* filename)
{
	const char* dot = nullptr;
	for (const char* dot_tmp = filename; *dot_tmp; ++dot_tmp)
		if (*dot_tmp == '.')
			dot = dot_tmp;
	
	if (!dot || dot == filename) return "";
	return dot + 1;
}

inline char* explorer_directory_prepare_html(const char* dir, const char* dir_abs)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	sprintf(
			html, explorer_dir_html,
			dir, dir,
			dir, dir, dir, dir, path_dirname(dir), path_basename(dir), st.files, st.folders
	);
	return html;
}

inline char* explorer_file_prepare_html(const char* file, const char* file_abs)
{
	struct stat st{ };
	stat((std::string("./") + file_abs).c_str(), &st);
	char* html = new char[static_strlen(explorer_file_html) + 2088];
	auto ext = get_filename_ext(file);
	sprintf(
			html, explorer_file_html,
			file, file,
			file, file, file, file, path_dirname(file), ext, ext, path_basename(file), st.st_size
	);
	return html;
}

inline std::string directory_list_html(const char* dir, const char* dir_abs, prepare_function prepare_dir, prepare_function prepare_file)
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
					(std::string((dir ? dir : "")) + "/" + entry->d_name).c_str(),
					(std::string((dir_abs ? dir_abs : "")) + "/" + entry->d_name).c_str()
			);
			
			result += html;
			
			delete[] html;
		}
		if (entry->d_type == DT_REG)
		{
			auto html = prepare_file(
					(std::string((dir ? dir : "")) + "/" + entry->d_name).c_str(),
					(std::string((dir_abs ? dir_abs : "")) + "/" + entry->d_name).c_str()
			);
			
			result += html;
			
			delete[] html;
		}
	}
	closedir(dirp);
	
	std::string html(result.size() + static_strlen(explorer_dir_content_html), 0);
	
	sprintf(html.data(), explorer_dir_content_html, result.c_str());
	
	return html;
}


inline int starts_with(const char* str, const char* prefix)
{
	for (; *prefix; ++prefix, ++str)
		if (*prefix != *str)
			return false;
	return true;
}

inline char* path_dirname(const char* path)
{
	if (!path) return new char[1]{ };
	char* accesible_path = ::strdup(path);
	char* slash = nullptr;
	for (char* tmp = accesible_path; *tmp; ++tmp)
		if (*tmp == '/')
			slash = tmp;
	
	if (!slash) return new char[1]{ };
	*slash = 0;
	return accesible_path;
}

inline const char* path_basename(const char* path)
{
	if (!path) return "";
	const char* slash = nullptr;
	for (const char* tmp = path; *tmp; ++tmp)
		if (*tmp == '/')
			slash = tmp;
	
	if (!slash) return "";
	return slash + 1;
}

inline char* getcwd()
{
	char* cwd = new char[PATH_MAX];
	getcwd(cwd, PATH_MAX);
	cwd[PATH_MAX - 1] = 0;
	return cwd;
}


inline void http_redirect_to_session(struct mg_connection* connection, uint64_t session_cookie, const char* url_fmt, ...)
{
	char mem[256], * buf = mem;
	va_list ap;
	size_t len;
	va_start(ap, url_fmt);
	len = mg_vasprintf(&buf, sizeof(mem), url_fmt, ap);
	va_end(ap);
	
	mg_printf(
			connection, "HTTP/1.1 307 %s\r\nLocation: %s\r\nSet-Cookie: session=%d\r\nContent-Length: 0\r\n\r\n",
			mg_http_status_code_str(307), buf, session_cookie
	);
	
	if (buf != mem) free(buf);
}

inline void http_redirect_to(struct mg_connection* connection, const char* url_fmt, ...)
{
	char mem[256], * buf = mem;
	va_list ap;
	size_t len;
	va_start(ap, url_fmt);
	len = mg_vasprintf(&buf, sizeof(mem), url_fmt, ap);
	va_end(ap);
	
	mg_printf(
			connection, "HTTP/1.1 307 %s\r\nLocation: %s\r\nContent-Length: 0\r\n\r\n",
			mg_http_status_code_str(307), buf
	);
	
	if (buf != mem) free(buf);
}


inline char* http_get_ret_path(struct mg_http_message* msg)
{
	char* ret_path = new char[msg->query.len]{ };
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len);
	mg_url_decode(ret_path, msg->query.len, ret_path, msg->query.len, 1);
	
	if (!*ret_path)
	{
		delete[] ret_path;
		return new char[]{ "/explorer/" };
	}
	
	return ret_path;
}


inline char* http_get_ret_path_encoded(struct mg_http_message* msg)
{
	char* ret_path = new char[msg->query.len + 1]{ };
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len + 1);
	
	if (!*ret_path)
	{
		delete[] ret_path;
		return new char[]{ "?return_to=%2fexplorer%2f" };
	}
	
	size_t ret_path_len = strlen(ret_path);
	char* ret_path_url = new char[ret_path_len * 3]{ };
	mg_url_encode(ret_path, ret_path_len, ret_path_url, ret_path_len * 3);
	
	char* ret_path_raw = new char[msg->query.len + ret_path_len * 2]{ };
	sprintf(ret_path_raw, "?return_to=%s", ret_path_url);
	
	delete[] ret_path;
	delete[] ret_path_url;
	
	return ret_path_raw;
}



inline uint64_t http_get_session_cookie(mg_http_message* msg)
{
	auto all_session_cookies = mg_http_get_header(msg, "Cookie");
	if (all_session_cookies)
		return strtoull(mg_http_get_header_var(*all_session_cookies, mg_str("session")).ptr, nullptr, 10);
	else return 0;
}


inline uint64_t session_cookie_generate(const char* login, const char* password)
{
	uint64_t session_cookie = 0;
	for (auto tmp = password; *tmp; ++tmp)
		session_cookie |= (uint64_t)(*tmp) << 8;
	session_cookie += time(nullptr);
	session_cookies[session_cookie] = { .login = login, .password = password, .last_access = time(nullptr) };
	return session_cookie;
}


inline bool session_cookie_is_valid(uint64_t session_cookie)
{
	if (!session_cookies.contains(session_cookie))
	{
		session_cookies.erase(session_cookie);
		return false;
	}
	
	if (time(nullptr) - session_cookies[session_cookie].last_access < 120) // for 2 minutes
	{
		session_cookies[session_cookie].last_access = time(nullptr);
		return true;
	}
	
	return false;
}


inline user_credentials session_cookie_get_user_credentials(uint64_t session_cookie) { return session_cookies[session_cookie]; }



static void restore_http_cb_rp(struct mg_connection* c)
{
	delete (str_buf_fd*)c->pfn_data;
	c->pfn_data = nullptr;
	c->pfn = http_cb;
}

static void static_resource_send(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	if (ev == MG_EV_WRITE || ev == MG_EV_POLL)
	{
		auto rc = (str_buf_fd*)fn_data;
		// Read to send IO buffer directly, avoid extra on-stack buffer
		size_t max = MG_IO_SIZE, space, * cl = (size_t*)c->label;
		if (c->send.size < max)
			mg_iobuf_resize(&c->send, max);
		if (c->send.len >= c->send.size)
			return;  // Rate limit
		if ((space = c->send.size - c->send.len) > *cl)
			space = *cl;
		
		memcpy(c->send.buf + c->send.len, &rc->data[rc->pos], space);
		rc->pos += space;
		c->send.len += space;
		*cl -= space;
		if (space == 0)
			restore_http_cb_rp(c);
	}
	else if (ev == MG_EV_CLOSE)
		restore_http_cb_rp(c);
}

inline void http_send_resource_file(struct mg_connection* connection, struct mg_http_message* msg, const char* rcdata, size_t rcsize)
{
	char etag[64];
	time_t mtime = 0;
	struct mg_str* inm = nullptr;
	
	if (mg_http_etag(etag, sizeof(etag), rcsize, mtime) != nullptr &&
	    (inm = mg_http_get_header(msg, "If-None-Match")) != nullptr &&
	    mg_vcasecmp(inm, etag) == 0)
	{
		mg_printf(connection, "HTTP/1.1 304 Not Modified\r\nContent-Length: 0\r\n\r\n");
	}
	else
	{
		int n, status = 200;
		char range[100] = "";
		int64_t r1 = 0, r2 = 0, cl = (int64_t)rcsize;
		struct mg_str mime = MG_C_STR("image/x-icon");
		
		// Handle Range header
		struct mg_str* rh = mg_http_get_header(msg, "Range");
		if (rh != nullptr && (n = getrange(rh, &r1, &r2)) > 0 && r1 >= 0 && r2 >= 0)
		{
			// If range is specified like "400-", set second limit to content len
			if (n == 1) r2 = cl - 1;
			if (r1 > r2 || r2 >= cl)
			{
				status = 416;
				cl = 0;
				mg_snprintf(range, sizeof(range), "Content-Range: bytes */%lld\r\n", (int64_t)rcsize);
			}
			else
			{
				status = 206;
				cl = r2 - r1 + 1;
				mg_snprintf(range, sizeof(range), "Content-Range: bytes %lld-%lld/%lld\r\n", r1, r1 + cl - 1, (int64_t)rcsize);
			}
		}
		mg_printf(
				connection,
				"HTTP/1.1 %d %s\r\n"
				"Content-Type: %.*s\r\n"
				"Etag: %s\r\n"
				"Content-Length: %llu\r\n"
				"\r\n",
				status, mg_http_status_code_str(status), (int)mime.len, mime.ptr,
				etag, cl, range
		);
		if (mg_vcasecmp(&msg->method, "HEAD") == 0)
		{
			connection->is_draining = 1;
		}
		else
		{
			connection->pfn = static_resource_send;
			connection->pfn_data = new str_buf_fd{ .data = rcdata, .len = rcsize, .pos = 0 };
			*(size_t*)connection->label = (size_t)cl;  // Track to-be-sent content length
		}
	}
}

#include "sha256.cpp"