/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergmigu <sergmigu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 14:02:24 by gfontao-          #+#    #+#             */
/*   Updated: 2024/02/22 14:18:29 by sergmigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	c;

	c = 0;
	if (n == 0)
		return (0);
	if (!s1 || !s2)
		return (1);
	while (s1[c] == s2[c] && (s1[c] && s2[c]) && c < n - 1)
	{
		c++;
	}
	return ((unsigned char)s1[c] - (unsigned char)s2[c]);
}
