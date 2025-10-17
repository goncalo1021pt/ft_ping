/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: goncalo1021pt <goncalo1021pt@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:31:42 by goncalo1021       #+#    #+#             */
/*   Updated: 2025/05/20 12:31:43 by goncalo1021      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	ctd;

	ctd = 0;
	while (s1[ctd] && s2[ctd] && s1[ctd] == s2[ctd])
		ctd++;
	return ((unsigned char)s1[ctd] - (unsigned char)s2[ctd]);
}
