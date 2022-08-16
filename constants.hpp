//
// Created by imper on 8/16/22.
//

#ifndef FILESTORAGE_CONSTANTS_HPP
#define FILESTORAGE_CONSTANTS_HPP


# define _STR(s) #s
# define MACRO_STR(v) _STR(v)


# ifndef VERSION
#  define VERSION "<git>"
# endif

# ifndef APPNAME
#  define APPNAME "filestorage"
# endif

# ifndef COUNTRY
#  define COUNTRY "UA"
# endif

# ifndef ORGANIZATION
#  define ORGANIZATION "imper"
# endif

# ifndef CERTIFICATE_NAME
#  define CERTIFICATE_NAME "imper"
# endif

# ifndef DEFAULT_PORT
#  define DEFAULT_PORT 144
# endif

# ifndef DEFAULT_SERVER_ADDRESS
#  define DEFAULT_SERVER_ADDRESS INADDR_ANY
# endif


#endif //FILESTORAGE_CONSTANTS_HPP
