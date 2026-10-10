/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cguinot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 14:29:35 by ebansse           #+#    #+#             */
/*   Updated: 2025/06/04 18:17:59 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_redirection(t_cmd *cmd, int type, char *filename)
{
	t_redir	*tmp;
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return ;
	redir->type = type;
	redir->filename = ft_strdup(filename);
	redir->fd = -1;
	redir->next = NULL;
	if (!cmd->redir)
		cmd->redir = redir;
	else
	{
		tmp = cmd->redir;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = redir;
	}
}

char	**extract_word_tokens(t_token *tokens)
{
	char	**argv_parts;
	int		i;
	int		skip_next;

	argv_parts = malloc(sizeof(char *) * (count_word_tokens(tokens) + 1));
	if (!argv_parts)
		return (NULL);
	i = 0;
	skip_next = 0;
	while (tokens)
	{
		if (tokens->type == T_PIPE)
			break ;
		if (tokens->type >= T_REDIR_IN && tokens->type <= T_HEREDOC)
			skip_next = 1;
		else if (tokens->type == T_WORD && !skip_next)
			argv_parts[i++] = ft_strdup(tokens->value);
		else if (tokens->type == T_WORD)
			skip_next = 0;
		tokens = tokens->next;
	}
	return (argv_parts[i] = NULL, argv_parts);
}

void	extract_redir_token(t_token *tokens, t_cmd *curr_cmd)
{
	while (tokens && tokens->type != T_PIPE)
	{
		if (tokens->type >= T_REDIR_IN && tokens->type <= T_HEREDOC)
		{
			add_redirection(curr_cmd, tokens->type, tokens->next->value);
			tokens = tokens->next;
		}
		tokens = tokens->next;
	}
}

static t_cmd	*create_cmd(t_token *tokens)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->redir = NULL;
	cmd->next = NULL;
	cmd->argv = extract_word_tokens(tokens);
	if (!cmd->argv)
		return (free(cmd), NULL);
	extract_redir_token(tokens, cmd);
	return (cmd);
}

t_cmd	*parse_tokens(t_token *tokens)
{
	t_cmd	*cmd_list;
	t_cmd	*last_cmd;
	t_cmd	*curr_cmd;

	cmd_list = NULL;
	last_cmd = NULL;
	while (tokens)
	{
		curr_cmd = create_cmd(tokens);
		if (!curr_cmd)
			return (free_cmd(cmd_list), NULL);
		if (!cmd_list)
			cmd_list = curr_cmd;
		else
			last_cmd->next = curr_cmd;
		last_cmd = curr_cmd;
		while (tokens && tokens->type != T_PIPE)
			tokens = tokens->next;
		if (tokens && tokens->type == T_PIPE)
			tokens = tokens->next;
	}
	return (cmd_list);
}
