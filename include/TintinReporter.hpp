#ifndef TINTINREPORTER_HPP
#define TINTINREPORTER_HPP

#include <fstream>
#include <ctime>
#include <string>

enum class Log
{
	Info,
	Error,
	Log,
	Sig,
};

class TintinReporter
{
public:
	TintinReporter();
	TintinReporter(const TintinReporter& rhs);

	std::string		timeStamp();
	void			log(Log type, std::string msg);

	~TintinReporter();

	TintinReporter&	operator=(const TintinReporter& rhs);

private:
	std::ofstream	_log;
	std::time_t		_time;
};

#endif
