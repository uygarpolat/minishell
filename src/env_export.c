/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:54:44 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/29 19:36:55 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

char	**ch_envp(char **envp, char *new_dir)
{
	char	*dir;
	char	**new_envp;
	char	**tmp;

	tmp = envp;
	while (*envp)
	{
		if (!ft_strncmp(*envp, "PWD=", 4))
		{
			dir = *envp + 4;
			break ;
		}
		envp++;
	}
	dir = ft_strjoin(dir, "/");
	dir = ft_strjoin_free(dir, new_dir);
	if (!dir)
		return (NULL);
	new_envp = ft_strdup3(tmp, dir);
	if (!new_envp)
		return (NULL);
	if (envp)
		free_2d_array((void ***)&tmp);
	free(dir);
	return (new_envp);
}

char	**rm_envp(char **envp, char *str)
{
	char	**new_envp;
	int		i;
	int		j;
	int		flag;

	i = 0;
	flag = 0;
	while (envp[i])
		i++;
	new_envp = ft_calloc((size_t) i, sizeof(char *));
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	j = 0;
	while (envp[++i])
	{
		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
		{
			flag = 1;
			continue ;
		}
		if (new_envp[j])
			new_envp[j] = ft_strdup(envp[i]);
		if (!new_envp[j])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
	}
	if (flag)
	{
		new_envp[i - 1] = NULL;
		free_2d_array((void ***)&envp);
		return (new_envp);
	}
	free_2d_array((void ***)&new_envp);
	return (envp);
}

char	**add_env(char **envp, char *str)
{
	char	**new_envp;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	while (envp[++i])
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
	}
	new_envp[i] = ft_strdup(str);
	new_envp[++i] = NULL;
	free_2d_array((void ***)&envp);
	return (new_envp);
}

void	print_env(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
		printf("%s\n", envp[i]);
}

int	run_export(char **cmd, char ***envp)
{
	int		i;
	char	**str;

	// Note from Uygar: I added this check for when the only input is "export",
	// otherwise it was seg faulting.
	// Currently envp and export both print the same list, that should be fixed.
	int j = 0;
	while(cmd[j])
		j++;
	if (j == 1)
		return (print_env(*envp), 1);
	
	if (cmd[1][0] != '_' && !ft_isalpha(cmd[1][0]))
	{
		ft_putstr_fd(" not a valid identifier\n", 2);
		return (0);
	}
	str = ft_split(cmd[1], '=');
	if (!str)
	{
		ft_putstr_fd(" not a valid identifier\n", 2);
		return (0);
	}
	i = -1;
	while (str[0][++i])
	{
		if (str[0][i] != '_' && !ft_isalpha(str[0][i]) && \
				!ft_isdigit(str[0][i]))
		{
			ft_putstr_fd(" not a valid identifier\n", 2);
			return (0);
		}
	}
	*envp = add_env(*envp, cmd[1]);
	if (!envp)
		return (0);
	return (1);
}
