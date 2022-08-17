// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#include "server.h"
#include "mongoose.c"
#include "mariadb/conncpp.hpp"
#include "constants.hpp"


static constexpr const char* error_404_html = R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>ERROR 404</title>
    <style type="text/css">
        @import url(https://fonts.googleapis.com/css?family=Open+Sans:400,700);

        body {
            font-family: 'Open Sans';
            padding: 0;
            margin: 0;
        }

        a,
        a:visited {
            color: #fff;
            outline: none;
            text-decoration: none;
        }

        a:hover,
        a:focus,
        a:visited:hover {
            color: #fff;
            text-decoration: none;
        }

        * {
            paading: 0;
            margin: 0;
        }

        #oopss {
            background: #222;
            text-align: center;
            margin-bottom: 50px;
            font-weight: 400;
            font-size: 20px;
            position: fixed;
            width: 100%;
            height: 100%;
            line-height: 1.5em;
            z-index: 9999;
            left: 0px;
        }

        #error-text {
            top: 30%;
            position: relative;
            font-size: 40px;
            color: #eee;
        }

        #error-text a {
            color: #eeeaaa;
        }

        #error-text a:hover {
            color: #fff;
        }

        #error-text p {
            color: #eee;
            margin: 70px 0 0 0;
        }

        #error-text i {
            margin-left: 10px;
        }

        #error-text p.hmpg {
            margin: 40px 0 0 0;
            font-size: 30px;
        }

        #error-text span {
            position: relative;
            background: #ef4824;
            color: #fff;
            font-size: 300%;
            padding: 0 20px;
            border-radius: 5px;
            font-weight: bolder;
            transition: all .5s;
            cursor: pointer;
            margin: 0 0 40px 0;
        }

        #error-text span:hover {
            background: #d7401f;
            color: #fff;
            -webkit-animation: jelly .5s;
            -moz-animation: jelly .5s;
            -ms-animation: jelly .5s;
            -o-animation: jelly .5s;
            animation: jelly .5s;
        }

        #error-text span:after {
            top: 100%;
            left: 50%;
            border: solid transparent;
            content: '';
            height: 0;
            width: 0;
            position: absolute;
            pointer-events: none;
            border-color: rgba(136, 183, 213, 0);
            border-top-color: #ef4824;
            border-width: 7px;
            margin-left: -7px;
        }

        @-webkit-keyframes jelly {
            from, to {
                -webkit-transform: scale(1, 1);
                transform: scale(1, 1);
            }
            25% {
                -webkit-transform: scale(.9, 1.1);
                transform: scale(.9, 1.1);
            }
            50% {
                -webkit-transform: scale(1.1, .9);
                transform: scale(1.1, .9);
            }
            75% {
                -webkit-transform: scale(.95, 1.05);
                transform: scale(.95, 1.05);
            }
        }

        @keyframes jelly {
            from, to {
                -webkit-transform: scale(1, 1);
                transform: scale(1, 1);
            }
            25% {
                -webkit-transform: scale(.9, 1.1);
                transform: scale(.9, 1.1);
            }
            50% {
                -webkit-transform: scale(1.1, .9);
                transform: scale(1.1, .9);
            }
            75% {
                -webkit-transform: scale(.95, 1.05);
                transform: scale(.95, 1.05);
            }
        }

        /* CSS Error Page Responsive */

        @media only screen and (max-width: 640px) {
            #error-text span {
                font-size: 200%;
            }

            #error-text a:hover {
                color: #fff;
            }
        }

        .back:active {
            -webkit-transform: scale(0.95);
            -moz-transform: scale(0.95);
            transform: scale(0.95);
            background: #f53b3b;
            color: #fff;
        }

        .back:hover {
            background: #4c4c4c;
            color: #fff;
        }

        .back {
            text-decoration: none;
            background: #5b5a5a;
            color: #fff;
            padding: 10px 20px;
            font-size: 20px;
            font-weight: 700;
            line-height: normal;
            text-transform: uppercase;
            border-radius: 3px;
            -webkit-transform: scale(1);
            -moz-transform: scale(1);
            transform: scale(1);
            transition: all 0.5s ease-out;
        }
    </style>
</head>
<body>
<div id='oopss'>
    <div id='error-text'>
        <span>404</span>
        <p>PAGE NOT FOUND</p>
        <p></p>
        <p class='hmpg'><a href='/' class="back">Back To Home</a>, but you also can go <a href='/' class="back">Back To Home</a></p>
    </div>
</div>
</body>
</html>
)";

static constexpr const char* login_page_html = R"(
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Welcome to the storage</title>
    <style type="text/css">
        @import url(https://fonts.googleapis.com/css?family=Inconsolata);

        .login-page {
            width: 360px;
            padding: 8% 0 0;
            margin: auto;
        }

        .form {
            position: relative;
            z-index: 1;
            background: #FFFFFF;
            max-width: 360px;
            margin: 0 auto 100px;
            padding: 45px;
            text-align: center;
            box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
        }

        .form input {
            font-family: "Inconsolata";
            outline: 0;
            background: #f2f2f2;
            width: 100%;
            border: 0;
            margin: 0 0 15px;
            padding: 15px;
            box-sizing: border-box;
            font-size: 14px;
        }

        .form input[type=submit] {
            font-family: "Inconsolata";
            font-weight: 700;
            text-transform: uppercase;
            outline: 0;
            background: #4CAF50;
            width: 100%;
            border: 0;
            padding: 15px;
            color: #FFFFFF;
            font-size: 14px;
            -webkit-transition: all 0.3 ease;
            transition: all 0.3 ease;
            cursor: pointer;
        }

        .form input[type=submit]:hover, .form input[type=submit]:active, .form input[type=submit]:focus {
            background: #43A049;
        }

        .form .message {
            margin: 15px 0 0;
            color: #b3b3b3;
            font-size: 12px;
        }

        .form .message a {
            color: #4CAF50;
            text-decoration: none;
        }

        .form .register-form {
            display: none;
        }

        .container {
            position: relative;
            z-index: 1;
            max-width: 300px;
            margin: 0 auto;
        }

        .container:before, .container:after {
            content: "";
            display: block;
            clear: both;
        }

        .container .info {
            margin: 50px auto;
            text-align: center;
        }

        .container .info h1 {
            margin: 0 0 15px;
            padding: 0;
            font-size: 36px;
            font-weight: 300;
            color: #1a1a1a;
        }

        .container .info span {
            color: #4d4d4d;
            font-size: 12px;
        }

        .container .info span a {
            color: #000000;
            text-decoration: none;
        }

        .container .info span .fa {
            color: #EF3B3A;
        }

        body {
            background: #6b991f; /* fallback for old browsers */
            background: rgb(107, 153, 31);
            background: linear-gradient(90deg, rgba(121, 174, 91, 1) 10%, rgba(78, 124, 2, 1) 75%);
            font-family: "Inconsolata";
            -webkit-font-smoothing: antialiased;
            -moz-osx-font-smoothing: grayscale;
            margin: 0;
            height: 100%;
            min-height: 100%;
        }

        html {
            margin: 0px;
            height: 100%;
            width: 100%;
        }
    </style>
    <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
</head>
<body>
<div class="login-page">
    <div class="form">
        <form class="register-form" method="POST" action="/register">
            <input type="text" placeholder="name"/>
            <input type="password" placeholder="password"/>
            <input type="text" placeholder="email address"/>
            <input type="submit" value="register"/>
            <p class="message">Already registered? <a href="#">Sign In</a></p>
        </form>
        <form class="login-form" method="POST" action="/login" autocomplete="on">
            <input type="text" placeholder="username"/>
            <input type="password" placeholder="password"/>
            <input type="submit" value="login"/>
            <p class="message">Not registered? <a href='#'>Create an account</a></p>
        </form>
    </div>
</div>
</body>
    <script type="text/javascript">
        $('.message a').click(function () {
            $('form').animate({height: "toggle", opacity: "toggle"}, "slow");
        });
    </script>
</html>
)";


const char* address = "http://0.0.0.0:80";
const char* log_level = "2";
const char* directory = ".";
const char* pattern = "#.html";
int hexdump = 0;

// Handle interrupts, like Ctrl-C
static int s_signo = 0;

static void signal_handler(int signo) { s_signo = signo; }


inline void mariadb_create_db(const char* db_user_password)
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
		
		std::unique_ptr<sql::Connection> connection(driver->connect(url, properties));
		
		std::unique_ptr<sql::Statement> stmnt(connection->createStatement());
		
		stmnt->executeQuery("CREATE DATABASE " DB_NAME);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Database creation failed: %s", e.what())); }
}

inline void mariadb_drop_db(const char* db_user_password)
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
		
		std::unique_ptr<sql::Connection> connection(driver->connect(url, properties));
		
		std::unique_ptr<sql::Statement> stmnt(connection->createStatement());
		
		stmnt->executeQuery("DROP DATABASE " DB_NAME);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Database dropping failed: %s", e.what())); }
}

inline std::unique_ptr<sql::Connection> mariadb_connect_to_db(const char* db_user_password)
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

inline void mariadb_create_table(sql::Connection* connection)
{
	if (!connection)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Creating table " TABLE_NAME " in database " DB_NAME " ..."));
	
	try
	{
		std::unique_ptr<sql::Statement> stmnt(connection->createStatement());
		
		stmnt->executeQuery(
				"CREATE TABLE " TABLE_NAME " "
				"( login varchar(" MACRO_STR(MAX_LOGIN) ") NOT NULL PRIMARY KEY,"
				"password varchar(" MACRO_STR(MAX_PASSWORD) ") NOT NULL );"
		);
	}
	catch (sql::SQLException& e) { MG_ERROR(("Table creation failed: %s", e.what())); }
}

inline void mariadb_drop_table(sql::Connection* connection)
{
	if (!connection)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Dropping table " TABLE_NAME " from database " DB_NAME " ..."));
	
	try
	{
		std::unique_ptr<sql::Statement> stmnt(connection->createStatement());
		
		stmnt->executeQuery("DROP TABLE " TABLE_NAME ";");
	}
	catch (sql::SQLException& e) { MG_ERROR(("Table dropping failed: %s", e.what())); }
}

inline void mariadb_save_user(sql::Connection* connection, const char* login, const char* password)
{
	if (!connection)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Saving user '%s' into table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(connection->prepareStatement("UPDATE " TABLE_NAME " SET password=? WHERE login=?;"));
		
		stmnt->setString(1, password);
		stmnt->setString(2, login);
		
		stmnt->executeQuery();
	}
	catch (sql::SQLException& e) { MG_ERROR(("User creation failed: %s", e.what())); }
}

inline void mariadb_update_user(sql::Connection* connection, const char* login, const char* password)
{
	if (!connection)
	{
		MG_ERROR(("Connection is nullptr."));
		return;
	}
	
	MG_DEBUG(("Updating user '%s' in table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(
				connection->prepareStatement("INSERT INTO " TABLE_NAME " ( login, password ) values( ?,? );")
		);
		
		stmnt->setString(1, login);
		stmnt->setString(2, password);
		
		stmnt->executeQuery();
	}
	catch (sql::SQLException& e) { MG_ERROR(("User update failed: %s", e.what())); }
}

inline const char* mariadb_get_user_password(sql::Connection* connection, const char* login)
{
	if (!connection)
	{
		MG_ERROR(("Connection is nullptr."));
		return nullptr;
	}
	
	MG_DEBUG(("Querying %s's password from table " TABLE_NAME " of database " DB_NAME " ...", login));
	
	try
	{
		std::unique_ptr<sql::PreparedStatement> stmnt(connection->prepareStatement("SELECT password FROM " TABLE_NAME " WHERE login=?;"));
		
		stmnt->setString(1, login);
		
		return stmnt->executeQuery()->getString(1).c_str();
	}
	catch (sql::SQLException& e) { MG_ERROR(("Failed to get user password: %s.", e.what())); }
	return nullptr;
}

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
			mg_http_reply(connection, 308, "Location: /myfiles\r\n", login_page_html);
		}
		else if (mg_http_match_uri(msg, "/register"))
		{
			mg_http_reply(connection, 308, "Location: /myfiles\r\n", login_page_html);
		}
		else mg_http_reply(connection, 404, "Content-Type: text/html\r\n", error_404_html);
		
	}
	(void)fn_data;
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
