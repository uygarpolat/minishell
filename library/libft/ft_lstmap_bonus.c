/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 15:26:30 by upolat            #+#    #+#             */
/*   Updated: 2024/04/22 00:00:21 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	retrospective_deletion(t_list *newlist,
	t_list *temp, void (*del)(void *))
{
	while (newlist != NULL)
	{
		temp = newlist->next;
		(*del)(newlist->content);
		free(newlist);
		newlist = temp;
	}
}

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*newlist;
	t_list	*newnode;
	t_list	*temp;

	temp = NULL;
	newlist = NULL;
	while (lst != NULL)
	{
		newnode = malloc(sizeof(t_list));
		if (newnode == NULL)
		{
			retrospective_deletion(newlist, temp, del);
			return (NULL);
		}
		newnode->content = (*f)(lst->content);
		newnode->next = NULL;
		if (newlist == NULL)
			newlist = newnode;
		else
			temp->next = newnode;
		temp = newnode;
		lst = lst->next;
	}
	return (newlist);
}
