#include "helper.hpp"

std::string	remove_duplicate_forward_slashes(std::string location)
{
	std::string	result;
	size_t		x = 0;

	while (x < location.length())
	{
		result += location[x];
		if (location[x] == '/')
		{
			while (location[x] == '/')
				x++;
		}
		else
			x++;
	}
	return (result);
}

std::string	get_file(location_context location_block, std::string location)
{
	std::string	result;
	std::string	temp;
	struct stat	s;

	location = remove_duplicate_forward_slashes(location);
	result = location;
	temp = location;
	if (stat(location.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
		{
			if (location_block.get_index().empty())
			{
				result = "not found";
				return (result);
			}
			temp.append(location_block.get_index());
			if (stat(temp.c_str(), &s) == -1)
			{
				if (location_block.get_autoindex())
					result.append("/index.php");
				else
					result = "not found"; // forbidden instead?
			}
			else
				result.append(location_block.get_index());
		}
		else if (s.st_mode & S_IFREG)
			return (result);
	}
	else
		result = "not found";
	return (result);
}

std::string	skip_first_directory(std::string uri_location)
{
	int			start;
	std::string	result;

	start = uri_location.find_first_of('/', 1);
	if (start == -1)
		result = "";
	else
		result = uri_location.substr(start, std::string::npos);
	return (result);
}

std::string	get_first_directory(std::string uri_location)
{
	int			end;
	std::string	result;

	end = uri_location.find_first_of('/', 1);
	if (end == -1)
		result = uri_location;
	else
		result = uri_location.substr(0, end);
	return (result);
}
