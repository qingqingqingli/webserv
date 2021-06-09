#include "helper.hpp"
#include "../server/client.hpp"

int     check_server_block(std::vector<std::string> server_block) {
    int         open_bracket = 0;
    int         closing_bracket = 0;
    std::string str;

    for (std::vector<std::string>::iterator it = server_block.begin(); it != server_block.end(); it++) {
        str = *it;
        if (str.find('{') != std::string::npos)
            open_bracket++;
        if (str.find('}') != std::string::npos)
            closing_bracket++;
    }
    if (open_bracket == closing_bracket && open_bracket != 0)
        return 1;
    return 0;
}

int    location_block_size(vector_iterator it, vector_iterator end) {
    for (int i = 0; it != end; it++) {
        std::string str = *it;

        if (str.find("}") != std::string::npos)
            return i;
        i++;
    }
    return 0;
}

static int val(char c)
{
    if (c >= '0' && c <= '9')
        return (int)c - '0';
    else
        return (int)c - 'a' + 10;
}

#include <iostream>
int hex_to_dec(std::string str, int base)
{
    int len = str.length();
    int power = 1;
    int num = 0;
    int i;

    if (str == "0")
        return 0;
    for (i = len - 1; i >= 0; i--)
    {
        if (val(str[i]) >= base)
            return -1;
        num += val(str[i]) * power;
        power = power * base;
    }
    return num;
}
