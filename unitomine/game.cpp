#include <ctime>

#include "gioco.h"

/* FUNZIONI PER LA LETTURA DI INPUT */

// pulisce il buffer del cin e ignora tutto quello dopo il carattere in input
void pulisci_cin()
{
	std::cin.clear();
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

std::vector<std::string> separa_spazi(std::string input)
{
	std::vector<std::string> res;
	std::string temp;
	for (int i = 0; i < input.size(); i++)
	{
		temp.push_back(input[i]);
		if (std::isspace(input[i+1]) || i == input.size() - 1)
		{
			//std::cout << temp << " "; /* TO DO: toglierlo quando siamo sicuri funzioni tutto */ 
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

/* TO DO: classe Risolutore? Potrebbe sfruttare i comandi predisposti per gioco, tenendo 'più pulito' il codice*/

int main()
{

/* OPZIONI ESCLUSIVE PER WINDOWS PER FAR SÌ CHE VISUALIZZI CARATTERI UNICODE */

#ifdef _WIN32
	// Mette la code page della console in UTF8 cosicché la console sa interpretare le stringhe unicode
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// Abilita il buffering per evitare che Visual Studio tagli le sequenze di byte UTF-8
	setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

/* VARIABILI DI GIOCO/OPZIONI */

	// resetta il clock per rendere la generazione (pseudo)-casuale più casuale

	std::srand(std::time(nullptr));

	// Variabili per inserire le dimensioni e il numero di mine del campo personalizzato 

	int altezza = 9;
	int larghezza = 9;
	int mine = 10;

	// Variabili per comandi di gioco: coordinate della cella + comando dell'azione

	int x = 0;
	int y = 0;
	char comando = 'S';

	/* FLAG per controllare i loop di gioco */
	
	// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma

	bool uscita_programma = false;

	// uscita_opzioni_menu/gioco : se è 'true', esce dal ciclo delle opzioni (del menù principale/di gioco)
	/* TO DO: magari è meglio metterla e inizializzarla *solo* all'interno del loop delle opzioni */

	bool uscita_opzioni_menu = false;
	bool uscita_opzioni_gioco = false;

	// uscita_opzioni : se è 'false', esce dal ciclo del gioco per tornare al menù principale oppure per uscire dal programma
	bool in_gioco = false;

	// mossa_lecita : finché l'input fornito dal giocatore per la mossa NON è in uno dei formati corretti, questa flag non viene posta come true
	bool mossa_lecita = false;

	// campo_rivelato : se è 'true', non permette al giocatore di visualizzare ancora la tabella o rigiocare la stessa tabella.

	bool campo_rivelato = false;

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
		
		while (!uscita_opzioni_menu)
		{
			int comando_opzioni;

			std::cout << "> ";
			std::cin >> comando_opzioni;
			pulisci_cin();

			switch (comando_opzioni)
			{
			case 1:
				gioco.aggiorna(9, 9, 10);
				uscita_opzioni_menu = true;
				break;
			case 2:
				gioco.aggiorna(16, 16, 40);
				uscita_opzioni_menu = true;
				break;
			case 3:
				gioco.aggiorna(16, 30, 99);
				uscita_opzioni_menu = true;
				break;
			case 4:
				std::cout << "We're no stranger to love..." << std::endl;
				uscita_opzioni_menu = true;
				break;
			case 5:
				std::cout << "Inserire altezza, larghezza e numero di mine:" << std::endl;
				std::cout << "> ";
				std::cin >> altezza >> larghezza >> mine;
				pulisci_cin();
				gioco.aggiorna(altezza, larghezza, mine);
				uscita_opzioni_menu = true;
				break;
			case 6:
				uscita_opzioni_menu = true;
				uscita_programma = true;
				break;
			case 42:
				std::cout << u8"\"La Vita, l'Universo, e il Tutto. C'è una risposta. Ma ci devo pensare.\"" << std::endl;
				gioco.aggiorna(42, 42, 420);
				uscita_opzioni_menu = true;
				break;
			default:
				std::cout	<< u8"Lo vedi che ci sono solo numeri dall'1 al 6, vero? Che diamine scrivi "
							<< comando_opzioni
							<< u8" quando chiaramente\nnon ti mostrerà nulla di importante?"
							<< u8" Piuttosto che perdere tempo così, potresti chiederti\n"
							<< u8"se c'è una risposta alla Vita, all'Universo, al tutto." << std::endl;
				break;
			}
		}
			
	uscita_opzioni_menu = false;
	in_gioco = true;

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			while (gioco._status() == '-')
			{
				std::cout << gioco._campo_giocatore() << std::endl;
				std::cout << u8"Inserisci una mossa nel formato 'x y comando' oppure anche solo 'x y' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt 'O'." << std::endl;
				std::cout << "Hai messo " << gioco._numero_bandiere() << " bandiere su " << gioco._mine() << " mine presenti." << std::endl;
				while (!mossa_lecita)
				{
					std::string input_gioco;
					std::cout << "> "; 
					std::getline(std::cin, input_gioco);

					/* INTERPRETAZIONE DELL'INPUT */
					// Prende la stringa (ergo, le coordinate e l'azione) data in input e provvede a spezzarla in un vettore di n parole
					std::vector<std::string> v = separa_spazi(input_gioco);

					switch (v.size())
					{
					case 1:
						if (v[0].size() > 1 || std::toupper(v[0][0]) != 'O') break; // throw std::domain_error("comando non valido");
						
						comando = 'O';

						mossa_lecita = true;
						break;
					case 2:
						if (!solo_numeri(v[0]) || !solo_numeri(v[1])) break;																				//throw std::domain_error("comando non valido");
						if (!gioco._campo_giocatore().nel_campo(std::stoi(v[0]) - 1, std::stoi(v[1]) - 1)) break;	//throw std::domain_error("coordinate non valide");
						

						x = std::stoi(v[0]);
						y = std::stoi(v[1]);
						comando = 'S';
						
						mossa_lecita = true;
						break;
					case 3:
						if (!gioco._campo_giocatore().nel_campo(std::stoi(v[0])-1, std::stoi(v[1]) - 1)) break;		// throw std::domain_error("coordinate non valide");
						if (v[2].size() > 1 || !comando_lecito(std::toupper(v[2][0]))) break;						// throw std::domain_error("comando non valido");
						
						x = std::stoi(v[0]);
						y = std::stoi(v[1]);
						comando = std::toupper(v[2][0]);
						
						mossa_lecita = true;
						break;
					default:
						break;
					}

					if (!mossa_lecita)
					{
						std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
					}
					
				}
				
				mossa_lecita = false;

				// Esce dalla partita per le opzioni
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima della partita (eccetto il caso in cui si ricominci) il campo
				// viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!prima_mossa_effettuata)
				{
					gioco.randomizza_campo(x - 1, y - 1);
					prima_mossa_effettuata = true;
				}
				gioco.gioca(x - 1, y - 1, comando);

				/* TO DO: fare in modo che si vinca se sono state scavate tutte le celle non minate*/
				// controllo della vittoria
				if (gioco._campo_giocatore().conta_tutti_nulli() == gioco._mine())
				{
					gioco.vittoria();
				}
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
			std::cout << "> ";

			while (!uscita_opzioni_gioco)
			{
				int comando_opzioni;

				std::cin >> comando_opzioni;
				pulisci_cin();

				switch (comando_opzioni)
				{
				case 1:
					uscita_opzioni_gioco = true;
					break;
				case 2:
					if (!campo_rivelato)
					{
						gioco.reset_giocatore();
						gioco.reset_numero_bandiere();
					}
					uscita_opzioni_gioco = true;
					break;
				case 3:
					if (!campo_rivelato)
					{
						std::cout << gioco.rivela() << std::endl;

						campo_rivelato = true;

						std::cout << "OPZIONI:\n"
									<< u8"• Torna al menù principale.\t (4)\n"
									<< u8"• Esci dal gioco.\t\t (5)" << std::endl;
						std::cout << "> ";
					}
					break;
				case 4:
					gioco.reset();

					uscita_opzioni_gioco = true;
					in_gioco = false;
					break;
				case 5:
					uscita_opzioni_gioco = true;
					in_gioco = false;
					uscita_programma = true;
					break;
				default:
					std::cout << "Comando non riconosciuto o lecito. Riprova!" << std::endl;
					std::cout << "> ";
					break;
				}
			}
			uscita_opzioni_gioco = false;
			if (gioco._status() == 'S') break;
		}

		if (gioco._status() == 'S' || gioco._status() == 'V')
		{
			std::cout << gioco._campo_giocatore() << std::endl;
			
			if (gioco._status() == 'S')
			{
				std::cout << u8"OH NO! Hai perso. Cosa vuoi fare ora?\n";
			}
			else
			{
				std::cout << u8"HAI VINTO! Cosa vuoi fare ora?\n";
			}
			std::cout	<< u8"• Torna al menù principale.\t (1)\n"
						<< u8"• Esci dal gioco.\t\t (2)" << std::endl;

			while (!uscita_opzioni_gioco)
			{
				int comando_opzioni;
				
				std::cout << "> ";
				std::cin >> comando_opzioni;
				pulisci_cin();

				switch (comando_opzioni)
				{
				case 1:
					gioco.reset();

					uscita_opzioni_gioco = true;
					break;
				case 2:
					uscita_opzioni_gioco = true;
					uscita_programma = true;
					break;
				default:
					std::cout << "Comando non riconosciuto. Riprova!" << std::endl;
					break;
				}
			}
		}
		uscita_opzioni_gioco = false;
		campo_rivelato = false;
		prima_mossa_effettuata = false;
		in_gioco = true;
	}
}

/* TO DO: MOLTO IMPORTANTE: Capire i limiti del campo. Provando a caso un campo di dimensioni maggiori di 44x43 (suppergiù) con poche mine (1,2) genera il campo,
	ma non permette di scavare senza andare in overflow! Il sospetto è un grosso, gigante problema con la ricorsività della funzione di scavo */


/* VECCHIA VERSIONE DELLA LETTURA INPUT + TENTATIVI*/
/* TO DO: Eliminare non appena abbiamo debuggato e testato per bene il metodo di input nuovo */
//pulisci_cin();

				/*
				std::cin >> x >> y >> comando;
				std::cin.clear();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				std::cout << "What is your name? ";
				std::string name;
				std::getline(std::cin, name);
				std::cout << "Hello " << name << ", nice to meet you.\n";
				*/

				/*do
				{
					std::cout << "Sei qua";
					if (!(std::cin >> comando))
					{
						std::cout << "Comando non riconosciuto o lecito. Riprova!";
						std::cin.clear();
						std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
					};
				}
				while (comando_lecito(std::toupper(comando)));
				std::cout << "Sei quo";
				std::cout << "> ";*/
				//std::cin >> x >> y;