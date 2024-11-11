/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:52:03 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/11 13:27:12 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"


static int	path_error(char **cmd, char **all_paths, t_minishell *minishell)
{
	if (all_paths && cmd[0][0] != '/')
		error2(minishell, "command not found\n");
	else
		error2(minishell, "No such file or directory\n");
	free_2d_array((void ***)&all_paths);
	return (127);
}

static void	check_cmd(char **cmd, t_minishell *minishell, char **envp)
{
	if (!cmd)
	{
		free_2d_array((void ***)&envp);
		error2(minishell, "command not found\n");
		exit (127);
	}
	if (cmd[0][0] == '.')
	{
		if (cmd[0][ft_strlen(cmd[0]) - 1] == '/')
		{
			free_2d_array((void ***)&envp);
			error2(minishell, "Is a directroy\n");
			exit (126);
		}
		else if (access(cmd[0], X_OK) == -1)
		{
			free_2d_array((void ***)&envp);
			error2(minishell, "command not found\n");
			exit (126);
		}
	}
}

char	**paths(char **envp)
{
	char	*paths;
	char	**all_paths;

	paths = NULL;
	all_paths = NULL;
	while (*envp)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
		{
			paths = *envp + 5;
			break;
		}
		envp++;
	}
	all_paths = ft_split(paths, ':');
	return (all_paths);
}

char	*get_path(char **cmd, char **envp, t_minishell *minishell)
{
	int	i;
	char	**all_paths;
	char	*full_path;
	char	*result;

	check_cmd(cmd, minishell, envp);
	i = 0;
	//split error??
	all_paths = paths(envp);
	while (all_paths && all_paths[i])
	{
		full_path = ft_strjoin(all_paths[i++], "/");
		result = ft_strjoin(full_path, cmd[0]);
		free(full_path);
		if (access(result, X_OK) == 0)
			return (result);
		free(result);
	}
	if (access(cmd[0], X_OK) == 0 && ft_strchr(cmd[0], '/'))
		return (cmd[0]);
	free_2d_array((void ***)&envp);
	exit(path_error(cmd, all_paths, minishell));
}
