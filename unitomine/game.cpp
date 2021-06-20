// AUTORI:
// Massimo Bertolotti - massimo.bertolott86@edu.unito.it - Matricola n° 919899
//
// Guido Buffa		  - guido.buffa@edu.unito.it		 - Matricola n° 919899
// 

#include <ctime>

#include "risolutore.h"

int main(int argc, char* argv[])
{
	// Controlla se il codice viene compilato nella forma "game <dimensione massima delle sezioni per il metodo probabilistico del risolutore>'.
	// <dimensione massima delle sezioni per il metodo probabilistico del risolutore> è un intero strettamente positivo; il valore consigliato è 20.
	if (argc != 2) {
		std::cerr	<< "Argomenti di compilazione non corretti.\n"
					<< "Utilizzo: game <dimensione massima della sezione di incognite che il risolutore può controllare per il metodo probabilistico>." << std::endl;
	}	
	else
	{
		/* OPZIONI ESCLUSIVE PER WINDOWS PER FAR SÌ CHE VISUALIZZI CARATTERI UNICODE E VISUALIZZARE COLORI */

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

		std::srand(static_cast<int>(std::time(nullptr)));

		/* FLAG per controllare i loop di gioco */

		// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma.

		bool uscita_programma = false;

		// in_gioco : se è 'false', esce e bypassa i loop di gioco.

		bool in_gioco = false;

		// in_risolutore : se è 'false', esce e bypassa il loop del risolutore.

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
			//Campo partita(9, 9, 10);

			/* SCHERMATA PRINCIPALE */

			// Piccola ASCII art con il titolo; il prefisso R"( ... )" indica che il contenuto delle parentesi
			// viene stampato così com'è, quindi '\' NON è prefisso per alcun carattere speciale tipo \n o \t.

			std::cout << R"(
 _       _                     _        ____                                        
(_)     | |                   (_)      / /\ \                                       
 _ _ __ | |_   _ __ ___   __ _ _ _ __ | |  | |_____      _____  ___ _ __   ___ _ __ 
| | '_ \| __| | '_ ` _ \ / _` | | '_ \| |  | / __\ \ /\ / / _ \/ _ \ '_ \ / _ \ '__|
| | | | | |_  | | | | | | (_| | | | | | |  | \__ \\ V  V /  __/  __/ |_) |  __/ |   
|_|_| |_|\__| |_| |_| |_|\__,_|_|_| |_| |  | |___/ \_/\_/ \___|\___| .__/ \___|_|   
                                       \_\/_/                      | |              
                                                                   |_|              
)";
			std::cout << "Un gioco in C++ programmato da Massimo Bertolotti e Guido Buffa." << std::endl << std::endl;

			/* MENU */

				// Inserendo come input il corrispettivo numero è possibile scegliere una modalità fra quelle predefinite
				// (PRINCIPANTE, INTERMEDIO, ESPERTO), la EPIC MODE (un piccolo Easter Egg) OPPURE uscire dal gioco. 
				// Se il comando non è uno di questi OPPURE il numero 42 (altro piccolo Easter Egg)
				// segnala all'utente che il commando non è riconosciuto e ripropone il prompt.

			std::cout << "Seleziona un opzione scrivendo il numero corrispondendente nel prompt:" << std::endl;
			std::cout << u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
				<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
				<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
				<< u8"• EPIC MODE \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
				<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
				<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;

			Campo partita = menu_principale(uscita_programma, in_gioco, campo_generato);

			/* LOOP DI GIOCO*/
			// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
			while (!uscita_programma && in_gioco)
			{
				std::cout << u8"\nInserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt la lettera 'O'." << std::endl;
				std::cout << "COMANDI DISPONIBILI:\n"
					<< u8"• B: mette la bandierina (se non presente)\n"
					<< u8"• T: toglie la bandierina (se presente)\n"
					<< u8"• S: scava la cella (se non è presente la bandierina)\n"
					<< u8"• R: scava la cella indicata e attiva il Risolutore™ automatico (può essere solo attivato alla prima mossa)" << std::endl;
				while (!in_risolutore && partita._status() == '-')
				{
					int riga = 1;
					int colonna = 1;
					char comando = 'S';

					std::cout << partita << std::endl;
					std::cout << "Hai messo " << partita._bandiere() << " bandiere su " << partita._mine() << " mine presenti." << std::endl;

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

					Risolutore risolutore(partita);
					risolutore.risolvi(std::stoi(argv[1]));
					partita = risolutore._partita();
					break;
				}

				// NON entra nelle opzioni se il gioco è stato vinto o perso.

				if (partita._status() == '-')
				{
					std::cout << "OPZIONI:\n"
						<< u8"• Torna al gioco.\t\t\t\t\t(1)\n"
						<< u8"• Ricomincia la partita con lo stesso schema di mine.\t(2)\n"
						<< u8"• Rivela il campo.\t\t\t\t\t(3)\n"
						<< u8"• Torna al menù principale.\t\t\t\t(4)\n"
						<< u8"• Esci dal gioco.\t\t\t\t\t(5)" << std::endl;

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
}
