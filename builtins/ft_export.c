/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 14:21:26 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 19:29:52 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_valid_key(const char *key)
{
	int	i;

	if (!key || (!ft_isalpha(key[0]) && key[0] != '_'))
	{
		return (0);
	}
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
		{
			return (0);
		}
		i++;
	}
	return (1);
}

int	handle_invalid_key(char *key)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(key, 2);
	ft_putendl_fd("': not a valid identifier", 2);
	free(key);
	return (0);
}

int	handle_empty_value(char *arg, t_env **env)
{
	char	*key;

	key = ft_strdup(arg);
	if (!is_valid_key(key))
		return (handle_invalid_key(key));
	set_env_value(env, key, NULL);
	free(key);
	return (1);
}

int	process_assignment(char *arg, t_env **env)
{
	char	*equal;
	char	*key;
	char	*value;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (handle_empty_value(arg, env));
	key = ft_substr(arg, 0, equal - arg);
	if (!is_valid_key(key))
		return (handle_invalid_key(key));
	if (*(equal + 1) == '\0')
		value = ft_strdup("");
	else
		value = ft_strdup(equal + 1);
	set_env_value(env, key, value);
	free(key);
	free(value);
	return (1);
}

int	ft_export(char **args, t_env **env)
{
	int	i;
	int	result;

	if (!args[1])
		return (ft_export_display(*env));
	result = 0;
	i = 1;
	while (args[i])
	{
		if (!process_assignment(args[i], env))
			result = 1;
		i++;
	}
	return (result);
}
