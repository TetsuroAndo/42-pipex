/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:06:40 by teando           ###   ########.fr       */
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

static void	parse_args(int argc, char **argv, t_pipex *px)
{
	if (argc < 5)
		error_exit("Invalid number of arguments.\n", 1);
	px->heredoc = is_here_doc(argv[1]);
	if (px->heredoc)
	{
		if (argc < 6)
			error_exit("Invalid number of arguments for here_doc.\n", 1);
		px->infile = handle_here_doc(argv[2]);
		px->outfile = open_output_file_append(argv[argc - 1]);
		px->cmds = &argv[3];
		px->cmd_count = argc - 4;
	}
	else
	{
		px->infile = open_input_file(argv[1]);
		px->outfile = open_output_file(argv[argc - 1]);
		px->cmds = &argv[2];
		px->cmd_count = argc - 3;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	px;

	parse_args(argc, argv, &px);
	execute_pipeline(px.cmds, px.cmd_count, px.infile, px.outfile, envp);
	wait_children();
	return (0);
}
