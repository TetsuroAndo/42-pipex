/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_open.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:19:43 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 11:20:00 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	open_input_file(char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		error_exit("Input file error", 1);
	return (fd);
}

int	open_output_file(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error_exit("Output file error", 1);
	return (fd);
}

int	open_output_file_append(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		error_exit("Output file error", 1);
	return (fd);
}
