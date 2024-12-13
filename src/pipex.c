/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:19:02 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	wait_children(void)
{
	int	status;

	while (wait(&status) > 0)
		;
}

static void	launch_command(char *cmd, char **envp, int in_fd, int out_fd)
{
	if (dup2(in_fd, STDIN_FILENO) < 0)
		error_exit("Dup error", 1);
	if (dup2(out_fd, STDOUT_FILENO) < 0)
		error_exit("Dup error", 1);
	close(in_fd);
	close(out_fd);
	execute_cmd(cmd, envp);
}

static void	fork_command(char *cmd, char **envp, int in_fd, int out_fd)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		error_exit("Fork error", 1);
	if (pid == 0)
		launch_command(cmd, envp, in_fd, out_fd);
	close(in_fd);
	close(out_fd);
}

void	execute_pipeline(char **cmds, int cmd_count, int in_fd, int out_fd,
		char **envp)
{
	int	i;
	int	pipe_fd[2];
	int	prev_fd;

	prev_fd = in_fd;
	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipe_fd) == -1)
			error_exit("Pipe error", 1);
		fork_command(cmds[i], envp, prev_fd, pipe_fd[WRITE_END]);
		close(pipe_fd[WRITE_END]);
		prev_fd = pipe_fd[READ_END];
		i++;
	}
	fork_command(cmds[i], envp, prev_fd, out_fd);
}

int	main(int ac, char **av, char **envp)
{
	t_pipex	px;

	if (ac < 5)
		error_exit("Invalid number of arguments.\n", 1);
	px.heredoc = is_here_doc(av[1]);
	if (px.heredoc)
	{
		if (ac < 6)
			error_exit("Invalid number of arguments for here_doc.\n", 1);
		px.infile = handle_here_doc(av[2]);
		px.outfile = open_output_file_append(av[ac - 1]);
		px.cmds = &av[3];
		px.cmd_count = ac - 4;
	}
	else
	{
		px.infile = open_input_file(av[1]);
		px.outfile = open_output_file(av[ac - 1]);
		px.cmds = &av[2];
		px.cmd_count = ac - 3;
	}
	execute_pipeline(px.cmds, px.cmd_count, px.infile, px.outfile, envp);
	wait_children();
	return (0);
}
