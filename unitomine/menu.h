#ifndef __MENU_H__
#define __MENU_H__
#include <fstream>

#include "campo.h"

//'leggi_campo_da_file' utilizza come schema predefinito per le mine quello fornito da un file .txt, il campo NON viene generato randomicamente,
//converte i caratteri (- e *) rispettivamente in 0 e 1, indicando celle vuote e mine
Campo leggi_campo_da_file(std::istream& is) {
	Matrice<bool> campo_input;
	std::string stringa;
	while (is >> stringa)
	{
		std::vector<bool> futura_riga(stringa.size(), false);
		for (int i = 0; i < futura_riga.size(); i++)
		{
			if (stringa[i] == '*') futura_riga[i] = true;
		}
		campo_input.push_back(futura_riga);
	}
	Campo res(campo_input);
	return res;
}

/* FUNZIONI MENÙ */

/* CONTROLLI sulla validità dell'input */
bool input_menu_lecito(std::vector<std::string> input)  //Verifica che il comando per le operazioni nel menu sia della forma corretta (un numero intero)
														//NB: l'input viene preso come vettore di stringhe formate solo da interi, viene quindi
														//verificato che sia UNA stringa (->inserire '42' come input non dà errori)
{
	return input.size() == 1 && solo_numeri(input[0]);
}

bool input_personalizzata_lecito(std::vector<std::string> input) //Verifica che il comando per le dimensioni del campo personalizzato sia lecito
{
	return input.size() == 3 && solo_numeri(input[0]) && solo_numeri(input[1]) && solo_numeri(input[2]);
}

void partita_personalizzata(Campo& campo) //Si entra nella modalità di gioco con campo personalizzato, se scelta nel menu
{
	std::cout << "Inserisci altezza, larghezza e numero di mine (nel formato 'altezza', 'larghezza', 'mine'):" << std::endl;

	while (true)
	{
		std::vector<std::string> input = leggi_input();
		if (input_personalizzata_lecito(input))
		{
			int altezza = std::stoi(input[0]);
			int larghezza = std::stoi(input[1]);
			int mine = std::stoi(input[2]);
			campo = Campo(altezza, larghezza, mine);
			return;
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

void menu_principale(Campo& gioco, bool& uscita_programma, bool& in_gioco, bool& campo_generato, bool& prima_mossa_effettuata)
{
	std::ifstream f("epic_scheme.txt");  //legge il file così denominato per utilizzarlo come schema predefinito. Per usare uno schema 
	
	while (true)
	{
		std::vector<std::string> input = leggi_input();

		if (input_menu_lecito(input))
		{
			int comando_opzioni = std::stoi(input[0]);
			switch (comando_opzioni)
			{
			case 1:
				gioco = Campo(9, 9, 10);
				in_gioco = true;
				return;
			case 2:
				gioco = Campo(16, 16, 40);
				in_gioco = true;
				return;
			case 3:
				gioco = Campo(16, 30, 99);
				in_gioco = true;
				return;
			case 4:
				std::cout << "We're no stranger to love..." << std::endl;
				gioco = Campo(leggi_campo_da_file(f));
				campo_generato = true;  //visto che viene utilizzato uno schema predefinito dal file .txt campo_generato passa a true, evita che venga
										//generato di nuovo 
										//prima_mossa_effettuata = true;
				in_gioco = true;
				return;
			case 5:
				partita_personalizzata(gioco);
				in_gioco = true;
				return;
			case 6:
				uscita_programma = true;
				return;
			case 42:
				std::cout << u8"\"La Vita, l'Universo, e il Tutto. C'è una risposta. Ma ci devo pensare.\"" << std::endl;
				gioco = Campo(42, 42, 420);
				in_gioco = true;
				return;
			default:
				std::cout << u8"Riprova! Devi scrivere un numero da 1 a 6! smh "
				<< std::endl;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}

	}
}

void menu_risolutore(bool& in_risolutore)
{
	std::cout << u8"Vuoi che il Risolutore™ giochi al posto tuo?\n"
		<< u8"• Sì, fai giocare il Risolutore™.\t (1)\n"
		<< u8"• No, voglio giocare io.\t\t (2)" << std::endl;

	while (true)
	{
		std::vector<std::string> input = leggi_input();

		if (input_menu_lecito(input))
		{
			int comando_opzioni = std::stoi(input[0]);

			switch (comando_opzioni)
			{
			case 1:
				in_risolutore = true;
				return;
			case 2:
				return;
			default:
				std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
				break;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

//menu di fine partita con risolutore (vittoria o sconfitta), resetta stato di gioco
void menu_opzioni_breve(Campo& gioco, bool& uscita_programma, bool& in_risolutore, bool& in_gioco)
{
	std::cout << "OPZIONI:\n"
		<< u8"• Torna al menù principale.\t (1)\n"
		<< u8"• Esci dal gioco.\t\t (2)" << std::endl;

	while (true)
	{
		std::vector<std::string> input = leggi_input();

		if (input_menu_lecito(input))
		{
			int comando_opzioni = std::stoi(input[0]);

			switch (comando_opzioni)
			{
			case 1:
				gioco.reset();

				in_gioco = false;
				in_risolutore = false;
				return;
			case 2:
				in_gioco = false;
				in_risolutore = false;
				uscita_programma = true;
				return;
			default:
				std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
				break;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

void menu_opzioni(Campo& partita, bool& uscita_programma, bool& in_risolutore, bool& in_gioco, bool& prima_mossa_effettuata)
{
	while (true)
	{
		std::vector<std::string> input = leggi_input();

		if (input_menu_lecito(input))
		{
			int comando_opzioni = std::stoi(input[0]);
			switch (comando_opzioni)
			{
			case 1:
				return;
			case 2:
				partita.reset_campo_visibile();
				partita.reset_numero_bandiere();

				prima_mossa_effettuata = false;
				return;
			case 3:
				partita.rivela();
				menu_opzioni_breve(partita, uscita_programma, in_risolutore, in_gioco);
				return;
			case 4:
				partita.reset();

				in_gioco = false;
				in_risolutore = false;
				return;
			case 5:
				in_gioco = false;
				in_risolutore = false;
				uscita_programma = true;
				return;
			default:
				std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
				break;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

void interpreta_mossa(Campo& partita, int& riga, int& colonna, char& comando, bool& in_risolutore, bool& prima_mossa_effettuata)
{
	while (true)
	{
		std::vector<std::string> input = leggi_input();

		switch (input.size())
		{
		case 1:
			if (input[0].size() > 1 || std::toupper(input[0][0]) != 'O') break; // throw std::invalid_argument("comando non valido");

			comando = 'O';

			return;
		case 2:
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;											//throw std::invalid_argument("comando non valido");
			if (!partita._campo_visibile().indici_leciti(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;	//throw std::domain_error("coordinate non valide");

			riga = std::stoi(input[0]);
			colonna = std::stoi(input[1]);
			comando = 'S';

			return;
		case 3:
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;
			if (!partita._campo_visibile().indici_leciti(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;				// throw std::invalid_argument("coordinate non valide");
			if (input[2].size() > 1 || (!comando_lecito(std::toupper(input[2][0])) && std::toupper(input[2][0]) != 'R')) break;	// throw std::domain_error("comando non valido");

			riga = std::stoi(input[0]);
			colonna = std::stoi(input[1]);

			if (!prima_mossa_effettuata && std::toupper(input[2][0]) == 'R')
			{
				comando = 'S';
				in_risolutore = true;
				return;
			}
			else if (std::toupper(input[2][0]) != 'R')
			{
				comando = std::toupper(input[2][0]);
				return;
			}
		default:

			break;
		}

		std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
	}
}

void inizializza_risolutore(Campo& partita)
{
	while (true)
	{
		std::vector<std::string> input = leggi_input();

		switch (input.size())
		{
		case 2:
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;											//throw std::invalid_argument("comando non valido");
			if (!partita._campo_visibile().indici_leciti(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;	//throw std::domain_error("coordinate non valide");

			partita.randomizza_campo(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1);
			partita.gioca(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1, 'S');

			return;
		default:
			break;
		}

		std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
	}
}

#endif // __MENU_H__