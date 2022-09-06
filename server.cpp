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

typedef char* (* prepare_function)(const char* path, const char* path_abs, const char* login, const char* password);

inline statistics directory_count(const char* dir);

inline char* deleter_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password);

inline char* explorer_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password);

inline char* deleter_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password);

inline char* explorer_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password);

inline std::string directory_list_html(
		const char* dir, const char* dir_abs, const char* login, const char* password,
		prepare_function prepare_dir, prepare_function prepare_file
);


inline int starts_with(const char* str, const char* prefix);

inline char* path_dirname(const char* path);

inline const char* path_basename(const char* path);

inline char* getcwd();


inline void http_redirect_to(struct mg_connection* connection, const char* url_fmt, ...);

inline char* http_get_ret_path(struct mg_http_message* msg);

inline char* http_get_ret_path_raw(struct mg_http_message* msg);


inline void handle_index_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void handle_login_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_register_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_explorer_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_deleter_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_delete_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_uploader_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_upload_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_mkdir_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_move_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_extension_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password);

inline void handle_qr_html(struct mg_connection* connection, struct mg_http_message* msg);

inline void send_error_html(struct mg_connection* connection, int code, const char* color)
{
	mg_http_reply(
			connection, code, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(error_html),
			"rgba(147, 0, 0, 0.90)", "rgba(147, 0, 0, 0.90)", "rgba(147, 0, 0, 0.90)", "rgba(147, 0, 0, 0.90)", code
	);
}


inline void handle_http_message(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	if (mg_http_match_uri(msg, "/") || mg_http_match_uri(msg, "/index.html") || mg_http_match_uri(msg, "/index"))
		handle_index_html(connection, msg);
	else if (mg_http_match_uri(msg, "/login"))
		handle_login_html(connection, msg, database_user_password);
	else if (mg_http_match_uri(msg, "/register"))
		handle_register_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/explorer/"))
		handle_explorer_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/deleter/"))
		handle_deleter_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/delete/"))
		handle_delete_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/uploader/"))
		handle_uploader_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/upload/"))
		handle_upload_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/mkdir/"))
		handle_mkdir_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/move/"))
		handle_move_html(connection, msg, database_user_password);
	else if (starts_with(msg->uri.ptr, "/extension/"))
		handle_extension_html(connection, msg, database_user_password);
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
	mariadb_create_table(connection.get());
	
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
	char* ret_path = http_get_ret_path_raw(msg);
	
	if (*ret_path)
		mg_http_reply(connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(index_html), ret_path, ret_path);
	else
		mg_http_reply(connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(index_html), "", "");
	
	delete[] ret_path;
}

inline void handle_login_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
	if (!mariadb_user_insert(conn.get(), login, password))
	{
		char* ret_path = http_get_ret_path_raw(msg);
		
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
		http_redirect_to(connection, ret_path);
		
		delete[] ret_path;
	}
}

inline void handle_explorer_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
				dir_rel_dirname, dir_rel_dirname, login, password,
				dir_rel_dirname, dir_rel_dirname, path_basename(dir_rel),
				dir_rel, dir_rel, login, password, dir_rel,
				dir_rel, dir_rel, login, password, dir_rel,
				dir_rel, login, password, dir_rel,
				directory_list_html(
						dir_rel, dir, login, password,
						explorer_directory_prepare_html, explorer_file_prepare_html
				).c_str(),
				login, password
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

inline void handle_deleter_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
			send_error_html(connection, 404, "rgba(147, 0, 0, 0.90)");
			
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
				connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(deleter_html),
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
		mg_url_encode(msg->uri.ptr, msg->uri.len, curr_url, msg->uri.len * 3);
		
		http_redirect_to(connection, "/?return_to=%s", curr_url);
		
		delete[] curr_url;
	}
}

inline void handle_delete_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
		
		mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Ok");
		
		delete[] dir_rel;
		delete[] dir;
	}
}

inline void handle_uploader_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
			send_error_html(connection, 404, "rgba(147, 0, 0, 0.90)");
			
			delete[] dir;
			delete[] dir_rel;
			return;
		}
		
		mg_http_reply(
				connection, 200, "Content-Type: text/html\r\n", reinterpret_cast<const char*>(uploader_html),
				dir_rel, dir_rel, login, password, dir_rel, dir_rel, dir_rel, dir_rel,
				dir_rel, login, password, dir_rel, login, password, dir_rel, login, password, dir_rel, login, password
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

inline void handle_upload_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	char* login = new char[1]{ }, * password = new char[1]{ }, * filename = new char[1]{ }, * sha = new char[1]{ };
	long long pos = -1;
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "login", form_part.name.len))
		{
			delete[] login;
			login = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "password", form_part.name.len))
		{
			delete[] password;
			password = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "start", form_part.name.len))
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
			auto conn = mariadb_connect_to_db(database_user_password);
			auto db_password = mariadb_user_get_password(conn.get(), login);
			if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
			{
				if (sha256(std::string(form_part.body.ptr, form_part.body.len)) != sha)
				{
					delete[] login;
					delete[] password;
					delete[] filename;
					delete[] sha;
					
					mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "Fail");
					return;
				}
				
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
	delete[] login;
	delete[] password;
	delete[] filename;
	delete[] sha;
}

inline void handle_mkdir_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	char* login = new char[1]{ }, * password = new char[1]{ };
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "login", form_part.name.len))
		{
			delete[] login;
			login = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "password", form_part.name.len))
		{
			delete[] password;
			password = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "folder", form_part.name.len))
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
				
				strscanf(uri, "/mkdir/%s", &dir_rel);
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
	delete[] login;
	delete[] password;
}

inline void handle_move_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
{
	char* login = new char[1]{ }, * password = new char[1]{ };
	
	struct mg_http_part form_part{ };
	size_t ofs = 0;
	while ((ofs = mg_http_next_multipart(msg->body, ofs, &form_part)) > 0)
	{
		MG_INFO((
				        "Chunk name: [%.*s] filename: [%.*s] length: %lu bytes",
						        form_part.name.len, form_part.name.ptr, form_part.filename.len,
						        form_part.filename.ptr, form_part.body.len
		        ));
		if (!strncmp(form_part.name.ptr, "login", form_part.name.len))
		{
			delete[] login;
			login = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "password", form_part.name.len))
		{
			delete[] password;
			password = strndup(form_part.body.ptr, form_part.body.len);
		}
		else if (!strncmp(form_part.name.ptr, "to", form_part.name.len) && *form_part.body.ptr == '/')
		{
			auto conn = mariadb_connect_to_db(database_user_password);
			auto db_password = mariadb_user_get_password(conn.get(), login);
			if (db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0)
			{
				char* file_rel;
				auto file = new char[msg->uri.len + MAX_LOGIN]{ };
				
				char* uri = new char[msg->uri.len + 1];
				strncpy(uri, msg->uri.ptr, msg->uri.len);
				uri[msg->uri.len] = 0;
				
				strscanf(uri, "/move/%s", &file_rel);
				sprintf(file, "%s%s", login, file_rel);
				
				delete[] uri;
				
				std::string path_from("./");
				path_from += file;
				
				struct stat st{ };
				if (::stat(path_from.c_str(), &st) < 0)
				{
					delete[] file;
					delete[] file_rel;
					delete[] login;
					delete[] password;
					mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "Fail");
					return;
				}
				
				std::string path_to("./");
				path_to += login;
				path_to.append(form_part.body.ptr, form_part.body.len);
				
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
	delete[] login;
	delete[] password;
}

inline void handle_extension_html(struct mg_connection* connection, struct mg_http_message* msg, const char* database_user_password)
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
	bool valid = db_password && !strcmp(db_password, password) && strcmp(db_password, "") != 0;
	
	char* uri = new char[msg->uri.len + 1];
	strncpy(uri, msg->uri.ptr, msg->uri.len);
	uri[msg->uri.len] = 0;
	
	char* extension, * dir;
	
	strscanf(uri, "/extension/%s/%s", &extension, &dir);
	
	init_config_script();
	auto res = call_lua_extension({ .login = login, .password = password, .name = extension, .argument = dir, .valid = valid });
	
	mg_http_reply(connection, res.response_code, "Content-Type: text/html\r\n", res.page.c_str());
	
	delete[] dir;
}

inline void handle_qr_html(struct mg_connection* connection, struct mg_http_message* msg)
{
	char* data = new char[msg->query.len]{ }, bg[8]{ }, fg[8]{ };
	mg_http_get_var(&msg->query, "data", data, msg->query.len);
	mg_http_get_var(&msg->query, "bg", bg, 8);
	mg_http_get_var(&msg->query, "fg", fg, 8);
	
	if (*data)
	{
		int last = mg_url_decode(data, msg->query.len, data, msg->query.len, 1);
		data[last] = 0;
	}
	
	
	std::string bgs, fgs;
	
	if (*bg)
	{
		int last = mg_url_decode(bg, 8, bg, 8, 1);
		data[last] = 0;
		bgs = bg;
	}
	else bgs = "#ffffff";
	
	if (*fg)
	{
		int last = mg_url_decode(fg, 8, fg, 8, 1);
		data[last] = 0;
		fgs = fg;
	}
	else fgs = "#000000";
	
	
	mg_http_reply(
			connection, 200, "Content-Type: image/svg+xml;\r\n",
			To_svg_string(generate_qr_code(data), 0, bgs.c_str(), fgs.c_str()).c_str()
	);
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

inline char* explorer_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	sprintf(
			html, explorer_dir_html,
			dir, dir, login, password,
			dir, dir, dir, login, password, path_basename(dir), st.files, st.folders
	);
	return html;
}

inline char* explorer_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password)
{
	struct stat st{ };
	stat((std::string("./") + file_abs).c_str(), &st);
	char* html = new char[static_strlen(explorer_file_html) + 2088];
	auto ext = get_filename_ext(file);
	sprintf(
			html, explorer_file_html,
			file, file, login, password,
			file, file, file, login, password, ext, ext, path_basename(file), st.st_size
	);
	return html;
}

inline char* deleter_directory_prepare_html(const char* dir, const char* dir_abs, const char* login, const char* password)
{
	statistics st = directory_count(dir_abs);
	char* html = new char[static_strlen(explorer_dir_html) + 2088];
	char* dir_dirname = path_dirname(dir);
	sprintf(html, deleter_dir_html, dir, dir, login, password, dir_dirname, login, password, path_basename(dir), st.files, st.folders);
	delete[] dir_dirname;
	return html;
}

inline char* deleter_file_prepare_html(const char* file, const char* file_abs, const char* login, const char* password)
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

inline std::string directory_list_html(
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


inline int starts_with(const char* str, const char* prefix)
{
	for (; *prefix; ++prefix, ++str)
		if (*prefix != *str)
			return false;
	return true;
}

inline char* path_dirname(const char* path)
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

inline const char* path_basename(const char* path)
{
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
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len);
	
	if (!*ret_path)
	{
		delete[] ret_path;
		return new char[]{ "/explorer/" };
	}
	
	int last = mg_url_decode(ret_path, msg->query.len, ret_path, msg->query.len, 1);
	ret_path[last] = 0;
	
	return ret_path;
}


inline char* http_get_ret_path_raw(struct mg_http_message* msg)
{
	char* ret_path = new char[msg->query.len]{ };
	mg_http_get_var(&msg->query, "return_to", ret_path, msg->query.len);
	
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

#include "sha256.cpp"