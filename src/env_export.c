/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:54:44 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/21 11:37:14 by upolat           ###   ########.fr       */
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
	int		len;
	int		flag;

	len = 0;
	flag = 0;
	while (envp[len])
		len++;
	new_envp = ft_calloc((size_t) len, sizeof(char *));
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	j = 0;
	while (envp[++i] && j < len)
	{
		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
		{
			flag = 1;
			continue ;
		}
		new_envp[j] = ft_strdup(envp[i]);
		if (!new_envp[j])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
		j++;
	}
	if (flag)
	{
		new_envp[j] = NULL;
		free_2d_array((void ***)&envp);
		return (new_envp);
	}
	free(new_envp[j - 1]);
	new_envp[j - 1] = NULL;
	free_2d_array((void ***)&new_envp);
	return (envp);
}

int	var_exists(char **envp, char *str)
{
	int		i;
	int		j;
	char	*temp;
	
	i = -1;
	while (str[++i])
	{
		if (str[i] == '=')
		{
			i++;
			break;
		}
	}
	temp = malloc(sizeof(char) * (i + 1));
	j = -1;
	while (++j < i)
		temp[j] = str[j];
	temp[j] = '\0';
	if (get_var(envp, temp) == NULL)
	{
		free(temp);
		return (0);
	}
	free(temp);
	return (1);
}

char	**ch_var(char **envp, char *str)
{
	int	i;
	int	len;

	len = -1;
	while (str[++len])
	{
		if (str[len] == '=')
		{
			len++;
			break ;
		}
	}
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(envp[i], str, len))
		{
			free(envp[i]);
			envp[i] = ft_strdup(str);
			break ;
		}
	}
	return (envp);
}

char	**add_env(char **envp, char *str)
{
	char	**new_envp;
	int		i;
	int		flag;

	i = 0;
	flag = 0;
	while (envp[i])
		i++;
	//check with get_var if the variable already exists
	//str: var=abc, we need to separate the var= from str
	if (var_exists(envp, str))
		flag = 1;
	if (!flag)
	{
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
	else
		return (ch_var(envp, str));
}

void	print_env(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
		printf("%s\n", envp[i]);
}

int	print_export(char **envp)
{
	int	i;
	
	i = -1;
	while (envp && envp[++i])
	{
		printf("declare -x ");
		printf("%s\n", envp[i]);
	}
	return (1);
}

int	run_export(char **cmd, char ***envp)
{
	int		i;
	char	**str;

	if (!cmd[1])
		return (print_export(*envp));
	if (cmd[1][0] != '_' && !ft_isalpha(cmd[1][0]))
	{
		ft_putstr_fd("minishell: export: ", 2);
		ft_putstr_fd(cmd[1], 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd("not a valid identifier\n", 2);
		return (0);
	}
	str = ft_split(cmd[1], '=');
	if (!str)
	{
		ft_putstr_fd("minishell: export: ", 2);
		ft_putstr_fd(cmd[1], 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd("not a valid identifier\n", 2);
		return (0);
	}
	i = -1;
	while (str[0][++i])
	{
		if (str[0][i] != '_' && !ft_isalpha(str[0][i]) && \
				!ft_isdigit(str[0][i]))
		{
			ft_putstr_fd("minishell: export: ", 2);
			ft_putstr_fd(str[0], 2);
			ft_putstr_fd(": ", 2);
			ft_putstr_fd("not a valid identifier\n", 2);
			free_2d_array((void ***)&str);
			return (0);
		}
	}
	free_2d_array((void ***)&str);
	*envp = add_env(*envp, cmd[1]);
	if (!envp)
		return (0);
	return (1);
}
