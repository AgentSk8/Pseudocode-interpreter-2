#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define windows
#elif __APPLE__
	#define mac
#elif __linux__
	#define linux
#elif __unix__ // all unices not caught above
	#define unix
#elif defined(_POSIX_VERSION)
	#define posix
#else
#   error "Unknown compiler"
#endif
