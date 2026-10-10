/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:30:50 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 18:00:16 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_quit(int exit_code)
{
	exit(exit_code);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

int	run_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!cmd->argv[0])
		return (0);
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (ft_echo(cmd->argv));
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (ft_cd(cmd->argv, &shell->env));
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (ft_pwd());
	if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (ft_export(cmd->argv, &shell->env));
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (ft_unset(cmd->argv, &shell->env));
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (ft_env(shell->env));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		ft_exit(cmd->argv, shell->last_status);
	return (0);
}

/* Also used for commands made only of redirections (argv[0] == NULL). */
int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	save_in;
	int	save_out;
	int	result;

	if (apply_redirections_builtin(cmd->redir, &save_in, &save_out) != 0)
		return (1);
	result = run_builtin(cmd, shell);
	restore_std(save_in, save_out);
	return (result);
}

void	execute_commands(t_cmd *cmd, t_shell *shell)
{
	int	exit_status;

	if (!cmd || !cmd->argv)
		return ;
	if (cmd->next)
		exit_status = execute_pipeline(cmd, shell);
	else if (!cmd->argv[0] || is_builtin(cmd->argv[0]))
		exit_status = execute_builtin(cmd, shell);
	else
		exit_status = execute_external(cmd, shell);
	shell->last_status = exit_status;
}
