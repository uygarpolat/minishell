/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 12:00:43 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 19:22:29 by upolat           ###   ########.fr       */
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

static int	get_size(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
		i++;
	return (i);
}

static char	**create_new_envp(char **envp, char *str, int *flag)
{
	int		i;
	int		j;
	int		len;
	char	**new_envp;

	len = get_size(envp);
	new_envp = ft_calloc((size_t)len, sizeof(char *));
	if (!new_envp)
		return (NULL);
	i = -1;
	j = 0;
	while (envp[++i] && j < len)
	{
		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
		{
			*flag = 1;
			continue ;
		}
		new_envp[j] = ft_strdup(envp[i]);
		if (!new_envp[j])
			return (free_2d_array((void ***)&new_envp), NULL);
		j++;
	}
	new_envp[j] = NULL;
	return (new_envp);
}

char	**rm_envp(char **envp, char *str)
{
	char	**new_envp;
	int		flag;
	int		size;

	flag = 0;
	new_envp = create_new_envp(envp, str, &flag);
	if (!new_envp)
		return (NULL);
	size = get_size(new_envp);
	if (flag)
	{
		free_2d_array((void ***)&envp);
		return (new_envp);
	}
	free(new_envp[size - 1]);
	new_envp[size - 1] = NULL;
	free_2d_array((void ***)&new_envp);
	return (envp);
}

// char	**rm_envp(char **envp, char *str)
// {
// 	char	**new_envp;
// 	int		i;
// 	int		j;
// 	int		len;
// 	int		flag;

// 	len = 0;
// 	flag = 0;
// 	while (envp[len])
// 		len++;
// 	new_envp = ft_calloc((size_t) len, sizeof(char *));
// 	if (new_envp == NULL)
// 		return (NULL);
// 	i = -1;
// 	j = 0;
// 	while (envp[++i] && j < len)
// 	{
// 		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
// 		{
// 			flag = 1;
// 			continue ;
// 		}
// 		new_envp[j] = ft_strdup(envp[i]);
// 		if (!new_envp[j])
// 			return (free_2d_array((void ***)&new_envp), NULL);
// 		j++;
// 	}
// 	if (flag)
// 	{
// 		new_envp[j] = NULL;
// 		free_2d_array((void ***)&envp);
// 		return (new_envp);
// 	}
// 	free(new_envp[j - 1]);
// 	new_envp[j - 1] = NULL;
// 	free_2d_array((void ***)&new_envp);
// 	return (envp);
// }

char	**add_env(char **envp, char *str)
{
	char	**new_envp;
	int		i;

	if (!var_exists(envp, str))
	{
		new_envp = malloc(sizeof(char *) * (get_envp_size(envp) + 2));
		if (new_envp == NULL)
			return (NULL);
		i = -1;
		while (envp[++i])
		{
			new_envp[i] = ft_strdup(envp[i]);
			if (!new_envp[i])
				return (free_2d_array((void ***)&new_envp), NULL);
		}
		new_envp[i] = ft_strdup(str);
		if (!new_envp[i])
			return (free_2d_array((void ***)&envp), NULL);
		new_envp[++i] = NULL;
		free_2d_array((void ***)&envp);
		return (new_envp);
	}
	else
		return (ch_var(envp, str));
}
