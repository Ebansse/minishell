/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 12:00:00 by ebansse           #+#    #+#             */
/*   Updated: 2026/10/10 12:00:00 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*remove_quotes(char *s, int *quoted)
{
	char	*res;
	char	quote;
	int		i;

	*quoted = (ft_strchr(s, 39) || ft_strchr(s, 34));
	res = ft_strdup("");
	quote = 0;
	i = -1;
	while (res && s[++i])
	{
		if (!quote && (s[i] == 39 || s[i] == 34))
			quote = s[i];
		else if (quote && s[i] == quote)
			quote = 0;
		else
			res = str_append_char(res, s[i]);
	}
	return (res);
}

static char	*expand_heredoc_line(char *line, t_shell *shell)
{
	char	*res;
	int		i;

	res = ft_strdup("");
	i = -1;
	while (res && line[++i])
	{
		if (line[i] == '$')
			res = expand_dollar(line, &i, res, shell);
		else
			res = str_append_char(res, line[i]);
	}
	free(line);
	return (res);
}

static void	heredoc_child(char *delim, int quoted, t_shell *shell)
{
	int		fd;
	char	*line;

	setup_signals(2);
	fd = open(HEREDOC_TMP, O_CREAT | O_WRONLY | O_TRUNC, 0600);
	if (fd < 0)
		exit(1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delim) == 0)
			break ;
		if (!quoted)
			line = expand_heredoc_line(line, shell);
		ft_putendl_fd(line, fd);
		free(line);
	}
	if (!line)
	{
		ft_putstr_fd("minishell: warning: here-document delimited by "
			"end-of-file (wanted `", 2);
		ft_putstr_fd(delim, 2);
		ft_putendl_fd("')", 2);
	}
	exit((free(line), close(fd), 0));
}

static int	read_heredoc(t_redir *redir, t_shell *shell)
{
	char	*delim;
	int		quoted;
	pid_t	pid;
	int		status;

	delim = remove_quotes(redir->filename, &quoted);
	if (!delim)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (free(delim), perror("minishell: fork"), -1);
	if (pid == 0)
		heredoc_child(delim, quoted, shell);
	free(delim);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) || WEXITSTATUS(status) != 0)
	{
		unlink(HEREDOC_TMP);
		shell->last_status = wait_status(status);
		return (-1);
	}
	redir->fd = open(HEREDOC_TMP, O_RDONLY);
	unlink(HEREDOC_TMP);
	return (redir->fd);
}

/* Reads every heredoc of the command line before anything is executed. */
int	prepare_heredocs(t_cmd *cmd, t_shell *shell)
{
	t_redir	*redir;

	while (cmd)
	{
		redir = cmd->redir;
		while (redir)
		{
			if (redir->type == T_HEREDOC && read_heredoc(redir, shell) < 0)
				return (-1);
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}
