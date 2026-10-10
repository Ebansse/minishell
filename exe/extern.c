/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extern.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 16:43:55 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 17:58:57 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*find_command_path(char *cmd, t_env *env)
{
	char	*path_env;
	char	**paths;

	struct stat	st;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (stat(cmd, &st) == 0 && !S_ISDIR(st.st_mode)
			&& access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	else
	{
		path_env = get_env_value(env, "PATH");
		if (!path_env)
			return (NULL);
		paths = ft_split(path_env, ':');
		if (!paths)
			return (NULL);
		return (search_in_paths(cmd, paths));
	}
}

char	*search_in_paths(char *cmd, char **paths)
{
	int		i;
	char	*temp;
	char	*full_path;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
		{
			ft_free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_free_array(paths);
	return (NULL);
}

int	execute_external(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	pid = fork();
	if (pid < 0)
		return (perror("minishell: fork"), 1);
	if (pid == 0)
	{
		setup_signals(2);
		apply_file_redirections(cmd->redir);
		exec_external_child(cmd, shell);
	}
	waitpid(pid, &status, 0);
	return (wait_status(status));
}
