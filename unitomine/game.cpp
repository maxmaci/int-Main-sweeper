﻿#include <ctime>

#include "risolutore.h"

int main()
{

/* OPZIONI ESCLUSIVE PER WINDOWS PER FAR SÌ CHE VISUALIZZI CARATTERI UNICODE */

#ifdef _WIN32
	// Mette la code page della console in UTF8 cosicché la console sa interpretare le stringhe unicode
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// Abilita il buffering per evitare che Visual Studio tagli le sequenze di byte UTF-8
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	// Mette l'output in modo da obbligare l'uso di sequenze virtuali del terminale.
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return GetLastError();
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return GetLastError();
	}

#endif

/* VARIABILI DI GIOCO/OPZIONI */

	// resetta il clock per rendere la generazione (pseudo)-casuale 'più casuale'

	std::srand(static_cast<int>( std::time(nullptr) ));

	// Inizializzazione di delle variabili per comandi di gioco: coordinate della cella + comando dell'azione

	int riga = 1;
	int colonna = 1;
	char comando = 'S';

	/* FLAG per controllare i loop di gioco */
	
	// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma

	bool uscita_programma = false;

	bool in_gioco = false;

	bool in_risolutore = false;

	// prima_mossa_effettuata : ad inizio partita è posta come 'false'; quando si fa la prima mossa entrando in un IF si randomizza
	// il campo di mine in modo da generare uno spazio vuoto (o con numeri) 3x3 attorno alla casella scelta, poi viene settata come 'true'
	// in modo da non rigenerare le mine ogni volta 

	bool prima_mossa_effettuata = false;

	// 'campo_generato' segnala che campo sia già stato genrato o meno, di ri-generarlo accidentalmente una volta iniziata la partita;
	// cambia di stato dopo che viene caricato uno schema predefinito o una volta giocata la prima mossa

	bool campo_generato = false;

	/* LOOP DEL PROGRAMMA: finchè non gli viene dato l'input di uscita del menù tornerà qui */

	// inizializza la tabella di gioco

	while (!uscita_programma)
	{
		Campo partita(9, 9, 10);
		Risolutore risolutore(partita);

		/* SCHERMATA PRINCIPALE */

		// Piccola ASCII art con il titolo; il prefisso R"( ... )" indica che il contenuto delle parentesi
		// viene stampato così com'è, quindi '\' NON è prefisso per alcun "code" e.g. \n o \t.

			std::cout << R"(
  _    _       _ _        __  __ _            
 | |  | |     (_) |      |  \/  (_)           
 | |  | |_ __  _| |_ ___ | \  / |_ _ __   ___ 
 | |  | | '_ \| | __/ _ \| |\/| | | '_ \ / _ \
 | |__| | | | | | || (_) | |  | | | | | |  __/
  \____/|_| |_|_|\__\___/|_|  |_|_|_| |_|\___|
                                                                                      
)";
		std::cout << "Un gioco in C++ programmato da Massimo Bertolotti e Guido Buffa." << std::endl << std::endl;


		/* MENU */

			// Inserendo come input il corrispettivo numero è possibile scegliere una modalità fra quelle predefinite
			// (PRINCIPANTE, INTERMEDIO, ESPERTO), la EPIC MODE (un piccolo Easter Egg) OPPURE uscire dal gioco. 
			// Se il comando non è uno di questi OPPURE il numero 42 (altro piccolo Easter Egg)
			// segnala all'utente che il commando non è riconosciuto e ripropone il prompt.

		std::cout << "Seleziona un opzione scrivendo il numero corrispondendente nel prompt:" << std::endl;
		std::cout	<< u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
					<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
					<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
					<< u8"• EPIC MODE \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
					<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
					<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;
		
		menu_principale(partita, uscita_programma, in_gioco, campo_generato, prima_mossa_effettuata);

	/* TO DO: menù per chiedere risolutore */

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			std::cout << u8"\nInserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt la lettera 'O'." << std::endl;
			std::cout << "COMANDI DISPONIBILI:\n"
				<< "B: mette la bandierina (se non presente)\n"
				<< "T: toglie la bandierina (se presente)\n"
				<< u8"S: scava la cella (se non è presente la bandierina)\n"
				<< u8"R: scava la cella indicata e attiva il risolutore automatico (può essere solo attivato alla prima mossa)" << std::endl;
			while (!in_risolutore && partita._status() == '-')
			{
				std::cout << partita << std::endl;
				std::cout << "Hai messo " << partita._numero_bandiere() << " bandiere su " << partita._mine() << " mine presenti." << std::endl;
				
				interpreta_mossa(partita, riga, colonna, comando, in_risolutore, prima_mossa_effettuata);
				
				// Esce dalla partita e apre le opzioni se si dà 'O' in input
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima (tranne quando si RICOMINCIA la partita), il campo viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!campo_generato)
				{
					partita.randomizza_campo(riga - 1, colonna - 1);
					campo_generato = true;
					prima_mossa_effettuata = true;
				}
				
				partita.gioca(riga - 1, colonna - 1, comando);
			}

			while (!uscita_programma && in_risolutore)
			{
				std::cout << partita << std::endl;
				//inizializza_risolutore(partita);
				//std::cout << partita << std::endl;

				risolutore.aggiorna(partita);
				risolutore.risolvi(20);
				partita = risolutore._partita();
				break;
			}

			// NON entra nelle opzioni se il gioco è stato vinto o perso.

			if (partita._status() == '-')
			{
				std::cout << "OPZIONI:\n"
					<< u8"• Torna al gioco.\t\t(1)\n"
					<< u8"• Ricomincia la partita con lo stesso schema di mine.\t(2)\n"
					<< u8"• Rivela il campo.\t\t(3)\n"
					<< u8"• Torna al menù principale.\t(4)\n"
					<< u8"• Esci dal gioco.\t\t(5)" << std::endl;

				menu_opzioni(partita, uscita_programma, in_risolutore, in_gioco, prima_mossa_effettuata);

			}
			else
			{
				std::cout << partita << std::endl;

				if (partita._status() == 'S')
				{
					std::cout << u8"OH NO! Hai perso. ☹\nCosa vuoi fare ora?\n";
				}
				else
				{
					std::cout << u8"HAI VINTO! ☺\nCosa vuoi fare ora?\n";
				}

				menu_opzioni_breve(partita, uscita_programma, in_risolutore, in_gioco);

			}

		}

		campo_generato = false;
		prima_mossa_effettuata = false;

	}
}
