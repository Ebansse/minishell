/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 19:36:31 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/23 15:06:34 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_child_command(t_cmd *cmd, t_shell *shell,
						t_pipe_data *data, int cmd_index)
{
	setup_signals(2);
	setup_redirections(data, cmd_index);
	apply_file_redirections(cmd->redir);
	close_all_pipes(data);
	if (!cmd->argv[0] || is_builtin(cmd->argv[0]))
		exit(run_builtin(cmd, shell));
	exec_external_child(cmd, shell);
}

void	fork_all_commands(t_cmd *cmd_list, t_shell *shell, t_pipe_data *data)
{
	t_cmd	*current;
	int		cmd_index;

	current = cmd_list;
	cmd_index = 0;
	while (current)
	{
		data->pids[cmd_index] = fork();
		if (data->pids[cmd_index] == 0)
		{
			exec_child_command(current, shell, data, cmd_index);
		}
		else if (data->pids[cmd_index] == -1)
		{
			perror("minishell: fork");
			return ;
		}
		current = current->next;
		cmd_index++;
	}
}

int	wait_all_processes(t_pipe_data *data)
{
	int	i;
	int	status;
	int	last_status;

	last_status = 0;
	i = 0;
	while (i < data->nb_commands)
	{
		waitpid(data->pids[i], &status, 0);
		if (i == data->nb_commands - 1)
			last_status = wait_status(status);
		i++;
	}
	return (last_status);
}

int	execute_pipeline(t_cmd *cmd_list, t_shell *shell)
{
	t_pipe_data	data;
	int			exit_status;
	int			save_stdin;
	int			save_stdout;

	save_stdin = dup(STDIN_FILENO);
	save_stdout = dup(STDOUT_FILENO);
	data.nb_commands = count_commands_in_list(cmd_list);
	if (create_all_pipes(&data) == -1)
		return (1);
	fork_all_commands(cmd_list, shell, &data);
	close_all_pipes(&data);
	exit_status = wait_all_processes(&data);
	dup2(save_stdin, STDIN_FILENO);
	dup2(save_stdout, STDOUT_FILENO);
	close(save_stdin);
	close(save_stdout);
	return (exit_status);
}
