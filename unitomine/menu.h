// AUTORI:
// Massimo Bertolotti - massimo.bertolott86@edu.unito.it - Matricola n° 919899
//
// Guido Buffa		  - guido.buffa@edu.unito.it		 - Matricola n° 919899
// 
#ifndef __MENU_H__
#define __MENU_H__
#include <fstream>

#include "campo.h"

// Funzione che legge un file .txt e converte il contenuto nel campo nascosto di un Campo di Campo Minato.
// La funzione converte i caratteri (- e *) rispettivamente in 0 e 1, indicando celle vuote e mine, pertanto il campo NON viene generato randomicamente.
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

// Verifica che l'input per le operazioni nei menù (principale e delle opzioni) sia della forma corretta, cioè un numero intero positivo (nello
// specifico, una stringa di cifre).
// INPUT:
// • (std::vector<std::string>) input: l'input dell'utente separato "parola per parola" dal metodo 'separa_spazi'
// OUTPUT:
// • 'true' se l'input è nella forma corretta, 'false' altrimenti.
bool input_menu_lecito(std::vector<std::string> input) 
{
	return input.size() == 1 && solo_numeri(input[0]);
}

// Verifica che l'input per definire una partita personalizzata sia della forma corretta, cioè tre numero intero positivo (nello
// specifico, una stringa di cifre).
// INPUT:
// • (std::vector<std::string>) input: l'input dell'utente separato "parola per parola" dal metodo 'separa_spazi'
// OUTPUT:
// 'true' se l'input è nella forma corretta, 'false' altrimenti.
bool input_partita_personalizzata_lecito(std::vector<std::string> input)
{
	return input.size() == 3 && solo_numeri(input[0]) && solo_numeri(input[1]) && solo_numeri(input[2]);
}

// Funzione contenente il menù della creazione di una partita personalizzata. Rimane nel loop (e ripropone all'utente il prompt) finché l'input
// non viene dato correttamente.
// OUTPUT:
// • (Campo): il campo personalizzato secondo le condizioni fornite come input (tramite leggi_input() DENTRO al corpo della funzione)
Campo partita_personalizzata()
{
	std::cout	<< "Inserisci altezza (compresa fra 2 e 50), larghezza (compresa fra 2 e 50) e numero di mine (compreso fra 1 e altezza * larghezza - 1)\n"
				<< "nel formato 'altezza larghezza mine'." << std::endl;

	while (true)
	{
		std::vector<std::string> input = leggi_input();
		if (input_partita_personalizzata_lecito(input))
		{
			int altezza = std::stoi(input[0]);
			int larghezza = std::stoi(input[1]);
			int mine = std::stoi(input[2]);
			
			if (altezza > 1 && altezza <= 50 && larghezza > 1 && larghezza <= 50 && mine >= 1 && mine < altezza * larghezza)
			{
				return Campo(altezza, larghezza, mine);
			}
			else
			{
				std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

// Funzione contenente il menù principale. Rimane nel loop (e ripropone all'utente il prompt) finché l'input non viene dato correttamente.
// OUTPUT:
// • (Campo): il campo scelto secondo le condizioni fornite come input
Campo menu_principale(bool& uscita_programma, bool& in_gioco, bool& campo_generato)
{
	std::ifstream f("epic_scheme.txt");  // Legge il file così denominato per utilizzarlo come schema per la EPIC MODE. 
	
	while (true)
	{
		std::vector<std::string> input = leggi_input();

		if (input_menu_lecito(input))
		{
			int comando_opzioni = std::stoi(input[0]);
			switch (comando_opzioni)
			{
			case 1: // PRINCIPIANTE
				in_gioco = true;
				return Campo(9, 9, 10);
			case 2: // INTERMEDIO
				in_gioco = true;
				return Campo(16, 16, 40);
			case 3: // ESPERTO
				in_gioco = true;
				return Campo(16, 30, 99);
			case 4: // EPIC MODE: easter egg, legge il campo contenuto in f: il campo contiene la scritta
					// "NEVER GONNA GIVE YOU UP" dall'omonima canzone di Rick Astley. La funzione può essere generalizzata in "carica campo personalizzato",
					// se desiderato, modificando opportunamente il file .txt . 
				std::cout << "We're no stranger to love..." << std::endl;
				campo_generato = true;  // Visto che viene utilizzato uno schema predefinito dal file .txt campo_generato passa a true, evita che venga
										// generato di nuovo il campo randomicamente
				in_gioco = true;
				return Campo(leggi_campo_da_file(f));
			case 5: // PERSONALIZZATA
				in_gioco = true;
				return partita_personalizzata();
			case 6: // ESCI DALLA PARTITA
				uscita_programma = true;
				return Campo(9, 9, 10);		// Restituiamo un campo del tutto futile, dato che non verrà giocato.
			case 42: // EASTER EGG basato sulla "Guida Galattica per Autostoppisti", da cui è tratta la citazione stampata a schermo e il numero 42, usato per le dimensioni del campo.
				std::cout << u8"\"La Vita, l'Universo, e il Tutto. C'è una risposta. Ma ci devo pensare.\"" << std::endl;
				in_gioco = true;
				return Campo(42, 42, 420);
			default:
				std::cout << u8"Riprova! Devi scrivere un numero da 1 a 6."
				<< std::endl;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}

	}
}

// Funzione contenente il menù delle opzioni "corto". Viene chiamato se la partita è stata svelata oppure alla fine di una partita (vinta o persa).
// Rimane nel loop (e ripropone all'utente il prompt) finché l'input non viene dato correttamente: nel menù viene proposto la possibilità di tornare
// al menù principale OPPURE di uscire dal gioco.
// INPUT:
// • (Campo&) partita: la partita
// • (bool) uscita_programma: se è 'true', bypassa tutti i loop successivi ed esce dal programma.
// • (bool) in_risolutore: se è 'false', esce e bypassa il loop del risolutore.
// • (bool) in_gioco: se è 'false', esce e bypassa i loop di gioco.
void menu_opzioni_breve(Campo& partita, bool& uscita_programma, bool& in_risolutore, bool& in_gioco)
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
				partita.reset();

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

// Funzione contenente il menù delle opzioni "completo". Viene chiamato col comando 'O' (lettera) durante il gioco.
// Rimane nel loop (e ripropone all'utente il prompt) finché l'input non viene dato correttamente.
// INPUT:
// • (Campo&) partita: la partita
// • (bool) uscita_programma: se è 'true', bypassa tutti i loop successivi ed esce dal programma.
// • (bool) in_risolutore: se è 'false', esce e bypassa il loop del risolutore.
// • (bool) in_gioco: se è 'false', esce e bypassa i loop di gioco.
// • (bool) prima_mossa_effettuata : ad inizio partita è posta come 'false'; quando si fa la prima mossa entrando in un IF si randomizza
// il campo di mine in modo da generare uno spazio vuoto (o con numeri) 3x3 attorno alla casella scelta, poi viene settata come 'true'
// in modo da non rigenerare le mine ogni volta 
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
				partita.reset_bandiere();

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

// Funzione che interpreta l'input ricevuto dal giocatore durante il loop di gioco per convertirlo o in un opportuno comando di gioco
// oppure per attivare il menù delle opzioni oppure per attivare il risolutore.
// INPUT:
// • (Campo&) partita: la partita
// • (int) riga: l'indice della riga della cella su cui compieremo il comando.
// • (int) colonna: l'indice della della cella su cui compieremo il comando.
// • (char) comando: la lettera che identifica il comando che andremo a compiere.
// • (bool) in_risolutore: se è 'false', esce e bypassa il loop del risolutore.
// • (bool) prima_mossa_effettuata : ad inizio partita è posta come 'false'; quando si fa la prima mossa entrando in un IF si randomizza
// il campo di mine in modo da generare uno spazio vuoto (o con numeri) 3x3 attorno alla casella scelta, poi viene settata come 'true'
// in modo da non rigenerare le mine ogni volta 

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

#endif // __MENU_H__