/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:52:03 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/21 08:02:49 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

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
			break ;
		}
		envp++;
	}
	if (paths)
		all_paths = ft_split(paths, ':');
	return (all_paths);
}

char	*get_path(char **cmd, char **envp)
{
	int		i;
	char	**all_paths;
	char	*full_path;
	char	*result;

	//check_cmd(cmd, minishell, envp);
	if (!cmd[0])
		return (NULL);
	i = 0;
	//split error??
	all_paths = paths(envp);
	while (all_paths && all_paths[i])
	{
		full_path = ft_strjoin(all_paths[i++], "/");
		result = ft_strjoin(full_path, cmd[0]);
		free(full_path);
		if (access(result, X_OK) == 0)
		{
			free_2d_array((void ***)&all_paths);
			return (result);
		}
		free(result);
	}
	free_2d_array((void ***)&all_paths);
	if ((access(cmd[0], X_OK) == 0 && ft_strchr(cmd[0], '/')))
		return (cmd[0]);
	return (NULL);
	//free_2d_array((void ***)&envp);
	//exit(path_error(cmd, all_paths, minishell));
}
