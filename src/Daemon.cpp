#include "Daemon.hpp"

#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>

// Static funtions
static void
sigHandler(int sig)
{
	Daemon::instance()->reporter().log(Log::Sig, "Caught Signal [" + std::string(strsignal(sig)) + "]");
	Daemon::instance()->reporter().log(Log::Info, "Quitting.");
	close(Daemon::instance()->lock());
	remove("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock");
	exit(EXIT_SUCCESS);
}

// Ctors
Daemon::Daemon()
{
	_reporter.log(Log::Info, "Started.");

	_lock = open("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock", O_CREAT, 0666);
	if (flock(_lock, LOCK_EX) < 0)
	{
		perror("flock");
		close(_lock);
		_reporter.log(Log::Error, "File locked.");
		_reporter.log(Log::Info, "Quitting.");
		exit(EXIT_FAILURE);
	}

	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		_reporter.log(Log::Error, "Could not create socket.");
		exit(-1);
	}

	_sAddr.sin_family = AF_INET;
	_sAddr.sin_port = htons(4242);
	_sAddr.sin_addr.s_addr = INADDR_ANY;
	std::memset(&(_sAddr.sin_zero), 0, 8);

	if (bind(_sockfd, (struct sockaddr*)&_sAddr, sizeof(struct sockaddr)) < 0)
	{
		_reporter.log(Log::Error, "Could not bind socket.");
		close(_lock);
		remove("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock");
		exit(-2);
	}
	if (listen(_sockfd, 3) < 0)
	{
		_reporter.log(Log::Error, "Could not listen to port.");
		close(_lock);
		remove("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock");
		exit(-3);
	}

	for (int i = 1; i <= 31; i++)
		signal(i, &sigHandler);

	_running = false;
}

Daemon::Daemon(const Daemon& rhs)
{
	*this = rhs;
}

// Functions
void
Daemon::resetServer()
{
	_tv.tv_sec = 0;
	_tv.tv_usec = 0;
	FD_ZERO(&_readfds);
	for (int i = 0; i < 3; i++)
	{
		if (_clients[i] != 0)
			FD_SET(_clients[i], &_readfds);
	}
	FD_SET(_sockfd, &_readfds);
}

void
Daemon::runServer()
{
	_running = true;

	_sinSize = sizeof(struct sockaddr_in);
	std::memset(_clients, 0, 3 * sizeof(int));
	_ndfs = _sockfd;

	_reporter.log(Log::Info, "Server Running.");
	while (_running)
	{
		resetServer();
		if (select(_ndfs + 1, &_readfds, NULL, NULL, &_tv) >= 0)
		{
			if (FD_ISSET(_sockfd, &_readfds))
				newConnection();
			handleIO();
		}
		else
		{
			_reporter.log(Log::Error, "Could not select.");
			_running = false;
		}
	}
}

void
Daemon::newConnection()
{
	int i;

	_reporter.log(Log::Info, "Connection request.");
	if ((_newfd = accept(_sockfd, (struct sockaddr*)&_sAddr, &_sinSize)) < 0)
	{
		_reporter.log(Log::Error, "Connection refused.");
		return;
	}

	for (i = 0; i < 3; i++)
	{
		if (_clients[i] == 0)
		{
			_clients[i] = _newfd;
			break;
		}
	}
	if (i != 3)
		_ndfs = (_newfd > _ndfs) ? _newfd : _ndfs;
	else
	{
		_reporter.log(Log::Error, "No place for new connection.");
		close(_newfd);
	}
	_reporter.log(Log::Info, "Connection accepted.");
}

void
Daemon::handleIO()
{
	int n = 0;
	char buf[512];

	for (int i = 0; i < 3; i++)
	{
		if (FD_ISSET(_clients[i], &_readfds))
		{
			std::memset(buf, 0, 512);
			if ((n = recv(_clients[i], buf, 511, 0)) <= 0)
			{
				close(_clients[i]);
				_clients[i] = 0;
				_reporter.log(Log::Info, "Connection lost.");
			}
			else
			{
				if (strncmp(buf, "quit", 4) == 0)
				{
					_reporter.log(Log::Info, "Quit request.");
					_reporter.log(Log::Info, "Quitting.");
					close(_lock);
					remove("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock");
					exit(EXIT_SUCCESS);
				}
				buf[n] = 0;
				_reporter.log(Log::Log, std::string(buf));
			}
		}
	}
}

// Otors
Daemon&
Daemon::operator=(const Daemon& rhs)
{
	_sockfd = rhs._sockfd;
	_newfd = rhs._newfd;
	_sAddr = rhs._sAddr;
	std::memcpy(_clients, rhs._clients, 3 * sizeof(int));
	_cAddr = rhs._cAddr;
	_sinSize = rhs._sinSize;
	_ndfs = rhs._ndfs;
	_readfds = rhs._readfds;
	_tv = rhs._tv;
	_running = rhs._running;
	_reporter = rhs._reporter;
	_lock = rhs._lock;
	return *this;
}

// Dtor
Daemon::~Daemon()
{
	_reporter.log(Log::Info, "Quitting.");
	for (int i = 0; i < 3; i++)
	{
		if (_clients[i])
			close(_clients[i]);
	}
	close(_sockfd);
	close(_lock);
	remove("/nfs/2013/l/lbinet/Projects/C++/Matt_daemon/var/lock/matt_daemon.lock");
}
