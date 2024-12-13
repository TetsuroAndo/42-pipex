/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 20:51:04 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 09:25:11 by teando           ###   ########.fr       */
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

# define READ_END 0
# define WRITE_END 1

int		main(int argc, char **argv, char **envp);

/* pipex_utils.c */
void	error_exit(char *msg, int code);
void	close_pipes(int *fd);
void	ft_free_split(char **split);

/* execute.c */
void	execute_cmd(char *cmd, char **envp);

/* path_utils.c */
char	*get_cmd_path(char *cmd, char **envp);

/* file_ctl.c */
int		open_input_file(char *filename);
int		open_output_file(char *filename);
int		open_output_file_append(char *filename);
int		is_here_doc(char *arg);
int		handle_here_doc(char *limiter);

#endif