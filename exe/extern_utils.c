/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extern_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 12:00:00 by ebansse           #+#    #+#             */
/*   Updated: 2026/10/10 12:00:00 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	path_error(char *cmd, char *msg, int code)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(msg, 2);
	return (code);
}

/* Called when find_command_path failed: picks bash's message and status. */
int	command_error(char *cmd, t_env *env)
{
	struct stat	st;

	if (!ft_strchr(cmd, '/') && (get_env_value(env, "PATH") || !*cmd))
		return (path_error(cmd, ": command not found", 127));
	if (stat(cmd, &st) != 0)
		return (path_error(cmd, ": No such file or directory", 127));
	if (S_ISDIR(st.st_mode))
		return (path_error(cmd, ": Is a directory", 126));
	return (path_error(cmd, ": Permission denied", 126));
}

/* Runs in a child whose redirections are already in place; never returns. */
void	exec_external_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	**env_array;
	int		err;

	path = find_command_path(cmd->argv[0], shell->env);
	if (!path)
		exit(command_error(cmd->argv[0], shell->env));
	env_array = env_list_to_array(shell->env);
	if (!env_array)
		exit(1);
	execve(path, cmd->argv, env_array);
	err = errno;
	ft_putstr_fd("minishell: ", 2);
	perror(cmd->argv[0]);
	if (err == EACCES || err == ENOEXEC)
		exit(126);
	exit(127);
}

int	wait_status(int status)
{
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
