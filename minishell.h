/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cguinot <cesar.guinot66@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 18:30:52 by cguinot           #+#    #+#             */
/*   Updated: 2025/06/05 18:30:52 by cguinot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <sys/stat.h>

# define PROMPT_PRINT "\001\033[35m\002minishell $ \001\033[0m\002"
# define MEMORY_ERROR "Memory allocation error\n"
# define COMMAND_NOT_FOUND "Command not found\n"
# define PERMISSION_DENIED "Permission denied\n"
# define FILE_NOT_FOUND "No such file or directory\n"
# define INVALID_REDIR "Invalid redirection syntax\n"
# define HEREDOC_EOF "heredoc EOF reached\n"
# define HEREDOC_ERROR "Heredoc error\n"
# define PIPE_SYNTAX_ERROR "Syntax error near unexpected token `|'\n"
# define MAX_COMMANDS 100
# define MAX_PIPES 99
# define HEREDOC_TMP "/tmp/.minishell_heredoc"

extern volatile sig_atomic_t	g_signal_received;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_HEREDOC
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_redir
{
	int				type;
	char			*filename;
	int				fd;
	struct s_redir	*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redir;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_shell
{
	t_env	*env;
	int		last_status;
	int		running;
	char	*prompt;
	char	*terminal_prompt;
}	t_shell;

typedef struct s_pipe_data
{
	int		pipes[MAX_PIPES][2];
	pid_t	pids[MAX_COMMANDS];
	int		nb_commands;
	int		nb_pipes;
}	t_pipe_data;

/* Built-in commands */
int		ft_echo(char **argv);
int		ft_pwd(void);
int		ft_cd(char **args, t_env **env);
int		ft_env(t_env *env);
int		ft_export(char **args, t_env **env);
int		ft_export_display(t_env *env);
int		ft_unset(char **args, t_env **env);
void	ft_exit(char **args, int last_status);

/* Environment management */
t_env	*ft_copy_envp_to_list(char **envp, int i);
t_env	*ft_env_new(char *key, char *value);
void	ft_env_add_back(t_env **lst, t_env *new);
char	*get_env_value(t_env *env, char *key);
void	set_env_value(t_env **env, char *key, char *value);
char	**env_list_to_array(t_env *env);

/* Lexer and parser */
t_token	*lexer(char *line, t_shell *shell);
int		check_syntax(t_token *tok, t_shell *shell);
int		expand_tokens(t_token **head, t_shell *shell);
char	*expand_word(char *word, t_shell *shell);
char	*expand_dollar(char *word, int *i, char *res, t_shell *shell);
int		count_word_tokens(t_token *head);
t_token	*new_token(char *value, t_token_type type);
void	add_token(t_token **head, t_token *new);
t_token	*drop_token(t_token **head, t_token *prev, t_token *tok);
char	*str_append_char(char *dest, char c);
t_cmd	*parse_tokens(t_token *tokens);

/* Command execution */
void	execute_commands(t_cmd *cmd, t_shell *shell);
int		is_builtin(char *cmd);
int		run_builtin(t_cmd *cmd, t_shell *shell);
int		execute_builtin(t_cmd *cmd, t_shell *shell);
int		execute_external(t_cmd *cmd, t_shell *shell);
void	exec_external_child(t_cmd *cmd, t_shell *shell);
int		command_error(char *cmd, t_env *env);
int		wait_status(int status);
char	*find_command_path(char *cmd, t_env *env);
char	*search_in_paths(char *cmd, char **paths);

/* Pipeline execution */
int		execute_pipeline(t_cmd *cmd_list, t_shell *shell);
int		count_commands_in_list(t_cmd *cmd_list);
int		create_all_pipes(t_pipe_data *data);
void	close_all_pipes(t_pipe_data *data);
void	setup_redirections(t_pipe_data *data, int cmd_index);
void	exec_child_command(t_cmd *cmd, t_shell *shell,
			t_pipe_data *data, int cmd_index);
void	fork_all_commands(t_cmd *cmd_list, t_shell *shell, t_pipe_data *data);
int		wait_all_processes(t_pipe_data *data);

/* Heredoc management */
int		prepare_heredocs(t_cmd *cmd, t_shell *shell);

/* Redirections */
void	apply_file_redirections(t_redir *redir);
void	restore_std(int save_in, int save_out);
int		apply_redirections_builtin(t_redir *redir_list, int *save_in,
			int *save_out);

/* Signal handling */
void	handle_sigint_interactive(int sig);
void	handle_sigint_command(int sig);
void	handle_sigint_heredoc(int sig);
void	setup_signals(int mode);
void	check_and_handle_signals(t_shell *shell);

/* Utilities */
void	message(void);
bool	ft_isspace(char c);
char	*substrdup(const char *src, int start, int len);
void	print_token(t_token *lst);
void	free_tokens(t_token *head);
void	free_env(t_env *env);
void	ft_free_array(char **array);
void	free_cmd(t_cmd *cmd);
void	ft_quit(int exit_code);
void	prompt_loop(t_shell *shell);
void	print_cmd(t_cmd *cmd);

#endif