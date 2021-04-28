#include <iostream>		// per input/output
#include <vector>		// classe vector
#include <string>
#include <map>
#include <iomanip>
#include <ctime>

#ifdef _WIN32
#include <windows.h> // needed for Windows u8 compability (use Dejavù Sans Mono on console because there are many unicode chars); use (std::min) and (std::max) otherwise some macro incompatibily will happen
#endif

/*
Definiamo due mappe di conversioni, che associano ad un numero il colore
*/

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
	//os << std::setw(3) << std::setfill() << std::setw(campo._colonne()) << std::endl;
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

/*
void scava_celle(Campo<std::string>& campo_giocatore, const Campo<bool>& campo_gioco, int i, int j)
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

					scava_celle(campo_giocatore, campo_gioco, n, m);
				}
			}
		}
	}
	return;
}

void cambia_stato(Campo<std::string>& campo_giocatore, const Campo<bool>& campo_gioco, int i, int j, char comando)
{
	if (comando != 'B' && comando != 'S') throw std::invalid_argument("comando illecito");
	if (comando == 'B')
	{
		if (campo_giocatore[i][j] == u8"⚑")
		{
			campo_giocatore[i][j] = u8"⎕";
			return;
		}
		else if (campo_giocatore[i][j] == u8"⎕")
		{
			campo_giocatore[i][j] = u8"⚑";
			return;
		}
		else return;
	}
	else
	{
		if (campo_gioco[i][j])
		{
			campo_giocatore[i][j] = u8"◉";
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
			scava_celle(campo_giocatore, campo_gioco, i, j);
			return;
		}
	}
}
*/

/* La classe di Gioco */

class Gioco
{
private:
	int altezza;	// altezza > 0
	int larghezza;	// larghezza > 0
	int mine;		// 0 < mine < altezza * larghezza
	Campo<bool> campo_gioco;
	Campo<std::string> campo_giocatore;
public:
	/* COSTRUTTORE */
	Gioco(int, int, int);
	/* LEGGI CAMPI PRIVATE DI CLASS GIOCO */
	int _altezza() { return altezza; };
	int _larghezza() { return larghezza; };
	int _mine() { return mine; };
	Campo<bool> _campo_gioco() { return campo_gioco; };
	Campo<std::string> _campo_giocatore() { return campo_giocatore; };
	/* FUNZIONI DI GIOCO */
	void randomizza_campo();								// randomizza il campo con le mine
	void reset_gioco() { campo_gioco.reset(); };			// pulisce il campo da gioco dalle mine 
	void reset_giocatore() { campo_giocatore.reset(); };	// pulisce il campo dell giocatore allo stato originale
	void reset();											// pulisce il campo da gioco e del giocatore
	void aggiorna(int, int, int);							// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine
	void scava_celle(int, int);								// funzione ricorsiva che 'scava' le celle
	void gioca(int, int, char);								// compie le azioni di gioco
	Campo<std::string> rivela();							// rivela il campo da gioco al giocatore
};

Gioco::Gioco(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_larghezza < 1) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
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
	if (comando != 'B' && comando != 'S') throw std::invalid_argument("comando illecito");
	if (comando == 'B')
	{
		if (campo_giocatore[i][j] == u8"⚑")
		{
			campo_giocatore[i][j] = u8"⎕";
			return;
		}
		else if (campo_giocatore[i][j] == u8"⎕")
		{
			campo_giocatore[i][j] = u8"⚑";
			return;
		}
		else return;
	}
	else
	{
		if (campo_gioco[i][j])
		{
			campo_giocatore[i][j] = u8"◉";
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

void Gioco::reset()
{
	campo_gioco.reset();
	campo_giocatore.reset();
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
	std::cout << "Ci sono anna";
	return res;
}
// TO DO: comando che interviene e termina la partita se vede in campo_giocatore un asterisco (metodo di Gioco)

/* TO DO: trovare un modo per pulire i cin *dopo* la funzione - mi rimangono in input e sputtanano tutto */
/*
template <typename T>
void leggi_input(const Campo<T>& campo) {
	int x;
	int y;
	char commmando;
	std::cout << "Fai la tua mossa nella forma x-y-comando." << std::endl;
	std::cin >> x >> y >> commmando;
	if (std::cin)
	{
		if (!campo.nel_campo(x-1, y-1))
		{
			std::cout << "Errore! Coordinate non lecite." << std::endl;
		}
		if (commmando != 'S' && commmando != 'B')
		{
			std::cout << "Errore! Comando non lecito." << std::endl;
		}
	}
	else
	{
	std::cout << "Errore! Sintassi dell'input non corretta." << std::endl;
	}
	std::cin.clear();
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}
*/

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
	
	// Flag booleana per l'opzione "RIVELA IL CAMPO": se è 'true', non permette (giustamente)
	// al giocatore di visualizzare ancora la tabella o rigiocare la stessa tabella.

	bool rivelato = false;

	// Variabili che descrivono l'impostazione attuale del campo  

	int altezza;
	int larghezza;
	int mine;

	int x;
	int y;
	char comando;
	int comando_opzioni;

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

	std::cout	<< "Seleziona un opzione scrivendo il carattere corrispondendente nel prompt:" << std::endl;
	std::cout	<< u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
				<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
				<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
				<< u8"• EPICO \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
				<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
				<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;
	std::cout	<< "> ";
	std::cin	>> comando_opzioni;

	Gioco gioco(9, 9, 10);
	gioco.aggiorna(16, 30, 99);
	/*
	switch (comando_opzioni)
	{
	case 1:
		gioco.aggiorna(9, 9, 10);
		std::cout << "\7";
	case 2:
		gioco.aggiorna(16, 16, 40);
		std::cout << "\7";
	case 3:
		gioco.aggiorna(16, 30, 99);
		std::cout << "\7";
	case 4:
		gioco.aggiorna(16, 30, 99);
		Beep(750, 100);
	case 5:
		std::cout << "Inserire altezza, larghezza e numero di mine:" << std::endl;
		//std::cin >> altezza >> larghezza >> mine;
	}
	*/

	//Campo<bool> c(altezza, larghezza);
	//Campo<std::string> c_char(altezza, larghezza);
	//campo_casuale(c, mine);

	gioco.randomizza_campo();

#ifdef _WIN32
	system("CLS");
#endif

	/* LOOP PRINCIPALE*/
	// Il gioco e le opzioni sono svolte in questo loop

	while (true)
	{
		/* LOOP DI GIOCO*/
		// A meno di uscire per il menù opzioni, tutte le azioni vengono svolte in questo while true
		while (true)
		{
			std::cout << gioco._campo_giocatore() << std::endl;
			std::cout << "Inserisci una mossa nel formato 'x y comando'.\nAltrimenti, scrivi un numero negativo per le opzioni." << std::endl;
			std::cin >> x >> y >> comando;
			//leggi_input(c);
			//std::cin.clear();
			//std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

#ifdef _WIN32
			system("CLS");
#endif

			if (x < 0) break;
			gioco.gioca(x - 1, y - 1, comando);
		}

		std::cout	<< "OPZIONI:\n"
					<< u8"• Ricomincia la partita?\t(1)\n"
					<< u8"• Rivela il campo?\t\t(2)\n"
					<< u8"• Inizia una nuova?\t\t(3)\n"
					<< u8"• Esci dal gioco?\t\t(4)" << std::endl;

		while (true)
		{
			std::cin >> comando_opzioni;

			if (comando_opzioni == 2 && rivelato != true)
			{
#ifdef _WIN32
				system("CLS");
#endif
				
				std::cout << gioco.rivela() << std::endl;
				rivelato = true;

				std::cout << "OPZIONI:\n"
					<< u8"• Inizia una nuova?\t (3)\n"
					<< u8"• Esci dal gioco?\t (4)" << std::endl;
			}
			else if (comando_opzioni == 1 && rivelato != true)
			{
				gioco.reset_giocatore();
				break;
			}
			else if (comando_opzioni == 3)
			{
				std::cout << "Inserire altezza, larghezza e numero di mine:" << std::endl;
				std::cin >> altezza >> larghezza >> mine;

				gioco.aggiorna(altezza, larghezza, mine);
				gioco.randomizza_campo();
				break;

			}
			else if (comando_opzioni == 3)
			{
				break;
			}
			else
			{
				std::cout << "Comando non riconosciuto. Riprova!" << std::endl;
			}
		}
		if (comando_opzioni == 4) break;
		rivelato = false;
	}

	/*
	Campo<bool, 9, 9> c;
	Campo<std::string, 9, 9> c_char;
	c[0][6] = 1;
	c[1][1] = 1;
	c[2][0] = 1; c[2][5] = 1;
	c[3][6] = 1;
	c[5][5] = 1;
	c[6][3] = 1;
	c[7][4] = 1;
	c[8][5] = 1; c[8][6] = 1;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			c_char[i][j] = u8"⎕";
		}
	}

	Campo<std::string, 9, 9> c_char = converti_campo(c);

	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;1;0;254m1\033[0m" << std::endl;		// 1 - blu chiaro
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;1;127;1m2\033[0m" << std::endl;		// 2 - verde
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;254;0;0m3\033[0m" << std::endl;		// 3 - rosso
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;1;0;128m4\033[0m" << std::endl;		// 4 - blu scuro
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;129;1;2m5\033[0m" << std::endl;		// 5 - amaranto
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;0;128;129m6\033[0m" << std::endl;		// 6 - turchese
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;0;0;0m7\033[0m" << std::endl;			// 7 - nero
	std::cout << "\x1B[48;2;192;192;192m\x1B[38;2;128;128;128m8\033[0m" << std::endl;	// 8 - grigio scurino
	*/
}