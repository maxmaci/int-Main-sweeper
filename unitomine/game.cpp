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
	Campo(int, int);						// costruttore
	int _righe() const { return righe; };	
	int _colonne() const { return colonne; };
	std::vector<T> operator[](int) const;	// leggi riga Campo
	std::vector<T>& operator[](int);		// scrivi riga Campo
	template <typename T> friend std::ostream& operator<<(std::ostream&, const Campo<T>&);
	void resize(int, int, T);
	void reset();
	bool nelCampo(int, int) const;
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

std::ostream& operator<<(std::ostream& os, const Campo<std::string>& campo)
{
	os << std::setw(3);
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << j+1;
	}
	os << std::endl;
	//os << std::setw(2) << std::setfill('-') << std::setw(C) << std::endl;
	for (int i = 0; i < campo._righe(); i++)
	{
		os << i+1 << u8"│";
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
	std::vector<T> v;
	v.resize(larghezza, T());
	campo.resize(altezza, v);
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
bool Campo<T>::nelCampo(int i, int j) const
{
	return i >= 0 && i < righe && j >= 0 && j < colonne;
}

int conta_mine(const Campo<bool>& campo, int i, int j)
{
	if (!campo.nelCampo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo[i][j] == 1) return -1;

	int k = 0;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo.nelCampo(n, m) && campo[n][m] == 1) k++;
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
			if (campo_giocatore.nelCampo(n, m))
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

/* La classe di Gioco */

class Gioco
{
private:
	int altezza;	// altezza > 0
	int larghezza;	// larghezza > 0
	int mine;		// 0 < mine < altezza * larghezza
public:
	void giocare();
	void rivela();
};

void Gioco::giocare()
{
	// ???
}

void Gioco::rivela()
{
	// ???
}

// TO DO: comando che interviene e termina la partita se vede in campo_giocatore un asterisco (metodo di Gioco)

int main()
{

#ifdef _WIN32
	// Set console code page to UTF-8 so console known how to interpret string data
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// Enable buffering to prevent VS from chopping up UTF-8 byte sequences
	setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

	std::srand(std::time(nullptr));
	
	bool rivelato = false;

	int altezza;
	int larghezza;
	int mine;

	int x;
	int y;
	char comando;

	std::cout << "Inserire altezza, larghezza e numero di mine:" << std::endl;
	std::cin >> altezza >> larghezza >> mine;
	
	Campo<bool> c(altezza, larghezza);
	Campo<std::string> c_char(altezza, larghezza);
	campo_casuale(c, mine);
	
	/* TO DO: controllare resize */
	while (true)
	{
		std::cout << c_char << std::endl;
		while (true)
		{
			std::cout << "Fai la tua mossa (x, y, comando).\n Altrimenti, scrivi un numero negativo per le opzioni." << std::endl;
			std::cin >> x;
			if (x < 0) break;
			std::cin >> y >> comando;
			cambia_stato(c_char, c, x-1, y-1, comando);
			std::cout << c_char << std::endl;
#ifdef _WIN32
			system("CLS");
#endif

		}

		std::cout	<< "• Ricominciare la partita?\t (R)\n"
					<< "• Rivelare il campo?\t (V)\n"
					<< "• Iniziarne una nuova?\t (N)\n"
					<< "• Uscire dal gioco?\t (U)" << std::endl;

		do
		{
			std::cin >> comando;
			if (comando == 'V' && rivelato != true)
			{
				std::cout << converti_campo(c);
				rivelato = true;

				std::cout	<< "• Iniziarne una nuova?\t (N)\n"
							<< "• Uscire dal gioco?\t (U)" << std::endl;
			}
			else if (comando == 'R' && rivelato != true)
			{
				c_char.reset();
				break;
			}
			else if (comando == 'N')
			{
				std::cout << "Inserire altezza, larghezza e numero di mine:" << std::endl;
				std::cin >> altezza >> larghezza >> mine;

				c.reset();
				c_char.reset();
				c.resize(altezza, larghezza, 0);
				c_char.resize(altezza, larghezza, u8"⎕");
				std::cout << c_char << std::endl;
				campo_casuale(c, mine);
				std::cout << c << std::endl;
				break;
			}
			else if (comando == 'U')
			{
				break;
			}
			else
			{
				std::cout << "Comando non riconosciuto. Riprova!" << std::endl;
			}
		}
		while (true);
		if (comando == 'U') break;
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
	*/
	//Campo<std::string, 9, 9> c_char = converti_campo(c);
	//rgba(1, 0, 254, 255)
	/*
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