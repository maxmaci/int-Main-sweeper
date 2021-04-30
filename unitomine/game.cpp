#include <iostream>		// per input/output
#include <vector>		// classe vector
#include <string>
#include <map>
#include <iomanip>
#include <ctime>

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// È consigliato usare Dejavù Sans Mono dato che supporta molti caratteri unicode.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max)
#include <windows.h>
#endif

/* MAPPE */

// Definiamo due mappe: la prima associa ad un carattere del campo visibile dal giocatore un numero e ci servirà per il conteggio delle mine 
// (con alcuni valori inutilizzati ma presenti per 'debugging'), la seconda invece associa ad ogni carattere un numero e ci serve per la stampa
// del campo visto dal giocatore

/* TO DO: dove usiamo la mappa converti? possiamo eliminarla */
std::map<std::string, int > mappa_converti
{
	{"-", 0},
	{"1", 1},
	{"2", 2},
	{"3", 3},
	{"4", 4},
	{"5", 5},
	{"6", 6},
	{"7", 7},
	{"8", 8},
	{u8"◉", -1},
	{u8"⚑", -2},
	{u8"⎕", -3},
};

std::map<std::string, std::string > mappa_colori
{
	{"1", "\x1B[38;2;1;0;254m"},		// blu chiaro
	{"2", "\x1B[38;2;1;127;1m"},		// verde
	{"3", "\x1B[38;2;254;0;0m"},		// rosso
	{"4", "\x1B[38;2;1;0;128m"},		// blu scuro
	{"5", "\x1B[38;2;129;1;2m"},		// amaranto
	{"6", "\x1B[38;2;0;128;129m"},		// turchese
	{"7", "\x1B[38;2;0;0;0m"},			// nero
	{"8", "\x1B[38;2;128;128;128m"},	// grigio scurino
	{u8"◉", "\x1B[38;2;0;0;0m"},		// nero	
	{"-", "\x1B[38;2;0;0;0m"},			// nero	
	{u8"⚑", "\x1B[38;2;159;0;1m"},		// rosso scurino	
	{u8"⎕", "\x1B[38;2;0;0;0m"},		// nero	
};

template <typename T>
class Campo
{
private:
	int	righe;								// righe > 0
	int colonne;							// colonne > 0
	std::vector<std::vector<T>> campo;
public:
	Campo(int = 1, int = 1);						// costruttore
	int _righe() const { return righe; };	
	int _colonne() const { return colonne; };
	std::vector<T> operator[](int) const;	// leggi riga Campo
	std::vector<T>& operator[](int);		// scrivi riga Campo
	template <typename T> friend std::ostream& operator<<(std::ostream&, const Campo<T>&);
	void resize(int, int, T);
	void reset();
	bool nel_campo(int, int) const;
};

template <typename T>
Campo<T>::Campo(int numero_righe, int numero_colonne)
{
	if (numero_righe < 1 || numero_colonne < 1) throw std::domain_error("dimensioni campo invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	std::vector<T> v;
	if (std::is_same_v<T, std::string>)
	{
		v.resize(numero_colonne, u8"⎕");
	}
	else
	{
		v.resize(numero_colonne, T());
	}
	campo.resize(numero_righe, v);
}

template <typename T>
std::vector<T> Campo<T>::operator[](int i) const
{
	return campo.at(i);
}

template <typename T>
std::vector<T>& Campo<T>::operator[](int i)
{
	return campo.at(i);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Campo<T>& campo)
{
	for (int i = 0; i < campo._righe(); i++)
	{
		for (int j = 0; j < campo._colonne(); j++)
		{
			os << campo[i][j];
		}
		os << std::endl;
	}
	return os;
}

int cifre(int i)
{
	i = std::abs(i);
	int cifre = 1;
	while (i > 9) {
		i = i / 10;
		cifre++;
	}
	return cifre;
}


std::ostream& operator<<(std::ostream& os, const Campo<std::string>& campo)
{
	os << std::setw(cifre(campo._righe() - 1)) << std::setfill(' ') << " ";
	for (int j = 0; j < campo._colonne() + 1; j++)
	{
		if (j > 9)
		{
			os << (j / 10) % 10;
		}
		else
		{
			os << " ";
		}
	}
	os << std::endl;
	os << " " << " " << std::setw(cifre(campo._righe() - 1)) << std::setfill(' ');
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << (j + 1) % 10;
	}
	os << std::endl;
	os << std::right << std::setw(cifre(campo._righe()-1)) << std::setfill(' ') << " " << u8"┌";
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << u8"─";
	}
	os << std::endl;
	for (int i = 0; i < campo._righe(); i++)
	{
		os << std::right << std::setw(cifre(campo._righe() - 1)) << std::setfill(' ') << i + 1 << u8"│";
		os << "\x1B[48;2;192;192;192m";
		for (int j = 0; j < campo._colonne(); j++)
		{
			os << mappa_colori[campo[i][j]] << campo[i][j];
		}
		os << "\x1B[0m" << std::endl;
	}
	return os;
}

template <typename T>
void Campo<T>::reset() {
	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			if (std::is_same_v<T, std::string>)
			{
				campo[i][j] = u8"⎕";
			}
			else
			{
				campo[i][j] = T();
			}
		}
	}
}

template <typename T>
void Campo<T>::resize(int altezza, int larghezza, T elemento) {
	righe = altezza;
	colonne = larghezza;
	campo.resize(altezza, std::vector<T>(larghezza, elemento));
	for (int i = 0; i < altezza; i++)
	{
		campo[i].resize(larghezza, elemento);
	}
}

void campo_casuale2(Campo<bool>& campo, int mine)
{
	int k = 1;
	while (k <= mine)
	{
		for (int i = 0; i < campo._righe(); i++)
		{
			for (int j = 0; j < campo._colonne(); j++)
			{
				if (k <= mine)
				{
					int random = std::rand() % 99;
					if (random <= (mine / (campo._righe() * campo._colonne()) * 100) && campo[i][j] != 1)
					{
						campo[i][j] = 1;
						k++;
					}
				}
			}
		}
	}
}

/* TO DO: scegliere quale versione di campo casuale tenere*/
/* TO DO: far sì che il campo casuale venga generato DOPO il primo 'click'*/

void campo_casuale(Campo<bool>& campo, int mine)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % campo._righe();
		int random2 = std::rand() % campo._colonne();
		if (campo[random1][random2] != 1)
		{
			campo[random1][random2] = 1;
			k++;
		}
		if (k > mine) break;
	}
}

template <typename T>
bool Campo<T>::nel_campo(int i, int j) const
{
	return i >= 0 && i < righe && j >= 0 && j < colonne;
}

int conta_mine(const Campo<bool>& campo, int i, int j)
{
	if (!campo.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo[i][j] == 1) return -1;

	int k = 0;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo.nel_campo(n, m) && campo[n][m] == 1) k++;
		}
	}
	return k;
}

Campo<std::string> converti_campo(const Campo<bool>& campo)
{
	Campo<std::string> res(campo._righe(), campo._colonne());
	for (int i = 0; i < campo._righe(); i++)
	{
		for (int j = 0; j < campo._colonne(); j++)
		{
			if (conta_mine(campo, i, j) == -1) res[i][j] = u8"◉";
			else if (conta_mine(campo, i, j) == 0) res[i][j] = "-";
			else res[i][j] = std::to_string(conta_mine(campo, i, j));
		}
	}
	return res;
}

/* La classe di Gioco */

class Gioco
{
private:
	int altezza;						// altezza > 0
	int larghezza;						// larghezza > 0
	int mine;							// 0 < mine < altezza * larghezza
	int numero_bandiere;				// 0 <= numero_bandiere < altezza * larghezza
	char status;						// '-': nè persa, nè vinta; 'S': sconfitta; 'V': vittoria. 
	Campo<bool> campo_gioco;
	Campo<std::string> campo_giocatore;
public:
	/* COSTRUTTORE */
	Gioco(int, int, int);

	/* LEGGI CAMPI PRIVATE DI CLASS GIOCO */
	int _altezza() { return altezza; };
	int _larghezza() { return larghezza; };
	int _mine() { return mine; };
	int _numero_bandiere() { return numero_bandiere; };
	char _status() { return status; };
	Campo<bool> _campo_gioco() { return campo_gioco; };
	Campo<std::string> _campo_giocatore() { return campo_giocatore; };

	/* FUNZIONI DI GENERAZIONE DEL CAMPO */
	void randomizza_campo();								// randomizza il campo con le mine 
	void randomizza_campo(int, int);

	/* FUNZIONI DI RESET */
	void reset_gioco() { campo_gioco.reset(); };			// pulisce il campo da gioco dalle mine
	void reset_giocatore() { campo_giocatore.reset(); };	// pulisce il campo dell giocatore allo stato originale
	void reset_status() { status = '-'; };					// resetta lo status
	void reset_numero_bandiere() { numero_bandiere = 0; };	// resetta il numero di bandiere segnate
	void reset();											// pulisce il campo da gioco e del giocatore, resetta lo status a '-'
	
	/* FUNZIONI DI GIOCO */
	void aggiorna(int, int, int);							// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine
	void scava_celle(int, int);								// funzione ricorsiva che 'scava' le celle
	void gioca(int, int, char);								// compie le azioni di gioco

	/* FUNZIONI DI STATUS */
	Campo<std::string> rivela();							// rivela il campo da gioco al giocatore nelle opzioni
	void sconfitta(int, int);								// visualizzando il campo con le mine, imposta lo status a S(confitta)
	void vittoria();										// controlla se le mine sono state tutte segnate con la bandierina. Se è così, imposta lo status a V(ittoria)
};

Gioco::Gioco(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_larghezza < 1) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	numero_bandiere = 0;
	status = '-';
	campo_gioco = Campo<bool>(altezza, larghezza);
	campo_giocatore = Campo<std::string>(altezza, larghezza);
}

void Gioco::scava_celle(int i, int j)
{
	if (campo_giocatore[i][j] != "-")
{
	return;
}

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo_giocatore.nel_campo(n, m))
			{
				if (campo_giocatore[n][m] == u8"⚑" || campo_gioco[n][m])
				{
					break;
				}
				else if (campo_giocatore[n][m] == u8"⎕")
				{
					if (conta_mine(campo_gioco, n, m) == 0) campo_giocatore[n][m] = "-";
					else campo_giocatore[n][m] = std::to_string(conta_mine(campo_gioco, n, m));

					scava_celle(n, m);
				}
			}
		}
	}
	return;
}

void Gioco::gioca(int i, int j, char comando)
{
	if (comando != 'B' && comando != 'T' && comando != 'S') throw std::invalid_argument("comando illecito");
	if (comando == 'B')
	{
		if (campo_giocatore[i][j] == u8"⎕")
		{
			campo_giocatore[i][j] = u8"⚑";
			numero_bandiere++;
			return;
		}
		else return;
	}
	else if (comando == 'T')
	{
		if (campo_giocatore[i][j] == u8"⚑")
		{
			campo_giocatore[i][j] = u8"⎕";
			numero_bandiere--;
			return;
		}
		else return;
	}
	else
	{
		if (campo_gioco[i][j])
		{
			sconfitta(i, j);
			return;
		}
		else if (campo_giocatore[i][j] == u8"⚑")
		{
			return;
		}
		else
		{
			if (conta_mine(campo_gioco, i, j) == 0) campo_giocatore[i][j] = "-";
			else campo_giocatore[i][j] = std::to_string(conta_mine(campo_gioco, i, j));
			scava_celle(i, j);
			return;
		}
	}
}

void Gioco::randomizza_campo()
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % campo_gioco._righe();
		int random2 = std::rand() % campo_gioco._colonne();
		if (campo_gioco[random1][random2] != 1)
		{
			campo_gioco[random1][random2] = 1;
			k++;
		}
		if (k > mine) break;
	}
}

void Gioco::randomizza_campo(int i, int j)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % campo_gioco._righe();
		int random2 = std::rand() % campo_gioco._colonne();
		if (((random1 < i-1 || random1 > i+1) || (random2 < j - 1 || random2 > j + 1)) && campo_gioco[random1][random2] != 1)
		{
			campo_gioco[random1][random2] = 1;
			k++;
		}
		if (k > mine) break;
	}
}

void Gioco::reset()
{
	reset_gioco();
	reset_giocatore();
	reset_status();
	reset_numero_bandiere();
}

void Gioco::aggiorna(int altezza_input, int larghezza_input, int mine_input)
{
	altezza = altezza_input;
	larghezza = larghezza_input;
	mine = mine_input;
	campo_gioco.resize(altezza, larghezza, 0);
	campo_giocatore.resize(altezza, larghezza, u8"⎕");
}

Campo<std::string> Gioco::rivela()
{
	Campo<std::string> res(altezza, larghezza);
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if (conta_mine(campo_gioco, i, j) == -1) res[i][j] = u8"◉";
			else if (conta_mine(campo_gioco, i, j) == 0) res[i][j] = "-";
			else res[i][j] = std::to_string(conta_mine(campo_gioco, i, j));
		}
	}
	return res;
}

void Gioco::sconfitta(int x, int y)
{
	campo_giocatore[x][y] = u8"\x1B[48;2;159;0;1m\x1B[38;2;255;255;255m◉\x1B[48;2;192;192;192m";
	status = 'S';
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if ((i != x || j != y) && conta_mine(campo_gioco, i, j) == -1) campo_giocatore[i][j] = u8"◉";
		}
	}
}

void Gioco::vittoria()
{
	int mine_segnalate = 0;
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if (campo_giocatore[i][j] == u8"⚑" && campo_gioco[i][j] == 1) mine_segnalate++;
		}
	}
	if (mine_segnalate == mine)
	{
		status = 'V';
	}
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

	int altezza;
	int larghezza;
	int mine;

	// Variabili per comandi di gioco: coordinate della cella + comando dell'azione

	int x;
	int y;
	char comando;

	// Variabili di controllo dei menù di gioco

	int comando_opzioni;

	/* FLAG per controllare i loop di gioco */
	
	// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma

	bool uscita_programma = false;

	// uscita_opzioni_menu/gioco : se è 'true', esce dal ciclo delle opzioni (del menù principale/di gioco)
	/* TO DO: magari è meglio metterla e inizializzarla *solo* all'interno del loop delle opzioni */

	bool uscita_opzioni_menu = false;
	bool uscita_opzioni_gioco = false;

	// uscita_opzioni : se è 'false', esce dal ciclo del gioco per tornare al menù principale oppure per uscire dal programma
	bool in_gioco = false;

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

		std::cout << "Seleziona un opzione scrivendo il numero corrispondendente nel prompt:" << std::endl;
		std::cout	<< u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
					<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
					<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
					<< u8"• EPIC MODE \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
					<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
					<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;
		
		while (!uscita_opzioni_menu)
		{
			std::cout << "> ";
			std::cin >> comando_opzioni;

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
				std::cin >> altezza >> larghezza >> mine;
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
				std::cout << "Inserisci una mossa nel formato 'x y comando'.\nAltrimenti, scrivi un numero negativo per le opzioni." << std::endl;
				std::cout << "> ";
				std::cin >> x >> y >> comando;
				//std::cin.clear();
				//std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

				if (x < 0) break;
				if (!prima_mossa_effettuata)
				{
					gioco.randomizza_campo(x - 1, y - 1);
					prima_mossa_effettuata = true;
				}
				gioco.gioca(x - 1, y - 1, comando);
				if (gioco._numero_bandiere() == gioco._mine())
				{
					gioco.vittoria();
				}
			}
			if (gioco._status() == 'S' || gioco._status() == 'V')
			{
				break;
			}

			std::cout	<< "OPZIONI:\n"
						<< u8"• Torna al gioco.\t\t(1)\n"
						<< u8"• Ricomincia la partita.\t(2)\n"
						<< u8"• Rivela il campo.\t\t(3)\n"
						<< u8"• Torna al menù principale.\t(4)\n"
						<< u8"• Esci dal gioco.\t\t(5)" << std::endl;
			std::cout << "> ";

			while (!uscita_opzioni_gioco)
			{
			std::cin >> comando_opzioni;

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
				std::cout << "Comando non riconosciuto. Riprova!" << std::endl;
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
			std::cout << "> ";
			while (!uscita_opzioni_gioco)
			{
				std::cin >> comando_opzioni;

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