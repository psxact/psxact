#ifndef UTIL_LOGGER_HPP_
#define UTIL_LOGGER_HPP_

namespace psx::util {

	class logger {
		const char *name;
		bool enabled;

	public:
		logger(const char *name, bool enabled);

#ifdef WITH_LOGGING
		void log(const char *fmt, ...);
#endif
	};
}

#ifdef WITH_LOGGING
#define LOG_ERROR(msg, ...) logger.log("[ERROR] " msg, ## __VA_ARGS__)
#define LOG_INFO(msg, ...) logger.log("[INFO] " msg, ## __VA_ARGS__)
#define LOG_WARN(msg, ...) logger.log("[WARN] " msg, ## __VA_ARGS__)
#else
#define LOG_ERROR(msg, ...)
#define LOG_INFO(msg, ...)
#define LOG_WARN(msg, ...)
#endif

#endif
