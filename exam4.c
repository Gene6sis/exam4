#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

int	ft_putstr_fd2(char *str, char *arg)
{
	while (*str)
		write(STDERR_FILENO, str++, 1);
	if (arg)
		while(*arg)
			write(STDERR_FILENO, arg++, 1);
	write(STDERR_FILENO, "\n", 1);
	return (1);
}

int ft_execute(char **av, int i, int tmp_fd, char **env)
{
	av[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(av[0], av, env);
	ft_putstr_fd2("error: cannot execute ", av[0]);//if execve failed i do this 
	return (1);
}

int	main(int argc, char **av, char **env)
{
	int	i = 0;
	int pid = 0;
	int fd[2];
	int tmp_fd = dup(STDIN_FILENO);
	(void)argc;

	while (av[i] && av[i + 1]) //check if the end is reached       // skip le a.out et les operateurs et check qqc apres
	{
		av = &av[i + 1]; //the new av start after the ; or "|" *** this line is very importante			//prend qqc apres
		i = 0;
		//count until we have all informations to execute the next child;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|")) // i = nb de commande dans pipe line
			i++;
		if (strcmp(av[0], "cd") == 0) //cd
		{
			if (i != 2)
				ft_putstr_fd2("error: cd: bad arguments", NULL);
			else if (chdir(av[1]) != 0)
				ft_putstr_fd2("error: cd: cannot change directory to ", av[1]);
		}
		else if (i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0)) //exec in stdout
		{//we enter here when i != 0, it mean we have at least on cmd, and when we are on the last cmd befor a NULL or a ";".
			pid = fork();
			if ( pid == 0){
				if (ft_execute(av, i, tmp_fd, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				while(waitpid(-1, NULL, WUNTRACED) != -1)
					;
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if(i != 0 && strcmp(av[i], "|") == 0) // si pipe line && suivi d'un pipe
		{
			pipe(fd);			// creation pipe
			pid = fork();
			if ( pid == 0)		//enfant
			{
				dup2(fd[1], STDOUT_FILENO);		//STDOUT devient fd1 donc sortie sur fd1
				close(fd[1]);					//close fd1_old
				close(fd[0]);					//close fd0 car entre sur tmp
				if (ft_execute(av, i, tmp_fd, env))		
					return (1);
			}
			else
			{
				close(fd[1]);		//close fd1
				close(tmp_fd);		//close tmp
				tmp_fd = fd[0];		//sortie commande sur fd0 donc tmp = fd0
			}
		}
	}
	close(tmp_fd);
	return (0);
}