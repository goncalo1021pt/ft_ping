/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: goncalo1021pt <goncalo1021pt@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:26:43 by goncalo1021       #+#    #+#             */
/*   Updated: 2025/05/20 12:26:55 by goncalo1021      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strncat(char *dest, const char *src, size_t n)
{
	char	*od;
	size_t	ctd;

	ctd = 0;
	od = dest;
	dest += ft_strlen(dest);
	while (ctd < ft_strlen(src) && ctd < n)
	{
		*dest = src[ctd];
		dest++;
		ctd++;
	}
	*dest = '\0';
	return (od);
}
