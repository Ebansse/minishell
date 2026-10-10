/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:12:11 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 16:16:46 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_n_flag(char *arg)
{
	int	i;

	if (arg[0] != '-' || arg[1] != 'n')
		return (0);
	i = 1;
	while (arg[i] == 'n')
		i++;
	return (arg[i] == '\0');
}

int	ft_echo(char **argv)
{
	int	i;
	int	new_line;

	new_line = 1;
	i = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		new_line = 0;
		i++;
	}
	while (argv[i])
	{
		if (argv[i + 1])
			printf("%s ", argv[i]);
		else
			printf("%s", argv[i]);
		i++;
	}
	if (new_line == 1)
		printf("\n");
	return (0);
}
