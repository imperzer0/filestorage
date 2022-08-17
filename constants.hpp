//
// Created by imper on 8/16/22.
//

#ifndef FILESTORAGE_CONSTANTS_HPP
#define FILESTORAGE_CONSTANTS_HPP


# define _STR(s) #s
# define MACRO_STR(v) _STR(v)


# ifndef VERSION
#  define VERSION "(devel)"
# endif

# ifndef APPNAME
#  define APPNAME "filestorage"
# endif

# ifndef DB_USER_NAME
#  define DB_USER_NAME "filestorage"
# endif

# ifndef DB_NAME
#  define DB_NAME "filestorage"
# endif

# ifndef TABLE_NAME
#  define TABLE_NAME "users"
# endif

# ifndef MAX_LOGIN
#  define MAX_LOGIN 63
# endif

# ifndef MAX_PASSWORD
#  define MAX_PASSWORD 255
# endif

# ifndef DEFAULT_SERVER_ADDRESS
#  define DEFAULT_SERVER_ADDRESS "http://0.0.0.0:80"
# endif


#endif //FILESTORAGE_CONSTANTS_HPP
