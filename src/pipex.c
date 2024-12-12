/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/11 20:57:43 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

static char	*get_path_line(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static void	exec_child1(t_pipex *px, char *cmd, int fd[2])
{
	char	**cmd_args;
	char	*cmd_path;

	close(fd[0]);
	if (dup2(px->infile_fd, STDIN_FILENO) < 0)
		error_exit("dup2 error");
	if (dup2(fd[1], STDOUT_FILENO) < 0)
		error_exit("dup2 error");
	close(fd[1]);
	cmd_args = ft_split(cmd, ' ');
	if (!cmd_args)
		error_exit("split error");
	cmd_path = find_command_path(cmd_args[0], px->paths);
	if (!cmd_path)
	{
		perror(cmd_args[0]);
		free_split(cmd_args);
		exit(127);
	}
	execve(cmd_path, cmd_args, px->envp);
	perror("execve");
	free_split(cmd_args);
	free(cmd_path);
	exit(127);
}

static void	exec_child2(t_pipex *px, char *cmd, int fd[2])
{
	char	**cmd_args;
	char	*cmd_path;

	close(fd[1]);
	if (dup2(fd[0], STDIN_FILENO) < 0)
		error_exit("dup2 error");
	if (dup2(px->outfile_fd, STDOUT_FILENO) < 0)
		error_exit("dup2 error");
	close(fd[0]);
	cmd_args = ft_split(cmd, ' ');
	if (!cmd_args)
		error_exit("split error");
	cmd_path = find_command_path(cmd_args[0], px->paths);
	if (!cmd_path)
	{
		perror(cmd_args[0]);
		free_split(cmd_args);
		exit(127);
	}
	execve(cmd_path, cmd_args, px->envp);
	perror("execve");
	free_split(cmd_args);
	free(cmd_path);
	exit(127);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	px;
	int		fd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status;
	char	*path_line;

	if (argc != 5)
	{
		fprintf(stderr, "Usage: %s file1 cmd1 cmd2 file2\n", argv[0]);
		return (1);
	}
	px.infile_fd = open(argv[1], O_RDONLY);
	if (px.infile_fd < 0)
	{
		perror(argv[1]);
		return (1);
	}
	px.outfile_fd = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (px.outfile_fd < 0)
	{
		perror(argv[4]);
		close(px.infile_fd);
		return (1);
	}
	px.envp = envp;
	path_line = get_path_line(envp);
	if (path_line == NULL)
		px.paths = NULL;
	else
		px.paths = ft_split(path_line, ':');
	if (pipe(fd) == -1)
		error_exit("pipe error");
	pid1 = fork();
	if (pid1 < 0)
		error_exit("fork error");
	if (pid1 == 0)
		exec_child1(&px, argv[2], fd);
	pid2 = fork();
	if (pid2 < 0)
		error_exit("fork error");
	if (pid2 == 0)
		exec_child2(&px, argv[3], fd);
	close(fd[0]);
	close(fd[1]);
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	close(px.infile_fd);
	close(px.outfile_fd);
	if (px.paths)
		free_split(px.paths);
	return (WEXITSTATUS(status));
}

char	*find_command_path(char *cmd, char **env_paths)
{
	char	*full_path;
	int		i;

	if (strchr(cmd, '/')) // すでにパスが含まれる場合
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}
	if (!env_paths)
		return (NULL);
	i = 0;
	while (env_paths[i])
	{
		full_path = malloc(strlen(env_paths[i]) + 1 + strlen(cmd) + 1);
		if (!full_path)
			return (NULL);
		strcpy(full_path, env_paths[i]);
		strcat(full_path, "/");
		strcat(full_path, cmd);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

void	error_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void	free_split(char **arr)
{
	int i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}