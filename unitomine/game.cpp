#include <ctime>

#include "gioco.h"

/* Classe Risolutore */
class Risolutore
{
private:
	Gioco giocata_attuale;		// ...
	Gioco giocata_precedente;	// ...
public:
	Risolutore(Gioco);
	void metodo_meccanico();
	// qualcosa di più efficace che il metodo prob.
	void metodo_probabilistico();
	void risolve();
};

/* FUNZIONI MENÙ */

/* CONTROLLI sulla validità dell'input */
bool input_menu_lecito(std::vector<std::string> input)
{
	return input.size() == 1 && solo_numeri(input[0]);
}

bool input_personalizzata_lecito(std::vector<std::string> input)
{
	return input.size() == 3 && solo_numeri(input[0]) && solo_numeri(input[1]) && solo_numeri(input[2]);
}

void partita_personalizzata(Gioco& gioco)
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
			gioco.resize(altezza, larghezza, mine);
			return;
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
	}
}

void menu_principale(Gioco& gioco, bool& uscita_programma)
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
				gioco.resize(9, 9, 10);
				return;
			case 2:
				gioco.resize(16, 16, 40);
				return;
			case 3:
				gioco.resize(16, 30, 99);
				return;
			case 4:
				std::cout << "We're no stranger to love..." << std::endl;
				return;
				/* TO DO: metterci la tabella */
			case 5:
				partita_personalizzata(gioco);
				return;
			case 6:
				uscita_programma = true;
				return;
			case 42:
				std::cout << u8"\"La Vita, l'Universo, e il Tutto. C'è una risposta. Ma ci devo pensare.\"" << std::endl;
				gioco.resize(42, 42, 420);
				return;
			default:
				std::cout << u8"Lo vedi che ci sono solo numeri dall'1 al 6, vero? Che diamine scrivi "
					<< comando_opzioni
					<< u8" quando chiaramente\nnon ti mostrerà nulla di importante?"
					<< u8" Piuttosto che perdere tempo così, potresti chiederti\n"
					<< u8"se c'è una risposta alla Vita, all'Universo, al Tutto." << std::endl;
			}
		}
		else
		{
			std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
		}
		
	}
}

void menu_opzioni_breve(Gioco& gioco, bool& uscita_programma, bool& in_gioco)
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
				return;
			case 2:
				in_gioco = false;
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

void menu_opzioni(Gioco& gioco, bool& uscita_programma, bool& in_gioco)
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
				gioco.reset_giocatore();
				gioco.reset_numero_bandiere();
				return;
			case 3:
				gioco.rivela();
				menu_opzioni_breve(gioco, uscita_programma, in_gioco);
				return;
			case 4:
				gioco.reset();

				in_gioco = false;
				return;
			case 5:
				in_gioco = false;
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

void interpreta_mossa(Gioco& gioco, int& riga, int& colonna, char& comando)
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
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;										//throw std::invalid_argument("comando non valido");
			if (!gioco._campo_giocatore().nel_campo(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;	//throw std::domain_error("coordinate non valide");

			riga = std::stoi(input[0]);
			colonna = std::stoi(input[1]);
			comando = 'S';

			return;
		case 3:
			// TO DO: controllare qualche caso
			if (!solo_numeri(input[0]) || !solo_numeri(input[1])) break;
			if (!gioco._campo_giocatore().nel_campo(std::stoi(input[0]) - 1, std::stoi(input[1]) - 1)) break;		// throw std::invalid_argument("coordinate non valide");
			if (input[2].size() > 1 || !comando_lecito(std::toupper(input[2][0]))) break;						// throw std::domain_error("comando non valido");

			riga = std::stoi(input[0]);
			colonna = std::stoi(input[1]);
			comando = std::toupper(input[2][0]);

			return;
		default:
			
			break;
		}

		std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
	}
}

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

	// resetta il clock per rendere la generazione (pseudo)-casuale più casuale

	std::srand(std::time(nullptr));

	// Variabili per comandi di gioco: coordinate della cella + comando dell'azione

	int riga = 1;
	int colonna = 1;
	char comando = 'S';

	/* FLAG per controllare i loop di gioco */
	
	// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma

	bool uscita_programma = false;

	// uscita_opzioni : se è 'false', esce dal ciclo del gioco per tornare al menù principale oppure per uscire dal programma
	bool in_gioco = false;

	// prima_mossa_effettuata : ad inizio partita è posta come 'false'; quando si fa la prima mossa entrando in un IF si randomizza
	// il campo di mine in modo da generare uno spazio vuoto (o con numeri) 3x3 attorno alla casella scelta, poi viene settata come 'true'
	// in modo da non rigenerare le mine ogni volta 

	bool prima_mossa_effettuata = false;

	/* LOOP DEL PROGRAMMA: finchè non gli viene dato l'input di uscita del menù tornerà qui */

	// inizializza la tabella di gioco
	Gioco gioco(9, 9, 10);

	while (!uscita_programma)
	{

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
		std::cout << "Un gioco in C++ programmato da Bertolotti Massimo e Buffa Guido." << std::endl << std::endl;


		/* MENU */

			// Inserendo come input il corrispettivo numero è possibile scegliere una modalità fra quelle predefinite
			// (PRINCIPANTE, INTERMEDIO, ESPERTO), la EPIC MODE (un piccolo Easter Egg), creare una tabella personalizzata
			// OPPURE uscire dal gioco. Se il comando non è uno di questi sei OPPURE il numero 42 (altro piccolo Easter Egg) non fa nulla
			// eccetto segnalare al giocatore che il commando non è riconosciuto e ripropone il prompt.

		/* TO DO: mettere questo lettore input come l'altro in gioco */

		std::cout << "Seleziona un opzione scrivendo il numero corrispondendente nel prompt:" << std::endl;
		std::cout	<< u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
					<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
					<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
					<< u8"• EPIC MODE \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
					<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
					<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;
		
		menu_principale(gioco, uscita_programma);
		
		in_gioco = true;

	/* TO DO: menù per chiedere risolutore */

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			while (gioco._status() == '-')
			{
				std::cout << gioco._campo_giocatore() << std::endl;
				std::cout << u8"Inserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt 'O'." << std::endl;
				std::cout << "Hai messo " << gioco._numero_bandiere() << " bandiere su " << gioco._mine() << " mine presenti." << std::endl;
				
				interpreta_mossa(gioco, riga, colonna, comando);
				
				// Esce dalla partita per le opzioni
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima (tranne quando si ricomincia la partita), il campo viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!prima_mossa_effettuata)
				{
					gioco.randomizza_campo(riga - 1, colonna - 1);
					prima_mossa_effettuata = true;
				}
				gioco.gioca(riga - 1, colonna - 1, comando);

				/* TO DO: controllare che sia lecito come controllo */
			}

			// NON entra nelle opzioni se il gioco è stato vinto o perso.

			if (gioco._status() == 'S' || gioco._status() == 'V')
			{
				break;
			}

			/* TO DO: mettere questo lettore input come l'altro in gioco */

			std::cout	<< "OPZIONI:\n"
						<< u8"• Torna al gioco.\t\t(1)\n"
						<< u8"• Ricomincia la partita.\t(2)\n"
						<< u8"• Rivela il campo.\t\t(3)\n"
						<< u8"• Torna al menù principale.\t(4)\n"
						<< u8"• Esci dal gioco.\t\t(5)" << std::endl;
			
			menu_opzioni(gioco, uscita_programma, in_gioco);

		}

		if (gioco._status() == 'S' || gioco._status() == 'V')
		{
			std::cout << gioco._campo_giocatore() << std::endl;
			
			if (gioco._status() == 'S')
			{
				std::cout << u8"OH NO! Hai perso. ☹\nCosa vuoi fare ora?\n";
			}
			else
			{
				std::cout << u8"HAI VINTO! ☺\nCosa vuoi fare ora?\n";
			}
			
			menu_opzioni_breve(gioco, uscita_programma, in_gioco);
			
			}
		prima_mossa_effettuata = false;
		in_gioco = true;
	}
}