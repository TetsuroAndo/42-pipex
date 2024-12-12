/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 08:49:33 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	wait_children(void)
{
	int	status;

	status = 0;
	while (wait(&status) > 0)
		;
}

int	main(int argc, char **argv, char **envp)
{
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		infile;
	int		outfile;

	if (argc != 5)
		exit_with_error("Invalid number of arguments.\n", 1);
	infile = open(argv[1], O_RDONLY);
	if (infile < 0)
		exit_with_error("Input file error", 1);
	outfile = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfile < 0)
		exit_with_error("Output file error", 1);
	if (pipe(fd) == -1)
		exit_with_error("Pipe error", 1);
	pid1 = fork();
	if (pid1 < 0)
		exit_with_error("Fork error", 1);
	if (pid1 == 0)
		child_proc_first(fd, argv, envp, infile);
	pid2 = fork();
	if (pid2 < 0)
		exit_with_error("Fork error", 1);
	if (pid2 == 0)
		child_proc_second(fd, argv, envp, outfile);
	close_pipes(fd);
	close(infile);
	close(outfile);
	wait_children();
	return (0);
}

void	ft_free_split(char **split)
{
	size_t	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
}

/* utils */

void	exit_with_error(char *msg, int code)
{
	perror(msg);
	exit(code);
}

void	close_pipes(int fd[2])
{
	close(fd[0]);
	close(fd[1]);
}

void	execute_cmd(char *cmd, char **envp)
{
	char	**args;
	char	*path;

	args = ft_split(cmd, ' ');
	if (!args || !args[0])
	{
		if (args)
			ft_free_split(args);
		exit_with_error("Command parse error", 127);
	}
	path = get_cmd_path(args[0], envp);
	if (!path)
	{
		ft_free_split(args);
		exit_with_error("Command not found", 127);
	}
	if (execve(path, args, envp) == -1)
	{
		free(path);
		ft_free_split(args);
		exit_with_error("Execve error", 127);
	}
}

void	child_proc_first(int fd[2], char **argv, char **envp, int infile)
{
	if (dup2(infile, STDIN_FILENO) < 0)
		exit_with_error("Dup error", 1);
	if (dup2(fd[1], STDOUT_FILENO) < 0)
		exit_with_error("Dup error", 1);
	close_pipes(fd);
	close(infile);
	execute_cmd(argv[2], envp);
}

void	child_proc_second(int fd[2], char **argv, char **envp, int outfile)
{
	if (dup2(fd[0], STDIN_FILENO) < 0)
		exit_with_error("Dup error", 1);
	if (dup2(outfile, STDOUT_FILENO) < 0)
		exit_with_error("Dup error", 1);
	close_pipes(fd);
	close(outfile);
	execute_cmd(argv[3], envp);
}

/* path */
static char	*join_path(const char *p, const char *cmd)
{
	char	*tmp;
	char	*joined;

	tmp = ft_strjoin(p, "/");
	if (!tmp)
		return (NULL);
	joined = ft_strjoin(tmp, cmd);
	free(tmp);
	return (joined);
}

static char	*check_path(char **paths, char *cmd)
{
	int		i;
	char	*fullpath;
	int		ret;

	i = 0;
	fullpath = NULL;
	while (paths && paths[i])
	{
		fullpath = join_path(paths[i], cmd);
		if (!fullpath)
			return (NULL);
		ret = access(fullpath, X_OK);
		if (ret == 0)
			return (fullpath);
		free(fullpath);
		i++;
	}
	return (NULL);
}

char	*get_cmd_path(char *cmd, char **envp)
{
	int		i;
	char	**paths;
	char	*path_env;
	char	*res;

	i = 0;
	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = NULL;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_env = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	res = check_path(paths, cmd);
	ft_free_split(paths);
	return (res);
}
