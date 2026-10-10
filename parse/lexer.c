/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cguinot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:47:47 by ebansse           #+#    #+#             */
/*   Updated: 2025/06/04 16:52:16 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	word_end(char *line, int i)
{
	char	quote;

	quote = 0;
	while (line[i] && (quote || (!ft_isspace(line[i]) && line[i] != '|'
				&& line[i] != '<' && line[i] != '>')))
	{
		if (!quote && (line[i] == 39 || line[i] == 34))
			quote = line[i];
		else if (quote && line[i] == quote)
			quote = 0;
		i++;
	}
	if (quote)
		return (-1);
	return (i);
}

int	tokenisation(t_token **head, char *line, int i)
{
	int	start;

	if (line[i] == '>' && line[i + 1] == '>')
		return (add_token(head, new_token(ft_strdup(">>"), T_REDIR_APPEND))
			, i + 2);
	else if (line[i] == '<' && line[i + 1] == '<')
		return (add_token(head, new_token(ft_strdup("<<"), T_HEREDOC))
			, i + 2);
	else if (line[i] == '|')
		return (add_token(head, new_token(ft_strdup("|"), T_PIPE)), i + 1);
	else if (line[i] == '<')
		return (add_token(head, new_token(ft_strdup("<"), T_REDIR_IN)), i + 1);
	else if (line[i] == '>')
		return (add_token(head, new_token(ft_strdup(">"), T_REDIR_OUT)), i + 1);
	start = i;
	i = word_end(line, i);
	if (i < 0)
		return (-1);
	add_token(head, new_token(substrdup(line, start, i - start), T_WORD));
	return (i);
}

t_token	*lexer(char *line, t_shell *shell)
{
	int		i;
	t_token	*head;

	i = 0;
	head = NULL;
	while (line[i])
	{
		if (ft_isspace(line[i]))
			i++;
		else
			i = tokenisation(&head, line, i);
		if (i < 0)
		{
			ft_putendl_fd("minishell: syntax error: unclosed quote", 2);
			shell->last_status = 2;
			free_tokens(head);
			free(line);
			return (NULL);
		}
	}
	free(line);
	return (head);
}
