#pragma once

// usage std::cout << RED << "hello world" << RESET << std::endl;

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define CLI_RESET   "\033[0m"
#define CLI_BLACK   "\033[30m"      /* Black */
#define CLI_RED     "\033[31m"      /* Red */
#define CLI_GREEN   "\033[32m"      /* Green */
#define CLI_YELLOW  "\033[33m"      /* Yellow */
#define CLI_BLUE    "\033[34m"      /* Blue */
#define CLI_MAGENTA "\033[35m"      /* Magenta */
#define CLI_CYAN    "\033[36m"      /* Cyan */
#define CLI_WHITE   "\033[37m"      /* White */
#define CLI_BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define CLI_BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define CLI_BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define CLI_BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define CLI_BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define CLI_BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define CLI_BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define CLI_BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

