/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 09:07:14 by teando            #+#    #+#             */
/*   Updated: 2024/12/13 20:26:39 by teando           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "pipex.h"

void error_exit(char *msg, int code)
{
	perror(msg);
	exit(code);
}

void ft_free_split(char **split)
{
	size_t i;

	if (!split)
		return;
	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
}
