/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:06:21 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 20:14:45 by teando           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "pipex.h"

/*
 * コマンドを実行する
 * コマンド文字列をパースし、実行可能なパスを見つけて execve で実行する
 * @param cmd 実行するコマンド文字列（コマンドと引数を含む）
 * @param envp 環境変数の配列
 * 
 * エラー発生時は適切なエラーメッセージを表示して終了:
 * - コマンドのパースに失敗: 終了コード 127
 * - コマンドが見つからない: 終了コード 127
 * - execveの実行に失敗: 終了コード 127
 */
void execute_cmd(char *cmd, char **envp)
{
	char **args;
	char *path;

	args = ft_split(cmd, ' ');
	if (!args || !args[0])
	{
		if (args)
			ft_free_split(args);
		error_exit("Command parse error", 127);
	}
	path = get_cmd_path(args[0], envp);
	if (!path)
	{
		ft_free_split(args);
		error_exit("Command not found", 127);
	}
	if (execve(path, args, envp) == -1)
	{
		free(path);
		ft_free_split(args);
		error_exit("Execve error", 127);
	}
}
