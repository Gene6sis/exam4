#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


void	putstr(char *error, char *arg) {
	for (int i = 0; error[i]; i++)
		write(2,&error[i],1);
	if (arg)
		for (int i = 0; arg[i]; i++)
			write(2,&arg[i],1);
	write(2,"\n",1);
}

int main(int argc, char **av, char **envp){
	int i = 0;
	int pid = 0;
	int fd[2];
	int fd_tmp = dup(STDIN_FILENO);

	while(av[i] && av[i + 1]){
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (strcmp(av[0], "cd") == 0) {
			if (i != 2)
				putstr("error: cd: bad arguments", NULL);
			else if (chdir(av[1]))
				putstr("error: cd: cannot change directory to ", av[i]);
		}
	}
	close(fd_tmp);
	return (0);
}