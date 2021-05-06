#include <ctime>
#include <set>
#include <algorithm>

#include "campo.h"
#include "menu.h"

/* Classe Risolutore */
class Risolutore
{
private:
	Campo partita;				// ...
	int bandiere_precedenti;	// 0 <= bandiere_precedenti < partita._altezza() * partita._larghezza()		// TO DO: controllare per l'uguale
	int celle_non_scavate_precedenti; // 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()

	/* TYPDEF */
	typedef std::pair<int, int> Coord;

	/* METODI RISOLUTIVI */
	void metodo_meccanico();
	void metodo_gaussiano();
	void metodo_per_assurdo();
	void metodo_probabilistico();

public:
	/* COSTRUTTORE */
	Risolutore(const Campo&);

	void risolve();
};

Risolutore::Risolutore(const Campo& campo)
{
	partita = campo;
	bandiere_precedenti = 0;
	celle_non_scavate_precedenti = 0;
}

void Risolutore::metodo_meccanico()
{
	auto start = std::chrono::steady_clock::now();
	std::cout << "TAVOLA" << std::endl;

	std::cout << partita << std::endl;

	std::cout << "PRIMA FASE" << std::endl;
	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			// PRIMA FASE: mette le bandierine se attorno al numero n trova n celle non scavata (con potenzialmente già delle bandierine
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) + partita.conta_bandiere_vicine(i, j) == partita._campo_visibile()[i][j])
			{
				for (int n = i - 1; n <= i + 1; n++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (partita._campo_visibile().indici_leciti(n, m))
						{
							partita.gioca(n, m, 'B');
							std::cout << partita << std::endl;
						}
					}
				}
			}
			if (partita._status() == 'V')
			{
				std::cout << u8"Il mio lavoro qua è finito\n";
				auto end = std::chrono::steady_clock::now();

				auto diff = end - start;

				std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
				return;
			}
			// SECONDA FASE: scava tutto ciò che non è bandierinato attorno ad una cella numerata con n se sono già presenti esattamente n bandierine attorno
			else if (partita._campo_visibile()[i][j] > 0 && partita.conta_bandiere_vicine(i, j) == partita._campo_visibile()[i][j])
			{
				for (int n = i - 1; n <= i + 1; n++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (partita._campo_visibile().indici_leciti(n, m))
						{
							partita.gioca(n, m, 'S');
						}
					}
				}
				
			}
		}
	}
	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

template <typename T>
bool trova_elemento(const std::vector<T>& vettore, T elemento)
{
	typename std::vector<T>::const_iterator it;
	it = std::find(vettore.cbegin(), vettore.cend(), elemento);
	return it != vettore.end();
}

void Risolutore::metodo_gaussiano()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1: creazione della matrice di incognite
	
	std::vector<Coord> coordinate_numeri_bordo;
	std::vector<int> numeri_bordo;
	std::set<Coord> incognite_bordo;
	std::vector<Coord> incognite_bordo_vettore;

	// recuperiamo tutte i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno:
	// immagazziniamo tutto in un vettore, mentre mettiamo in un insieme (per 

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
			{
				coordinate_numeri_bordo.push_back(Coord(i, j));

				if (partita._campo_visibile().indici_leciti(i - 1, j - 1)	&& partita._campo_visibile()[i - 1][j - 1] == -3)	incognite_bordo.insert(Coord(i - 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i - 1, j)		&& partita._campo_visibile()[i - 1][j] == -3)		incognite_bordo.insert(Coord(i - 1, j));
				if (partita._campo_visibile().indici_leciti(i - 1, j + 1)	&& partita._campo_visibile()[i - 1][j + 1] == -3)	incognite_bordo.insert(Coord(i - 1, j + 1));
				if (partita._campo_visibile().indici_leciti(i, j - 1)		&& partita._campo_visibile()[i][j - 1] == -3)		incognite_bordo.insert(Coord(i, j - 1));
				if (partita._campo_visibile().indici_leciti(i, j + 1)		&& partita._campo_visibile()[i][j + 1] == -3)		incognite_bordo.insert(Coord(i, j + 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j - 1)	&& partita._campo_visibile()[i + 1][j - 1] == -3)	incognite_bordo.insert(Coord(i + 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j)		&& partita._campo_visibile()[i + 1][j] == -3)		incognite_bordo.insert(Coord(i + 1, j));
				if (partita._campo_visibile().indici_leciti(i + 1, j + 1)	&& partita._campo_visibile()[i + 1][j + 1] == -3)	incognite_bordo.insert(Coord(i + 1, j + 1));
			
			}
		}
	}
	
	// convertiamo l'insieme in un vettore per applicare più facilmente il passaggio successivo

	for (std::set<Coord>::const_iterator it = incognite_bordo.cbegin(); it != incognite_bordo.cend(); it++)
	{
		incognite_bordo_vettore.push_back(*it);
	}
	
	std::cout << "ORA CREO LA MATRICE " << coordinate_numeri_bordo.size() << "x" << incognite_bordo_vettore.size() << std::endl;
	
	Matrice<int> matrice(static_cast<int>( coordinate_numeri_bordo.size() ), static_cast<int>( incognite_bordo_vettore.size() ));
	
	for (int n = 0; n < matrice._righe(); n++)
	{
		int i = coordinate_numeri_bordo[n].first;
		int j = coordinate_numeri_bordo[n].second;

		numeri_bordo.push_back(partita._campo_visibile()[i][j] - partita.conta_bandiere_vicine(i,j));
				
		if (trova_elemento(incognite_bordo_vettore, Coord (i - 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord (i - 1, j - 1))]	= 1;
		if (trova_elemento(incognite_bordo_vettore, Coord (i - 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord (i - 1, j))]		= 1;
		if (trova_elemento(incognite_bordo_vettore, Coord (i - 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord (i - 1, j + 1))]	= 1;
		if (trova_elemento(incognite_bordo_vettore, Coord (i, j - 1)))		matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord (i, j - 1))] = 1;
		if (trova_elemento(incognite_bordo_vettore, Coord(i, j + 1)))		matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord(i, j + 1))] = 1;
		if (trova_elemento(incognite_bordo_vettore, Coord(i + 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord(i + 1, j - 1))] = 1;
		if (trova_elemento(incognite_bordo_vettore, Coord(i + 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord(i + 1, j))] = 1;
		if (trova_elemento(incognite_bordo_vettore, Coord(i + 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo_vettore, Coord(i + 1, j + 1))] = 1;
	}

	std::cout << matrice << std::endl;

	// FASE 2: riduzione gaussiana della matrice

	matrice.riduzione_gaussiana(numeri_bordo);

	std::cout << matrice;

	// FASE 3: sostituzione all'indietro.

	//Consideriamo solamente le righe non vuote (il sistema originale non sempre ha soluzioni - anzi, quasi mai!)

	std::cout << "ORA CREO LA MATRICE RIDOTTA " << std::endl;

	Matrice<int> matrice_ridotta(0, matrice._colonne());
	std::vector<int> termine_noto;
	std::vector<Coord> coordinate_numeri_bordo_ridotta;
	for (int i = 0; i < matrice._righe(); i++)
	{
		for (int j = 0; j < matrice._colonne(); j++)
		{
			if (matrice[i][j] != 0)
			{
				matrice_ridotta.push_back(matrice[i]);
				termine_noto.push_back(numeri_bordo[i]);
				coordinate_numeri_bordo_ridotta.push_back(coordinate_numeri_bordo[i]);
				break;
			}
		}
	}

	std::cout << matrice_ridotta._righe() << "x" << matrice_ridotta._colonne() << std::endl;

	for (int i = 0; i < matrice_ridotta._righe(); i++)
	{
		//std::cout << coordinate_numeri_bordo[i].first + 1 << ", " << coordinate_numeri_bordo[i].second + 1 << " : ";
		for (int j = 0; j < matrice_ridotta._colonne(); j++)
		{
			std::cout << matrice_ridotta[i][j];

		}
		std::cout << " = " << termine_noto[i] << std::endl;
	}

	std::cout << std::endl;

	for (int i = matrice_ridotta._righe() - 1; i >= 0; i--)
	{
		int upper_bound = 0;
		int lower_bound = 0;

		for (int j = 0; j < matrice_ridotta._colonne(); j++)
		{
			if (matrice_ridotta[i][j] > 0)	upper_bound += matrice_ridotta[i][j];
			else lower_bound += matrice_ridotta[i][j];

		}

		std::cout << "RIGA " << i << "-esima:" << std::endl;
		std::cout << "UPPER: " << upper_bound << std::endl;
		std::cout << "LOWER: " << lower_bound << std::endl;

		if (upper_bound == termine_noto[i])
		{
			
			for (int j = 0; j < matrice_ridotta._colonne(); j++)
			{
				if (matrice_ridotta[i][j] > 0)
				{
					std::cout << "FLAGGO \"" << incognite_bordo_vettore[j].first + 1 << ", " << incognite_bordo_vettore[j].second + 1 << "\"\n";
					partita.gioca(incognite_bordo_vettore[j].first, incognite_bordo_vettore[j].second, 'B');
				}
				else if (matrice_ridotta[i][j] < 0)
				{
					std::cout << "ESPLODE \"" << incognite_bordo_vettore[j].first + 1 << ", " << incognite_bordo_vettore[j].second + 1<< "\"\n";
					partita.gioca(incognite_bordo_vettore[j].first, incognite_bordo_vettore[j].second, 'S');
				}
			}
		}
		else if (lower_bound == termine_noto[i])
		{
			// tutti i termini con coefficiente -1 sono mine, tutti quelli con coefficiente 1 non lo sono

			for (int j = 0; j < matrice_ridotta._colonne(); j++)
			{
				if (matrice_ridotta[i][j] < 0)
				{
					std::cout << "FLAGGO \"" << incognite_bordo_vettore[j].first + 1 << ", " << incognite_bordo_vettore[j].second + 1 << "\"\n";
					partita.gioca(incognite_bordo_vettore[j].first, incognite_bordo_vettore[j].second, 'B');
				}
				else if (matrice_ridotta[i][j] > 0)
				{
					std::cout << "ESPLODE \"" << incognite_bordo_vettore[j].first + 1 << ", " << incognite_bordo_vettore[j].second + 1 << "\"\n";
					partita.gioca(incognite_bordo_vettore[j].first, incognite_bordo_vettore[j].second, 'S');
				}
			}
		}
		if (i != 0)
		{
			for (int b = i - 1; b > 0; b--)
			{
				for (int a = 0; a < matrice_ridotta._colonne(); a++)
				{
					// TO DO: rendere l'operatore - parte di una specifica classe vettore che funge da base ad una classe matrix adattata - > campo deve diventare solo nel caso della classe Gioco (che potremmo rinominare Campo).
					matrice_ridotta[b][a] -= matrice_ridotta[i][a];
				}
				termine_noto[b] -= termine_noto[i];
			}
		}

	}
	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

void Risolutore::metodo_per_assurdo()
{
	auto start = std::chrono::steady_clock::now();
	
	Risolutore gioco_per_assurdo (partita);
	while (gioco_per_assurdo.partita._status() == 'S')
	{
		gioco_per_assurdo.metodo_meccanico();
		if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-2) && partita._campo_visibile().conta_tutti_elemento(-3))
		gioco_per_assurdo.metodo_gaussiano();
		
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

void Risolutore::risolve()
{
	int riga;
	int colonna;
	std::cout << partita << std::endl;
	std::cout << u8"Inserisci le coordinate (nella forma 'riga colonna') per inizializzare il costruttore." << std::endl;
	inizializza_risolutore(partita, riga, colonna);
	partita.randomizza_campo(riga - 1, colonna - 1);

	partita.gioca(riga - 1, colonna - 1, 'S');
	std::cout << partita << std::endl;
	system("PAUSE");
	
	while (partita._status() == '-')
	{
		std::cout << "Applico il metodo meccanico." << std::endl;

		metodo_meccanico();
		std::cout << partita << std::endl;
		std::cout << "STATUS: " << partita._status() << std::endl;
		system("PAUSE");
		if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
		{
			std::cout << "Non ho messo nuove bandiere nè scavato celle, passo al metodo Gaussiano." << std::endl;
			metodo_gaussiano();
			std::cout << partita << std::endl;
			system("PAUSE");

			if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
			{
				std::cout << "Non ho messo nuove bandiere nè scavato celle, passo al metodo Assurdista." << std::endl;
				// metodo_per_assurdo();
				std::cout << partita << std::endl;
				system("PAUSE");
			}
		}

		bandiere_precedenti = partita._numero_bandiere();
		celle_non_scavate_precedenti = partita._campo_visibile().conta_tutti_elemento(-3);
		
		/*
		if (k == -1)
		{
			partita.reset();
			partita.randomizza_campo(riga - 1, colonna - 1);
			partita.gioca(riga - 1, colonna - 1, 'S');
			std::cin.clear();
			std::cout << partita._campo_giocatore() << std::endl;
			system("PAUSE");
		}
		*/
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

	std::srand(static_cast<int>( std::time(nullptr) ));

	// Variabili per comandi di gioco: coordinate della cella + comando dell'azione

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

	/* LOOP DEL PROGRAMMA: finchè non gli viene dato l'input di uscita del menù tornerà qui */

	// inizializza la tabella di gioco

	while (!uscita_programma)
	{
		Campo partita(9, 9, 10);
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
		
		menu_principale(partita, uscita_programma);
		
		menu_risolutore(in_risolutore, in_gioco);

	/* TO DO: menù per chiedere risolutore */

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			while (partita._status() == '-')
			{
				std::cout << partita << std::endl;
				std::cout << u8"Inserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt 'O'." << std::endl;
				std::cout << "Hai messo " << partita._numero_bandiere() << " bandiere su " << partita._mine() << " mine presenti." << std::endl;
				
				interpreta_mossa(partita, riga, colonna, comando);
				
				// Esce dalla partita per le opzioni
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima (tranne quando si ricomincia la partita), il campo viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!prima_mossa_effettuata)
				{
					partita.randomizza_campo(riga - 1, colonna - 1);
					prima_mossa_effettuata = true;
				}
				
				partita.gioca(riga - 1, colonna - 1, comando);

			}

			// NON entra nelle opzioni se il gioco è stato vinto o perso.

			if (partita._status() == 'S' || partita._status() == 'V')
			{
				break;
			}

			std::cout	<< "OPZIONI:\n"
						<< u8"• Torna al gioco.\t\t(1)\n"
						<< u8"• Ricomincia la partita.\t(2)\n"
						<< u8"• Rivela il campo.\t\t(3)\n"
						<< u8"• Torna al menù principale.\t(4)\n"
						<< u8"• Esci dal gioco.\t\t(5)" << std::endl;
			
			menu_opzioni(partita, uscita_programma, in_risolutore, in_gioco);

		}

		while (!uscita_programma && in_risolutore)
		{
			Risolutore risolutore(partita);
			risolutore.risolve();
			break;
		}

		if (partita._status() == 'S' || partita._status() == 'V')
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

		prima_mossa_effettuata = false;

	}
}