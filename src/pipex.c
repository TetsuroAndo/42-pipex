/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:53:41 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 19:42:35 by teando           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "pipex.h"

/*
 * 全ての子プロセスの終了を待機する
 * 子プロセスが終了するまでwaitを繰り返し呼び出す
 */
void wait_children(void)
{
	int status;

	while (wait(&status) > 0)
		;
}

/*
 * コマンドを実行するための標準入出力のリダイレクトと実行を行う
 * @param cmd 実行するコマンド文字列
 * @param envp 環境変数の配列
 * @param in_fd 標準入力用のファイルディスクリプタ
 * @param out_fd 標準出力用のファイルディスクリプタ
 */
static void launch_command(char *cmd, char **envp, int in_fd, int out_fd)
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
 * 新しいプロセスをフォークしてコマンドを実行する
 * @param cmd 実行するコマンド文字列
 * @param envp 環境変数の配列
 * @param in_fd 標準入力用のファイルディスクリプタ
 * @param out_fd 標準出力用のファイルディスクリプタ
 */
static void fork_command(char *cmd, char **envp, int in_fd, int out_fd)
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
		error_exit("Fork error", 1);
	if (pid == 0)
		launch_command(cmd, envp, in_fd, out_fd);
	close(in_fd);
	close(out_fd);
}

/*
 * パイプラインの実行を制御する
 * 各コマンドをパイプで接続して順次実行する
 * @param px pipex構造体へのポインタ（コマンド情報を含む）
 * @param envp 環境変数の配列
 */
void execute_pipeline(t_pipex *px, char **envp)
{
	int i;
	int pipe_fd[2];
	int prev_fd;

	prev_fd = px->infile;
	i = 0;
	while (i < px->cmd_count - 1)
	{
		if (pipe(pipe_fd) == -1)
			error_exit("Pipe error", 1);
		fork_command(px->cmds[i], envp, prev_fd, pipe_fd[WRITE_END]);
		close(pipe_fd[WRITE_END]);
		prev_fd = pipe_fd[READ_END];
		i++;
	}
	fork_command(px->cmds[i], envp, prev_fd, px->outfile);
}

/*
 * メイン関数
 * コマンドライン引数を解析し、パイプラインの実行を開始する
 * @param ac コマンドライン引数の数
 * @param av コマンドライン引数の配列
 * @param envp 環境変数の配列
 * @return 0
 */
int main(int ac, char **av, char **envp)
{
	t_pipex px;

	if (ac < 5)
		error_exit("Invalid number of arguments.\n", 1);
	px.heredoc = is_here_doc(av[1]);
	if (px.heredoc)
	{
		unlink(av[ac - 1]);
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
	execute_pipeline(&px, envp);
	wait_children();
	return (0);
}
