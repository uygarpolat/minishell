/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/02 00:36:44 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
#include "../includes/signals.h"

void	dupping(t_minishell *minishell, t_pipes *p, t_put *cmd, int n)
{
	int	in;
	int	out;

	in = -1;
	out = -1;
	if (!cmd->infile && p->count > 0)
		in = p->pipes[p->i][0];
	else if (cmd->infile)
		in = cmd->in;
	if (!cmd->outfile && p->count > 0)
		out = p->pipes[p->o][1];
	else if (cmd->outfile)
		out = cmd->out;
	if (n < minishell->p.count || cmd->outfile)
	{
		if (dup2(out, STDOUT_FILENO) == -1)
			error2(minishell, "dup2 error\n", cmd);
	}
	if (n > 0 || cmd->infile)
	{
		if (dup2(in, STDIN_FILENO) == -1)
			error2(minishell, "dup2 error\n", cmd);
	}
	if (cmd->infile)
		close(cmd->in);
	if (cmd->outfile)
		close(cmd->out);
	if (in >= 0)
		close(in);
	if (out >= 0)
		close(out);
}

int	open_files(t_put *cmd)
{	
	if (cmd->infile)
	{
		if (cmd->in > 0)
			close(cmd->in);
		cmd->in = open(cmd->infile, O_RDONLY);
		if (cmd->in < 0)
		{
			ft_putchar_fd(' ', 2);
			ft_putstr_fd(strerror(errno), 2);
			ft_putchar_fd('\n', 2);
			return (0);
		}
	}
	if (cmd->outfile)
	{
		if (cmd->out > 0)
			close(cmd->out);
		if (cmd->o_type == 'o')
			cmd->out = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
			cmd->out = open(cmd->outfile, O_APPEND | O_CREAT | O_WRONLY, 0644);
		if (cmd->out < 0)
		{
			ft_putchar_fd(' ', 2);
			ft_putstr_fd(strerror(errno), 2);
			ft_putchar_fd('\n', 2);
			return (0);
		}
	}
	return (1);
}

int	here(t_tokens *token, t_ast *ast)
{
	int	fd;
	int	len;
	char	*buf;

	len = ft_strlen(token->value);
	fd = open(".heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	set_signals(ast->code_parser, SIGNAL_HEREDOC); // Note from Uygar: Signals entering heredoc mode
	while (1)
	{
		//write(1, "heredoc> ", 9);
		//buf = get_next_line(0);
		buf = readline("> ");
		if (*ast->code_parser == 130) // Note from Uygar: This is checking for ctrl+C
			break ;
		if (!buf)
			break ;
		if (buf < 0)
			return (0);
		if (!ft_strncmp(token->value, buf, len + 1))
			break ;
		//if (!ft_strncmp(token->value, buf, len) && buf[len] == '\n')
		//	break ;
		write(fd, buf, ft_strlen(buf));
		write(fd, "\n", 1);
		free(buf);
	}
	set_signals(ast->code_parser, SIGNAL_PARENT); // Note from Uygar: Signals entering parent process mode
	if (buf)
		free(buf);
	close(fd);
	return (1);
}

void	check_here(t_ast *s)
{
	t_ast	*ast;
	t_ast	*temp;

	ast = s;
	while (ast)
	{
		if (ast->type == AST_COMMAND)
		{
			//check if redirect
			temp = ast->redir_target;
			while (temp)
			{
				if (temp->type == AST_HEREDOC)
					here(temp->token, s);
				temp = temp->redir_target; 
			}
		}
		else if (ast->type == AST_AND || ast->type == AST_OR)
		{
			//check first the left side and then the right side and then move to the right of the right
			temp = ast->left->redir_target;
			while (temp)
			{
				if (temp->type == AST_HEREDOC)
					here(temp->token, s);
				temp = temp->redir_target;
			}
			if (ast->right->type == AST_COMMAND)
				temp = ast->right->redir_target;
			else
				temp = ast->right->left->redir_target;
			while (temp)
			{
				if (temp->type == AST_HEREDOC)
					here(temp->token, s);
				temp = temp->redir_target;
			}
			ast = ast->right;
		}
		else if (ast->type == AST_PIPE)
		{
			//check the left side
			temp = ast->left->redir_target;
			while (temp)
			{
				if (temp->type == AST_HEREDOC)
					here(temp->token, s);
				temp = temp->redir_target;
			}
		}
		ast = ast->right;
	}
}

void	get_in_out(t_ast *s, t_put *cmd, t_minishell *minishell)
{
	t_ast	*temp;

	temp = s->redir_target;
	while (temp)
	{
		if (temp->type == AST_REDIR_IN)
			cmd->infile = temp->token->value;
		else if (temp->type == AST_REDIR_OUT)
		{
			cmd->o_type = 'o';
			cmd->outfile = temp->token->value;
		}
		else if (temp->type == AST_REDIR_APPEND)
		{
			cmd->o_type = 'a';
			cmd->outfile = temp->token->value;
		}
		else if (temp->type == AST_HEREDOC)
		{
			//if (access(".heredoc", F_OK) == 0)
				//unlink(".heredoc");
			//here(temp->token);
			cmd->infile = ".heredoc";
		}
		if (!open_files(cmd))
		{
			error(minishell, cmd);
			exit (1);
		}
		temp = temp->redir_target;
	}
}

void	execute(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *cmd)
{
	int	i;
	char	*path;
	struct stat	buf;

	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] == 0)
	{
		set_signals(s->code_parser, SIGNAL_CHILD); // Added by Uygar. Signals entering child process mode.
		get_in_out(s, cmd, minishell);
		if (minishell->p.pipes || cmd->infile || cmd->outfile)
		{
			if (n == minishell->p.count)
				minishell->p.o = minishell->p.count - 1;
			dupping(minishell, &minishell->p, cmd, n);
		}
		i = 0;
		while (i < minishell->p.count)
		{
			close(minishell->p.pipes[i][0]);
			close(minishell->p.pipes[i][1]);
			i++;
		}
		if (is_builtin(s->words))
		{
			if (!execute_builtin(s, s->words, envp, minishell, n, cmd))
			{
				error(minishell, cmd);
				exit(1);
			}
			exit(0);
		}
		if (!*s->words)
			exit(0);
		path = get_path(s->words, *envp);
		if (!path)
		{
			if (!ft_strchr(s->words[0], '/'))
			{
				ft_putstr_fd(" command not found\n", 2);
				exit(127);
			}
			else if (stat(s->words[0], &buf) ==0 && access(s->words[0], X_OK) != 0)
			{
				ft_putstr_fd(" Permission denied\n", 2);
				exit(126);
			}
			else if (ft_strchr(s->words[0], '/'))
			{	
				ft_putstr_fd(" No such file or directory\n", 2);
				exit(127);
			}
		}
		if (stat(path, &buf) == 0)
		{
			if (S_ISDIR(buf.st_mode))
			{
				ft_putstr_fd(" Is a directory\n", 2);
				exit (126);
			}
			if (access(path, X_OK) != 0)
			{
				ft_putstr_fd(" Permission denied\n", 2);
				exit(127);
			}
		}
		
		/*
		Note from Uygar regarding heredoc:
		The following 2 lines are added by Uygar.
		*s->code_parser=130 happens if ctrl+C comes during heredoc.
		If there is a better place for these 2 lines, they can be moved.
		I chose before execve because I didn't know where else they could be.
		My understanding is nothing should go into execve if ctrl+C happens during heredoc.
		You can check against *s->code_parser to implement this elsewhere.
		Currently there are some problems:
		1) If heredoc input is "cat << END" and ctrl+C happens, echo $? returns 1,
		which mimicks Bash. But if heredoc input is "<< END", echo $? returns 0, which is wrong.
		Why does this happen?
		2) One more new line than bash is displayed when ctrl+C happens in heredoc mode.
		3) When ctrl+C comes, .heredoc file is not displayed on the screen, but what
		happens to .heredoc? We should make sure we are deleting and unlinking .heredoc
		after ctrl+C (I didn't do it).
		
		IMPORTANT: One very important assumption in my heredoc signal implementation is
		that heredoc happens in the parent process. If it is happening in a child process,
		we should talk. My understanding is that it must happen in parent process.
		*/
		if (*s->code_parser == 130) // Added by Uygar
			exit(1); // Added by Uygar
		execve(path, s->words, *envp);
		ft_putstr_fd(strerror(errno), 2);
		error(minishell, cmd);
		exit(errno);
	}
}

int	count_pipes(t_ast *s)
{
	t_ast	*i;
	int	count;

	i = s;
	count = 0;
	while (i)
	{
		if (i->type == AST_PIPE)
			count++;
		i = i->right;
	}
	return (count);
}

int	count_operators(t_ast *s)
{
	t_ast	*i;
	int	count;

	i = s;
	count = 0;
	while (i)
	{
		if (i->type == AST_AND || i->type == AST_OR)
			count++;
		i = i->right;
	}
	return (count);
}

int	pipeing(t_pipes *p)
{
	int	i;
	//int	j;

	p->i = 0;
	p->o = 0;
	i = -1;
	while (++i < p->count)
	{
		if (pipe(p->pipes[i]) < 0)
		{
			//j = 0;
			//while (j < i)
			//{
				//close(p->pipes[i][0]);
				//close(p->pipes[i][1]);
				//j++;
			//}
			return (0);
		}
	}
	return (1);
}

int	mallocing(t_pipes *p)
{
	int	i;
	//HANDLe MALLOC ERROR
	p->pipes = NULL;
	p->pids = NULL;
	if (p->count > 0)
	{
		p->pipes = malloc(sizeof(int *) * p->count);
		if (!p->pipes)
			return (0);
		i = -1;
		while (++i < p->count)
		{
			p->pipes[i] = malloc(sizeof(int) * 2);
			if (!p->pipes[i])
				return (0);
		}
		p->pids = malloc(sizeof(int) * (p->count + 1));
		if (!p->pids)
			return (0);
	}
	else
	{
		p->pids = malloc(sizeof(int) * (p->o_count + 1));
		if (!p->pids)
			return (0);
	}
	return (1);
}

void	close_and_free(t_pipes *p, t_put *cmd)
{
	int	i;

	i = 0;
	if (p->pipes)
	{
		while (i < p->count)
		{
			close(p->pipes[i][0]);
			close(p->pipes[i][1]);
			free(p->pipes[i]);
			i++;
		}
		free(p->pipes);
	}
	if (cmd->infile && cmd->in >= 0)
		close(cmd->in);
	if (cmd->outfile && cmd->out >= 0)
		close(cmd->out);
}

int	waiting(int pid)
{
	int	status;
	int	code;
	
	status = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		code = WEXITSTATUS(status);
	else
		code = EXIT_FAILURE;
	return (code);
}
/*
void	in_out(t_pipes *p, t_ast *i)
{
	if (i->token.type == TOKEN_REDIR_OUT)
	{
		p->out = i->right;
	}
	else if (i->token.type == TOKEN_REDIR_IN)
	{
		p->in = i->right;
	}
	if (i->token.type == TOKEN_PIPE)
	{
		if (
	}
}
*/

int	execution(t_ast *s, char ***envp)
{
	t_minishell	minishell;
	int	n;
	int	j;
	int status;
	t_put	cmd;

	cmd.infile = NULL;
	cmd.outfile = NULL;
	cmd.in = -1;
	cmd.out = -1;
	cmd.stdin2 = -1;
	cmd.stdout2 = -1;
	minishell.ast = s;
	getcwd(minishell.pwd, sizeof(minishell.pwd));
	minishell.p.count = count_pipes(minishell.ast);
	minishell.p.o_count = count_operators(minishell.ast);
	if (!mallocing(&minishell.p) || !pipeing(&minishell.p))
	{
		error2(&minishell, "malloc failed\n", &cmd);
		return (1);
	}
	n = 0;
	if (minishell.p.count == 0 && is_builtin(s->words))
	{
			if (!execute_builtin(s, s->words, envp, &minishell, n, &cmd))
			{	
				error(&minishell, &cmd);
				return (1);
			}
			if (cmd.infile)
			{
				close(STDIN_FILENO);
				dup2(cmd.stdin2, STDIN_FILENO);
				close(cmd.stdin2);
			}
			if (cmd.outfile)
			{
				close(STDOUT_FILENO);
				dup2(cmd.stdout2, STDOUT_FILENO);
				close(cmd.stdout2);
			}
			if (access(".heredoc", F_OK) == 0)
				unlink(".heredoc");
			if (minishell.p.pids)
				free(minishell.p.pids);
			close_and_free(&minishell.p, &cmd);
			return (0);
	}
	else
	{
		//go throug the whole tree and check the heredocs
		check_here(minishell.ast);
		while (minishell.ast)
		{
			if (minishell.ast->type == AST_PIPE)
				execute(minishell.ast->left, envp, &minishell, n, &cmd);
			else if (minishell.ast->type == AST_COMMAND)
				execute(minishell.ast, envp, &minishell, n, &cmd);
			else if (minishell.ast->type == AST_AND)
			{	
				execute(minishell.ast->left, envp, &minishell, n, &cmd);
				while (minishell.ast->type == AST_AND)
				{
					status = 0;
					waitpid(minishell.p.pids[n], &status, 0);
					if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
					{
						if (minishell.ast->right->type == AST_COMMAND)
							execute(minishell.ast->right, envp, &minishell, ++n, &cmd);
						else
							execute(minishell.ast->right->left, envp, &minishell, ++n, &cmd);
					}
					else
					{
						while (minishell.ast->type == AST_AND)
							minishell.ast = minishell.ast->right;
						break ;
					}
					minishell.ast = minishell.ast->right;
				}
				//minishell.ast = minishell.ast->right;
				//run left side, if it succeeds run right side
			}
			else if (minishell.ast->type == AST_OR)
			{
				//if left side fails run right side, if left succeeds don't run right
				execute(minishell.ast->left, envp, &minishell, n, &cmd);
				while (minishell.ast->type == AST_OR)
				{
					status = 0;
					waitpid(minishell.p.pids[n], &status, 0);
					if (!WIFEXITED(status))
					{
						if (minishell.ast->right->type == AST_COMMAND)
							execute(minishell.ast->right, envp, &minishell, ++n, &cmd);
						else
							execute(minishell.ast->right->left, envp, &minishell, ++n, &cmd);
					}
					minishell.ast = minishell.ast->right;
				}
			}
			if (n > 0)
				minishell.p.i++;
			minishell.p.o++;
			n++;
			minishell.ast = minishell.ast->right;
		}
	}
	close_and_free(&minishell.p, &cmd);
	j = 0;
	while (j <= (minishell.p.count + minishell.p.o_count))
		s->code = waiting(minishell.p.pids[j++]);
	//i = -1;
	//free(minishell.pwd);
	if (minishell.p.pids)
		free(minishell.p.pids);
	if (access(".heredoc", F_OK) == 0)
		unlink(".heredoc");
	return (s->code);
}
