/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:06:21 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:05:54 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static void	handle_execve_error(char *path, char **args)
{
	free(path);
	ft_free_split(args);
	error_exit("Execve error", 127);
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
		error_exit("Command parse error", 127);
	}
	path = get_cmd_path(args[0], envp);
	if (!path)
	{
		ft_free_split(args);
		error_exit("Command not found", 127);
	}
	if (execve(path, args, envp) == -1)
		handle_execve_error(path, args);
}
