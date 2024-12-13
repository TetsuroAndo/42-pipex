/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:06:08 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:06:26 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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

char	*get_cmd_path(char *cmd, char **envp)
{
	int		i;
	char	**paths;
	char	*path_env;
	char	*res;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return ((access(cmd, X_OK) == 0) ? ft_strdup(cmd) : NULL);
	i = -1;
	path_env = NULL;
	while (envp && envp[++i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path_env = envp[i] + 5;
			break ;
		}
	}
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	res = check_path(paths, cmd);
	ft_free_split(paths);
	return (res);
}
