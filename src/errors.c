void	free_ast(t_ast	*ast)
{
}

int	error(t_minishell *minishell, t_command *cmd)
{
	if (minishell->ast)
		free_ast(minishell->ast);
	if (minishell->p.pipes)
	{
		i = -1;
		while (minishell->p.pipes[++i])
	}
	//free minishell->p.pipes
	//free minishell->p.pids
	//free minishell->pwd
	//free cmd->args
	//free cmd->path
}
