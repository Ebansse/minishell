/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:28:32 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/23 16:11:24 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	open_redir_file(t_redir *redir)
{
	int	fd;

	fd = -1;
	if (redir->type == T_HEREDOC)
	{
		fd = redir->fd;
		redir->fd = -1;
		return (fd);
	}
	if (redir->type == T_REDIR_IN)
		fd = open(redir->filename, O_RDONLY);
	else if (redir->type == T_REDIR_OUT)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == T_REDIR_APPEND)
		fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(redir->filename);
	}
	return (fd);
}

void	apply_file_redirections(t_redir *redir)
{
	int	fd;

	while (redir)
	{
		fd = open_redir_file(redir);
		if (fd < 0)
			exit(1);
		if (redir->type == T_REDIR_IN || redir->type == T_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		redir = redir->next;
	}
}

void	restore_std(int save_in, int save_out)
{
	fflush(stdout);
	dup2(save_in, STDIN_FILENO);
	dup2(save_out, STDOUT_FILENO);
	close(save_in);
	close(save_out);
}

int	apply_redirections_builtin(t_redir *redir_list, int *save_in, int *save_out)
{
	int	fd;

	*save_in = dup(STDIN_FILENO);
	*save_out = dup(STDOUT_FILENO);
	while (redir_list)
	{
		fd = open_redir_file(redir_list);
		if (fd < 0)
			return (restore_std(*save_in, *save_out), 1);
		if (redir_list->type == T_REDIR_IN || redir_list->type == T_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		redir_list = redir_list->next;
	}
	return (0);
}
