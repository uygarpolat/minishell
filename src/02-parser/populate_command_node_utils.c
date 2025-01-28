/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   populate_command_node_utils.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 08:28:36 by upolat            #+#    #+#             */
/*   Updated: 2025/01/28 08:29:12 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

int	get_size_split(t_tokens *tokens, char *str)
{
	char	**split;
	int		i;

	split = ft_split(str, ' ');
	if (split == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	i = 0;
	while (split[i])
		i++;
	free_2d_array((void ***)&split);
	return (i);
}

int	malloc_when_token_is_expanded(t_tokens *tokens, t_ast *root, int i)
{
	char	**split;
	int		j;

	split = ft_split(tokens[i].value, ' ');
	if (split == NULL)
		return (1);
	j = 0;
	while (split[j])
	{
		*root->words = ft_strdup(split[j]);
		if (*root->words == NULL)
		{
			free_2d_array((void ***)&split);
			return (1);
		}
		j++;
		root->words++;
	}
	free_2d_array((void ***)&split);
	return (0);
}
