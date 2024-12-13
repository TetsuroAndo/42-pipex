/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 09:32:39 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	wait_children(void)
{
	int	status;

	while (wait(&status) > 0)
		;
}

static void	child_process(char *cmd, char **envp, int in_fd, int out_fd)
{
	if (dup2(in_fd, STDIN_FILENO) < 0)
		error_exit("Dup error", 1);
	if (dup2(out_fd, STDOUT_FILENO) < 0)
		error_exit("Dup error", 1);
	close(in_fd);
	close(out_fd);
	execute_cmd(cmd, envp);
}

/*
** execute_pipeline:
** cmds: array of commands (argv部分)
** cmd_count: number of commands
** in_fd: file descriptor for input (最初の入力ソース)
** out_fd: file descriptor for final output (最終出力)
** envp: 環境変数
**
** パイプを作成しながらコマンドを一つずつ実行。
** cmd1 -> cmd2 -> ... -> cmdn
** 最初はin_fdをSTDINに、最後はout_fdをSTDOUTに設定。
** 中間コマンドは順次パイプでつなぐ。
*/
static void	execute_pipeline(char **cmds, int cmd_count, int in_fd, int out_fd,
		char **envp)
{
	int		i;
	int		pipe_fd[2];
	pid_t	pid;
	int		prev_fd;

	prev_fd = in_fd;
	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipe_fd) == -1)
			error_exit("Pipe error", 1);
		pid = fork();
		if (pid < 0)
			error_exit("Fork error", 1);
		if (pid == 0)
		{
			close(pipe_fd[READ_END]);
			if (dup2(prev_fd, STDIN_FILENO) < 0)
				error_exit("Dup error", 1);
			if (dup2(pipe_fd[WRITE_END], STDOUT_FILENO) < 0)
				error_exit("Dup error", 1);
			close(prev_fd);
			close(pipe_fd[WRITE_END]);
			execute_cmd(cmds[i], envp);
		}
		close(pipe_fd[WRITE_END]);
		close(prev_fd);
		prev_fd = pipe_fd[READ_END];
		i++;
	}
	pid = fork();
	if (pid < 0)
		error_exit("Fork error", 1);
	if (pid == 0)
		child_process(cmds[i], envp, prev_fd, out_fd);
	close(prev_fd);
}

int	main(int argc, char **argv, char **envp)
{
	int		infile;
	int		outfile;
	char	**cmds;
	int		cmd_count;
	int		heredoc;

	if (argc < 5)
		error_exit("Invalid number of arguments.\n", 1);
	heredoc = is_here_doc(argv[1]);
	if (heredoc)
	{
		if (argc < 6)
			error_exit("Invalid number of arguments for here_doc.\n", 1);
		infile = handle_here_doc(argv[2]);
		outfile = open_output_file_append(argv[argc - 1]);
		cmds = &argv[3];
		cmd_count = argc - 4;
	}
	else
	{
		infile = open_input_file(argv[1]);
		outfile = open_output_file(argv[argc - 1]);
		cmds = &argv[2];
		cmd_count = argc - 3;
	}
	execute_pipeline(cmds, cmd_count, infile, outfile, envp);
	wait_children();
	return (0);
}
