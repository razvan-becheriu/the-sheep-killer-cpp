#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

/** Replicates logger.pas: appends messages with timestamps to a file named 'log' */
void Log(const std::string& s);

#endif // LOGGER_HPP