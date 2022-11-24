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

int exec(int i, char **av, char **envp, int fd_tmp){
	av[i] = NULL;
	dup2(fd_tmp, STDIN_FILENO);
	close(fd_tmp);
	execve(av[0], av, envp);
	putstr("error: cannot execute ", av[0]);
	return (1);
}

int main(int argc, char **av, char **envp){
	int i = 0;
	int pid = 0;
	int fd[2];
	int fd_tmp = dup(STDIN_FILENO);
	(void)argc;

	while(av[i] && av[i + 1]){
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (strcmp(av[0], "cd") == 0) {
			if (i != 2)
				putstr("error: cd: bad arguments", NULL);
			else if (chdir(av[1]))
				putstr("error: cd: cannot change directory to ", av[1]);
		}
		else if (i && (av[i] == NULL || strcmp(av[i], ";") == 0)) {
			pid = fork();
			if (pid == 0) {	//enfant exec
				exec(i, av, envp, fd_tmp);
				return (1);
			}
			else {			//parent exec
				close(fd_tmp);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					;
				dup2(fd_tmp, STDIN_FILENO);
			}
		}
		else if (i && strcmp(av[i], "|") == 0) {
			pipe(fd);
			pid = fork();
			if (pid == 0) {
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
				exec(i, av, envp, fd_tmp);
				return(1);
			}
			else {
				close(fd[1]);
				close(fd_tmp);
				fd_tmp = fd[0];
			}
		}
	}
	close(fd_tmp);
	return (0);
}