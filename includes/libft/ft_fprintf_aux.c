/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fprintf_aux.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: goncalo1021pt <goncalo1021pt@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 00:17:09 by gfontao-          #+#    #+#             */
/*   Updated: 2025/05/13 23:32:37 by goncalo1021      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putnbr_base_fd(int fd, unsigned int nbr, char *base, int total)
{
	int		bl;
	char	c;

	bl = ft_strlen(base);
	if ((nbr / bl) > 0)
	{
		total = ft_putnbr_base_fd(fd, nbr / bl, base, total);
		total = ft_putnbr_base_fd(fd, nbr % bl, base, total);
	}
	else
	{
		c = base[nbr];
		total += write(fd, &c, 1);
	}
	return (total);
}

int	ft_putpt_fd(int fd, unsigned long int nbr, char *base, int total)
{
	int		bl;
	char	c;

	bl = ft_strlen(base);
	if ((nbr / bl) > 0)
	{
		total = ft_putpt_fd(fd, nbr / bl, base, total);
		total = ft_putpt_fd(fd, nbr % bl, base, total);
	}
	else
	{
		c = base[nbr];
		total += write(fd, &c, 1);
	}
	return (total);
}

int	ft_putnbrt_fd(int fd, int n, int total)
{
	char	c;

	if (n == -2147483648)
		total += write(fd, "-2147483648", 11);
	else
	{
		if (n < 0)
		{
			n *= -1;
			total += write(fd, "-", 1);
		}
		if ((n / 10) > 0)
		{
			total = ft_putnbrt_fd(fd, n / 10, total);
			total = ft_putnbrt_fd(fd, n % 10, total);
		}
		else
		{
			c = n + '0';
			total += write(fd, &c, 1);
		}
	}
	return (total);
}

int	ft_putunbr_fd(int fd, unsigned int n, int total)
{
	char	c;

	if ((n / 10) > 0)
	{
		total = ft_putunbr_fd(fd, n / 10, total);
		total = ft_putunbr_fd(fd, n % 10, total);
	}
	else
	{
		c = n + '0';
		total += write(fd, &c, 1);
	}
	return (total);
}
