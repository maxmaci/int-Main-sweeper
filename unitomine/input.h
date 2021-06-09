#ifndef __INPUT_H__
#define __INPUT_H__

/* FUNZIONI PER LA LETTURA DI INPUT */

// Separa le parole (individuate come sottostringhe isolate da spazi) in una data stringa e le immagazzina ciascuna come elemento di un vettore.
// INPUT: 
// •  (std::string) input: la stringa da spezzare
// OUTPUT:
// •  (std::vector<std::string>) : la stringa, separata parola per parola.
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

// Verifica se una stringa ha come caratteri solo delle cifre (0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
// INPUT: 
// •  (std::string) input: la stringa da controllare
// OUTPUT:
// •  (bool) : 'true' se la stringa è costituita da sole cifre, 'false' altrimenti
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

// Legge l'input del giocatore come stringa (che ammette spazi!) e lo spezza già parola per parola. Controlla inoltre che il comando di input abbia meno di 25 caratteri
// (limite arbitrario) per evitare possibili errori e che l'utente stressi il programma inutilmente.
// OUTPUT:
// •  (std::vector<std::string>) : l'input dato dal giocatore, separato parola per parola.
std::vector<std::string> leggi_input()
{
	std::string input_gioco;
	std::cout << "> ";
	std::getline(std::cin, input_gioco);
	if (input_gioco.size() > 25) throw std::length_error("lunghezza dell'input eccessiva");

	return std::vector<std::string>() = separa_spazi(input_gioco);
}

#endif // __INPUT_H__