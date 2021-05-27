#ifndef __INPUT_H__
#define __INPUT_H__

/* FUNZIONI PER LA LETTURA DI INPUT */

// Permette di prendere con un solo comando diversi tipi di input (x es.: riga, colonna e tipo di azione) grazie alla spaziatura tra i caratteri
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

// Conta i numeri presenti nell'input (?)
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

// Controlla che il comando di input abbia meno di 25 battute per evitare possibili errori e che l'utente stressi il programma
std::vector<std::string> leggi_input()
{
	std::string input_gioco;
	std::cout << "> ";
	std::getline(std::cin, input_gioco);
	if (input_gioco.size() > 25) throw std::length_error("lunghezza dell'input eccessiva");

	return std::vector<std::string>() = separa_spazi(input_gioco);
}

#endif // __INPUT_H__