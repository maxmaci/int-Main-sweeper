#ifndef __MENU_H__
#define __MENU_H__

#include "campo.h"

/* FUNZIONI MEN� */

/* CONTROLLI sulla validit� dell'input */
bool input_menu_lecito(std::vector<std::string> input)
{
	return input.size() == 1 && solo_numeri(input[0]);
}

bool input_personalizzata_lecito(std::vector<std::string> input)
{
	return input.size() == 3 && solo_numeri(input[0]) && solo_numeri(input[1]) && solo_numeri(input[2]);
}

void partita_personalizzata(Campo& campo)
{
	std::cout << "Inserisci altezza, larghezza e numero di mine (nel formato 'altezza', 'larghezza', 'mine'):" << std::endl; // TO DO: evitare errori di crash

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

void menu_principale(Campo& gioco, bool& uscita_programma, bool& in_gioco)
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
				in_gioco = true;
				return;
				/* TO DO: metterci la tabella */
			case 5:
				partita_personalizzata(gioco);
				in_gioco = true;
				return;
			case 6:
				uscita_programma = true;
				return;
			case 42:
				std::cout << u8"\"La Vita, l'Universo, e il Tutto. C'� una risposta. Ma ci devo pensare.\"" << std::endl;
				gioco = Campo(42, 42, 420);
				in_gioco = true;
				return;
			default:
				std::cout << u8"Lo vedi che ci sono solo numeri dall'1 al 6, vero? Che diamine scrivi "
					<< comando_opzioni
					<< u8" quando chiaramente\nnon ti mostrer� nulla di importante?"
					<< u8" Piuttosto che perdere tempo cos�, potresti chiederti\n"
					<< u8"se c'� una risposta alla Vita, all'Universo, al Tutto." << std::endl;
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
	std::cout << u8"Vuoi che il Risolutore� giochi al posto tuo?\n"
		<< u8"� S�, fai giocare il Risolutore�.\t (1)\n"
		<< u8"� No, voglio giocare io.\t\t (2)" << std::endl;

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

void menu_opzioni_breve(Campo& gioco, bool& uscita_programma, bool& in_risolutore, bool& in_gioco)
{
	std::cout << "OPZIONI:\n"
		<< u8"� Torna al men� principale.\t (1)\n"
		<< u8"� Esci dal gioco.\t\t (2)" << std::endl;

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

void menu_opzioni(Campo& partita, bool& uscita_programma, bool& in_risolutore, bool& in_gioco, bool& campo_generato)
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
				partita.reset_giocatore();
				partita.reset_numero_bandiere();

				campo_generato = false;
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

void interpreta_mossa(Campo& partita, int& riga, int& colonna, char& comando, bool& in_risolutore, bool& campo_generato)
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
			// TO DO: controllare qualche caso
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;
			if (!partita._campo_visibile().indici_leciti(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;				// throw std::invalid_argument("coordinate non valide");
			if (input[2].size() > 1 || (!comando_lecito(std::toupper(input[2][0])) && std::toupper(input[2][0]) != 'R')) break;	// throw std::domain_error("comando non valido");

			riga = std::stoi(input[0]);
			colonna = std::stoi(input[1]);

			if (!campo_generato && std::toupper(input[2][0]) == 'R')
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