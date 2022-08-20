// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#include "database.h"


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
