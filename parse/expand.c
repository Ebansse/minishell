/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 12:00:00 by ebansse           #+#    #+#             */
/*   Updated: 2026/10/10 12:00:00 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	syntax_error(char *token, t_shell *shell)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(token, 2);
	ft_putendl_fd("'", 2);
	shell->last_status = 2;
	return (0);
}

int	check_syntax(t_token *tok, t_shell *shell)
{
	if (tok && tok->type == T_PIPE)
		return (syntax_error("|", shell));
	while (tok)
	{
		if (tok->type == T_PIPE && (!tok->next || tok->next->type == T_PIPE))
			return (syntax_error("|", shell));
		if (tok->type >= T_REDIR_IN && tok->type <= T_HEREDOC)
		{
			if (!tok->next)
				return (syntax_error("newline", shell));
			if (tok->next->type != T_WORD)
				return (syntax_error(tok->next->value, shell));
		}
		tok = tok->next;
	}
	return (1);
}

/* Removes quotes and expands variables everywhere except inside '...'. */
char	*expand_word(char *word, t_shell *shell)
{
	char	*res;
	char	quote;
	int		i;

	res = ft_strdup("");
	quote = 0;
	i = -1;
	while (res && word[++i])
	{
		if (!quote && (word[i] == 39 || word[i] == 34))
			quote = word[i];
		else if (quote && word[i] == quote)
			quote = 0;
		else if (word[i] == '$' && !quote
			&& (word[i + 1] == 39 || word[i + 1] == 34))
			continue ;
		else if (word[i] == '$' && quote != 39)
			res = expand_dollar(word, &i, res, shell);
		else
			res = str_append_char(res, word[i]);
	}
	return (res);
}

/* Frees value when the token has to be dropped. */
static int	keep_token(t_token *tok, t_token *prev, char *value)
{
	if (*value || ft_strchr(tok->value, 39) || ft_strchr(tok->value, 34)
		|| (prev && prev->type >= T_REDIR_IN))
		return (1);
	free(value);
	return (0);
}

/* Heredoc delimiters stay raw; unquoted words expanding to "" vanish. */
int	expand_tokens(t_token **head, t_shell *shell)
{
	t_token	*tok;
	t_token	*prev;
	char	*value;

	prev = NULL;
	tok = *head;
	while (tok)
	{
		if (tok->type == T_WORD && !(prev && prev->type == T_HEREDOC))
		{
			value = expand_word(tok->value, shell);
			if (!value)
				return (0);
			if (!keep_token(tok, prev, value))
			{
				tok = drop_token(head, prev, tok);
				continue ;
			}
			free(tok->value);
			tok->value = value;
		}
		prev = tok;
		tok = tok->next;
	}
	return (1);
}
