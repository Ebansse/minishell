/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_lexer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 22:22:38 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 17:36:29 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	count_word_tokens(t_token *head)
{
	int	count;
	int	skip_next;

	count = 0;
	skip_next = 0;
	while (head)
	{
		if (head->type == T_PIPE)
			break ;
		if (head->type >= T_REDIR_IN && head->type <= T_HEREDOC)
			skip_next = 1;
		else if (head->type == T_WORD)
		{
			if (skip_next)
				skip_next = 0;
			else
				count++;
		}
		head = head->next;
	}
	return (count);
}

t_token	*new_token(char *value, t_token_type type)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = value;
	token->type = type;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **head, t_token *new)
{
	t_token	*tmp;

	if (!*head)
		*head = new;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new;
	}
}

char	*str_append_char(char *dest, char c)
{
	size_t	len;
	char	*new;

	len = 0;
	if (dest != NULL)
		len = ft_strlen(dest);
	new = malloc(len + 2);
	if (!new)
		return (NULL);
	if (dest)
		strcpy(new, dest);
	new[len] = c;
	new[len + 1] = '\0';
	free(dest);
	return (new);
}

t_token	*drop_token(t_token **head, t_token *prev, t_token *tok)
{
	t_token	*next;

	next = tok->next;
	if (prev)
		prev->next = next;
	else
		*head = next;
	free(tok->value);
	free(tok);
	return (next);
}
