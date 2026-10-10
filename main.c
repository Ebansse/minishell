/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebansse <ebansse@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 13:15:41 by ebansse           #+#    #+#             */
/*   Updated: 2025/05/22 16:51:30 by ebansse          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_line(char *line, t_shell *shell)
{
	t_cmd	*cmd;
	t_token	*lst;

	lst = lexer(line, shell);
	if (!lst)
		return (0);
	if (!check_syntax(lst, shell) || !expand_tokens(&lst, shell))
		return (free_tokens(lst), 0);
	cmd = parse_tokens(lst);
	free_tokens(lst);
	setup_signals(1);
	if (cmd && prepare_heredocs(cmd, shell) == 0)
		execute_commands(cmd, shell);
	free_cmd(cmd);
	return (1);
}

void	prompt_loop(t_shell *shell)
{
	char	*line;

	message();
	while (1)
	{
		setup_signals(0);
		if (!isatty(STDIN_FILENO))
			dup2(open("/dev/tty", O_RDONLY), STDIN_FILENO);
		line = readline(PROMPT_PRINT);
		if (!line)
		{
			write(1, "exit\n", 5);
			break ;
		}
		check_and_handle_signals(shell);
		if (line && *line && ft_strlen(line) > 0)
		{
			add_history(line);
			process_line(ft_strdup(line), shell);
		}
		free(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;
	t_env	*env;

	(void)argc;
	(void)argv;
	shell.last_status = 0;
	shell.running = 1;
	env = ft_copy_envp_to_list(envp, 0);
	shell.env = env;
	if (!env)
	{
		ft_putendl_fd("env copy failed", 2);
		return (1);
	}
	setup_signals(0);
	prompt_loop(&shell);
	free_env(env);
	rl_clear_history();
	return (0);
}
