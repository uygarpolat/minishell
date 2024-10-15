/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 10:36:09 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/15 16:54:42 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute(t_ast *s)
{
	//go through children
	//if childs token == pipe -> pipe and redirect output to the pipe with dup
	//if childs token == > -> redirect output to whatever comes after >
	//if next childs == null -> just execute the command
	//if child == cd -> use a built in to move (chdir), remember to change the env
}
