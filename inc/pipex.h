/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:51:04 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 08:33:17 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* pipex.c */
int		main(int argc, char **argv, char **envp);

/* pipex_utils.c */
void	exit_with_error(char *msg, int code);
void	execute_cmd(char *cmd, char **envp);
void	close_pipes(int fd[2]);
void	child_proc_first(int fd[2], char **argv, char **envp, int infile);
void	child_proc_second(int fd[2], char **argv, char **envp, int outfile);

/* path_utils.c */
char	*get_cmd_path(char *cmd, char **envp);

#endif