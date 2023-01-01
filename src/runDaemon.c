#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>

void run_daemon()
{
	pid_t pid;

	pid = fork();

	if (pid < 0)
		exit(EXIT_FAILURE);

	if (pid > 0)
		exit(EXIT_SUCCESS);

	if (setsid() < 0)
		exit(EXIT_FAILURE);

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0)
		exit(EXIT_FAILURE);

	if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);
	chdir("/");

	for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
	{
		close(x);
	}

	openlog("daemonize", LOG_PID, LOG_DAEMON);
}

struct File {
	char	path[256];
	char	name[256];
};

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("ERROR: a path must be provided!\n");
		return -1;
	}

	if(strlen(argv[1]) > 256)
	{
		printf("ERROR: too many characters in input!\n");
		return -1;
	}

	struct File file;
	strcpy(file.path, argv[1]);

	char *ch;
	ch = strtok(argv[1], "/");

	while(ch != NULL)
	{
		strcpy(file.name, ch);
		ch = strtok(NULL, "/");
	}

	run_daemon();

	pid_t pid;
	pid = fork();

	if (pid < 0)
		exit(EXIT_FAILURE);

	if (pid == 0)
	{
		syslog(LOG_NOTICE, "Starting child process");
		execl(file.path, file.name, (char *) NULL);
		syslog(LOG_NOTICE, "Child process running");
		exit(EXIT_SUCCESS);
	}

	if (pid > 0)
	{
		wait(NULL);
		syslog(LOG_NOTICE, "Child process exited.");
		closelog();
		exit(EXIT_SUCCESS);
	}

	return 0;
}