/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_ctl.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:10:16 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:19:49 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	is_here_doc(char *arg)
{
	if (!arg)
		return (0);
	if (ft_strncmp(arg, "here_doc", 9) == 0)
		return (1);
	return (0);
}

static void	write_heredoc_lines(int write_fd, char *limiter)
{
	char	*line;

	while (1)
	{
		line = get_next_line(STDIN_FILENO);
		if (!line || (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0
				&& line[ft_strlen(limiter)] == '\n'))
		{
			if (line)
				free(line);
			break ;
		}
		ft_putstr_fd(line, write_fd);
		free(line);
	}
}

int	handle_here_doc(char *limiter)
{
	int		pipe_fd[2];
	pid_t	pid;

	if (pipe(pipe_fd) == -1)
		error_exit("Pipe error", 1);
	pid = fork();
	if (pid < 0)
		error_exit("Fork error", 1);
	if (pid == 0)
	{
		close(pipe_fd[READ_END]);
		write_heredoc_lines(pipe_fd[WRITE_END], limiter);
		close(pipe_fd[WRITE_END]);
		exit(0);
	}
	close(pipe_fd[WRITE_END]);
	return (pipe_fd[READ_END]);
}
