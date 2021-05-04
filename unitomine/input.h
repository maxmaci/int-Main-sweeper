#ifndef __INPUT_H__
#define __INPUT_H__

/* FUNZIONI PER LA LETTURA DI INPUT */

std::vector<std::string> separa_spazi(std::string input)
{
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < input.size(); i++)
	{
		temp.push_back(input[i]);
		if (std::isspace(input[i + 1]) || i == input.size() - 1)
		{
			res.push_back(temp);
			temp.resize(0);
			i++;
		}
	}
	return res;
}

bool solo_numeri(std::string input)
{
	std::string::const_iterator it = input.cbegin();
	while (it != input.cend())
	{
		if (!std::isdigit(*it)) return false;
		else it++;
	}
	return true;
}

std::vector<std::string> leggi_input()
{
	std::string input_gioco;
	std::cout << "> ";
	std::getline(std::cin, input_gioco);
	if (input_gioco.size() > 100) throw std::length_error("lunghezza dell'input eccessiva");

	return std::vector<std::string>() = separa_spazi(input_gioco);
}

#endif // __INPUT_H__