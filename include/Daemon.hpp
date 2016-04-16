#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fstream>

#include "TintinReporter.hpp"

class Daemon
{
public:
	Daemon();
	Daemon(const Daemon& rhs);

	TintinReporter&		reporter()	{ return _reporter; }
	int					lock()		{ return _lock; }
	static Daemon*		instance()	{ _instance = (!_instance) ? new Daemon : _instance; return _instance; }

	void				resetServer();
	void				runServer();
	void				newConnection();
	void				handleIO();

	Daemon&				operator=(const Daemon& rhs);

	~Daemon();

private:
	int						_sockfd;
	int						_newfd;
	struct sockaddr_in		_sAddr;
	int						_clients[3];
	struct sockaddr_in		_cAddr;
	socklen_t				_sinSize;
	int						_ndfs;
	fd_set					_readfds;
	struct timeval			_tv;
	bool					_running;
	TintinReporter			_reporter;
	int						_lock;

	static Daemon*			_instance;
};

#endif
