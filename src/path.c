/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:06:08 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 20:00:26 by teando           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "pipex.h"

/*
 * パスとコマンド名を結合する補助関数
 * @param p パス文字列
 * @param cmd コマンド名
 * @return 結合された完全なパス。メモリ確保に失敗した場合はNULL
 */
static char *join_path(const char *p, const char *cmd)
{
	char *tmp;
	char *joined;

	tmp = ft_strjoin(p, "/");
	if (!tmp)
		return (NULL);
	joined = ft_strjoin(tmp, cmd);
	free(tmp);
	return (joined);
}

/*
 * 指定されたコマンドが実行可能なパスを探索する
 * @param paths 検索対象のパスの配列
 * @param cmd 検索するコマンド名
 * @return コマンドの完全なパス。見つからない場合はNULL
 */
static char *check_path(char **paths, char *cmd)
{
	int i;
	char *fullpath;

	i = 0;
	while (paths && paths[i])
	{
		fullpath = join_path(paths[i], cmd);
		if (!fullpath)
			return (NULL);
		if (access(fullpath, X_OK) == 0)
			return (fullpath);
		free(fullpath);
		i++;
	}
	return (NULL);
}

/*
 * 環境変数PATHからコマンドの実行可能なフルパスを取得する
 * @param cmd コマンド名
 * @param envp 環境変数の配列
 * @return コマンドの完全なパス。見つからない場合はNULL
 */
char *get_cmd_path(char *cmd, char **envp)
{
	char **paths;
	char *res;
	int i;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	i = 0;
	while (envp && envp[i] && ft_strncmp(envp[i], "PATH=", 5))
		i++;
	if (!envp || !envp[i])
		return (NULL);
	paths = ft_split(envp[i] + 5, ':');
	res = check_path(paths, cmd);
	return (ft_free_split(paths), res);
}
