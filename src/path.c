/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:52:03 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/24 11:45:05 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"

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

char	*get_path(char **cmd, char **envp)
{
	int	i;
	char	**all_paths;
	char	*full_path;
	char	*result;

	i = 0;
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
	//error
	return (NULL);
}


