#include "server.h"
#include "constants.hpp"
#include <getopt.h>


static constexpr const char* short_args = "p:a:l:d:P:DHvh";
static constexpr struct option long_args[] = {
		{ "password",   required_argument, nullptr, 'p' },
		{ "address",    required_argument, nullptr, 'a' },
		{ "loglevel",   required_argument, nullptr, 'l' },
		{ "dir",        required_argument, nullptr, 'd' },
		{ "pattern",    required_argument, nullptr, 'P' },
		{ "destroy-db", no_argument,       nullptr, 'D' },
		{ "hexdump",    no_argument,       nullptr, 'H' },
		{ "version",    no_argument,       nullptr, 'v' },
		{ "help",       no_argument,       nullptr, 'h' },
		{ nullptr, 0,                      nullptr, 0 }
};


inline static void help()
{
	::printf(APPNAME " v" VERSION "\n");
	::printf("Usage: " APPNAME " [OPTIONS]...\n");
	::printf("Runs http server.\n");
	::printf("Options:\n");
	::printf(" --password   | p  <password>  Database user's password.\n");
	::printf(" --address    | a  <address>   Listening address. Default: %s\n", address);
	::printf(" --loglevel   | l  <level>     Set log level (from 0 to 4). Default: %s\n", log_level);
	::printf(" --dir        | d  <dir>       Server directory. Default: %s\n", directory);
	::printf(" --pattern    | P  <pattern>   SSI filename pattern. Default: %s\n", pattern);
	::printf(" --destroy-db | D              Destroy server's database using given password and username=" DB_USER_NAME "\n");
	::printf(" --hexdump    | H              Enable hex dump.\n");
	::printf(" --version    | v              Show version information.\n");
	::printf(" --help       | h              Show this help message.\n");
	::printf("\n");
	
	::exit(807);
}

int main(int argc, char** argv)
{
	char* password = nullptr;
	int option_index, option, destroy_db = 0;
	while ((option = ::getopt_long(argc, argv, short_args, long_args, &option_index)) > 0)
	{
		switch (option)
		{
			case 'p':
				password = strdup(optarg);
				break;
			case 'a':
				address = strdup(optarg);
				break;
			case 'l':
				log_level = strdup(optarg);
				break;
			case 'd':
				directory = strdup(optarg);
				break;
			case 'P':
				pattern = strdup(optarg);
				break;
			case 'D':
				destroy_db = 1;
				break;
			case 'H':
				hexdump = 1;
				break;
			case 'v':
			{
				printf(APPNAME "version: " VERSION "\n");
				help();
				break;
			}
			case 'h':
			{
				help();
				break;
			}
			default:
				help();
		}
	}
	
	
	if (password == nullptr)
	{
		::fprintf(stderr, APPNAME ": ERROR: No password specified in command line arguments\n");
		help();
	}
	
	server_initialize(password);
	
	if (destroy_db)
		server_destroy_database(password);
	else
		server_run(password);
	
	
	return 0;
}
