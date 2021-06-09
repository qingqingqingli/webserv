#include "helper.hpp"

int		ft_atoi(const char *str)
{
    int				i;
    int				sign;
    unsigned long	result;

    i = 0;
    sign = 1;
    result = 0;
    while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
        i++;
    if (str[i] == '-')
        sign = -1;
    if (str[i] == '-' || str[i] == '+')
        i++;
    while (str[i] >= '0' && str[i] <= '9')
    {
        result = (result * 10) + (str[i] - '0');
        if (result >= 9223372036854775808UL && (sign == -1))
            return (0);
        if (result >= 9223372036854775807 && (sign == 1))
            return (-1);
        i++;
    }
    return ((int)(result * sign));
}

char	*ft_strdup(const char *str)
{
    int		index;
    char	*result;

    result = (char *)malloc(strlen(str) + 1);
    if (!result)
        throw (std::string("Malloc failed"));
    index = 0;
    while (str[index] != '\0')
    {
        result[index] = str[index];
        index++;
    }
    result[index] = '\0';
    return (result);
}

static int	get_n_size(int n)
{
    int	i;

    if (n > 0)
        i = 0;
    else
        i = 1;
    while (n != 0)
    {
        n = n / 10;
        i++;
    }
    return (i);
}

static char	*put_in_array(char *result, int i, int n, int size)
{
    if (n == -2147483648)
    {
        result[0] = '-';
        result[1] = '2';
        n = 147483648;
    }
    if (n == 0)
    {
        result[0] = '0';
        result[1] = '\0';
        return (result);
    }
    if (n < 0)
    {
        result[0] = '-';
        n = n * -1;
    }
    while (n != 0)
    {
        result[i] = (n % 10) + '0';
        n = n / 10;
        i--;
    }
    result[size] = '\0';
    return (result);
}

char		*ft_itoa(int n)
{
    int		size;
    int		i;
    char	*result;

    size = get_n_size(n);
    i = size - 1;
    result = (char *)malloc(sizeof(char) * (size + 1));
    if (!result)
        return (0);
    result = put_in_array(result, i, n, size);
    return (result);
}

size_t		ft_strlen(const char *s)
{
	size_t len;

	len = 0;
	while (s[len] != '\0')
	{
		len++;
	}
	return (len);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i1;
	size_t	i2;
	char	*dst;

	if (s1 == 0 || s2 == 0)
		return (0);
	i1 = 0;
	i2 = 0;
	dst = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!dst)
        throw (std::string("Malloc failed"));
	while (s1[i1] != '\0')
	{
		dst[i1] = s1[i1];
		i1++;
	}
	while (s2[i2] != '\0')
	{
		dst[i1 + i2] = s2[i2];
		i2++;
	}
	dst[i1 + i2] = '\0';
	return (dst);
}
