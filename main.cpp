#include "server.h"
#include "constants.hpp"
#include <getopt.h>


static constexpr const char* short_args = "a:l:d:p:Hvh";
static constexpr struct option long_args[] = {
		{ "address",  required_argument, nullptr,  'a' },
		{ "loglevel", required_argument, nullptr,  'l' },
		{ "dir",      required_argument, nullptr,  'd' },
		{ "pattern",  required_argument, nullptr,  'p' },
		{ "hexdump",  no_argument,       &hexdump, 'H' },
		{ "version",  no_argument,       nullptr,  'v' },
		{ "help",     no_argument,       nullptr,  'h' },
		{ nullptr, 0,                    nullptr,  0 }
};


inline static void help()
{
	::printf(APPNAME " v" VERSION "\n");
	::printf("Usage: " APPNAME " [OPTIONS]...\n");
	::printf("Runs http server.\n");
	::printf("Options:\n");
	::printf(" --address  | a  <address>  Listening address. Default: %s\n", address);
	::printf(" --loglevel | l  <level>    Set log level (from 0 to 4). Default: %s\n", log_level);
	::printf(" --dir      | d  <dir>      Server directory. Default: %s\n", directory);
	::printf(" --pattern  | p  <pattern>  SSI filename pattern. Default: %s\n", pattern);
	::printf(" --hexdump  | H             Enable hex dump.\n");
	::printf(" --version  | v             Show version information.\n");
	::printf(" --help     | h             Show this help message.\n");
	
	::exit(807);
}

int main(int argc, char** argv)
{
	int option_index;
	int option;
	while ((option = ::getopt_long(argc, argv, short_args, long_args, &option_index)))
	{
		switch (option)
		{
			case 'a':
				address = strdup(optarg);
				break;
			case 'l':
				log_level = strdup(optarg);
				break;
			case 'd':
				directory = strdup(optarg);
				break;
			case 'p':
				pattern = strdup(optarg);
				break;
			case 'H':
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
	
	initialize_server();
	run_server();
	
	return 0;
}
