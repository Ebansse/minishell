/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 15:06:45 by ebansse           #+#    #+#             */
/*   Updated: 2025/06/23 15:06:46 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*join_free(char *res, char *add)
{
	char	*new;

	if (!res || !add)
		return (res);
	new = ft_strjoin(res, add);
	free(res);
	return (new);
}

static char	*expand_key(char *res, char *key, t_env *env)
{
	if (!key)
		return (free(res), NULL);
	res = join_free(res, get_env_value(env, key));
	free(key);
	return (res);
}

static char	*expand_name(char *word, int *i, char *res, t_env *env)
{
	int	start;

	start = *i + 1;
	while (ft_isalnum(word[*i + 1]) || word[*i + 1] == '_')
		(*i)++;
	return (expand_key(res, substrdup(word, start, *i - start + 1), env));
}

static char	*expand_braces(char *word, int *i, char *res, t_env *env)
{
	int	start;
	int	end;

	start = *i + 2;
	end = start;
	while (ft_isalnum(word[end]) || word[end] == '_')
		end++;
	if (word[end] != '}' || end == start)
		return (str_append_char(res, '$'));
	*i = end;
	return (expand_key(res, substrdup(word, start, end - start), env));
}

/* Expands the $ at word[*i] into res; *i ends on the last consumed char. */
char	*expand_dollar(char *word, int *i, char *res, t_shell *shell)
{
	char	*status;
	char	next;

	next = word[*i + 1];
	if (next == '?')
	{
		status = ft_itoa(shell->last_status);
		res = join_free(res, status);
		free(status);
		(*i)++;
		return (res);
	}
	if (next == '{')
		return (expand_braces(word, i, res, shell->env));
	if (ft_isalpha(next) || next == '_')
		return (expand_name(word, i, res, shell->env));
	return (str_append_char(res, '$'));
}
