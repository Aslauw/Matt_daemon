#include "TintinReporter.hpp"
#include <iostream>
#include <string.h>

// Ctors
TintinReporter::TintinReporter()
{
	_log.open("./var/log/matt_daemon/matt_daemon.log", std::ios::out | std::ios::app);
}

TintinReporter::TintinReporter(const TintinReporter& rhs)
{
	*this = rhs;
}

// Functions
std::string
TintinReporter::timeStamp()
{
	char		strtime[65];
	struct tm*	timeinfo;

	memset(strtime, 0, 65);
	_time = std::time(nullptr);
	timeinfo = localtime(&_time);
	strftime(strtime, 64, "[%d/%m/%Y-%H:%M:%S]", timeinfo);
	return std::string(strtime);
}

void
TintinReporter::log(Log type, std::string msg)
{
	std::string logType;
	std::string logMsg;


//	msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
	if (!msg.empty() && msg[msg.length() - 1] == '\n')
		msg.erase(msg.length() - 1);
	
	if (!_log.is_open())
		_log.open("./var/log/matt_daemon/matt_daemon.log", std::ios::out | std::ios::app);

	if (type == Log::Info)
		logType = " [INFO]  ";
	else if (type == Log::Error)
		logType = " [ERROR] ";
	else if (type == Log::Log)
		logType = " [LOG]   ";
	else
		logType = " [SIG]   ";

	logMsg = timeStamp() + logType + msg;
	if (_log.is_open())
		_log << logMsg << std::endl;
}

// Otors
TintinReporter&
TintinReporter::operator=(const TintinReporter& rhs)
{
	_log.copyfmt(rhs._log);
	_log.clear(rhs._log.rdstate());
	_log.basic_ios<char>::rdbuf(rhs._log.rdbuf());
	_time = rhs._time;
	return *this;
}

// Dtor
TintinReporter::~TintinReporter()
{
}
