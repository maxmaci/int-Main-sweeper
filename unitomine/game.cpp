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
Definiamo due tavole da gioco: CampoMine, che contiene le mine, e CampoGiocatore, che è quello che verrà stampato e aggiornato date le mosse del giocatore
In entrambi i casi definiamo le due tavole come matrici template nell'implementazione "vettore" di "vettori".
*/

std::map<std::string, int > mappa_converti
{
	{"1", 1},
	{"2", 2},
	{"3", 3},
	{"4", 4},
	{"5", 5},
	{"6", 6},
	{"7", 7},
	{"8", 8},
	{u8"◉", -1},
	{"-", 0},
	{u8"⚑", -2},
	{u8"⎕", -3},
};

std::map<std::string, std::string > mappa_colori
{
	{"1", "\x1B[38;2;1;0;254m"},
	{"2", "\x1B[38;2;1;127;1m"},
	{"3", "\x1B[38;2;254;0;0m"},
	{"4", "\x1B[38;2;1;0;128m"},
	{"5", "\x1B[38;2;129;1;2m"},
	{"6", "\x1B[38;2;0;128;129m"},
	{"7", "\x1B[38;2;0;0;0m"},
	{"8", "\x1B[38;2;128;128;128m"},
	{u8"◉", "\x1B[38;2;0;0;0m"},
	{"-", "\x1B[38;2;0;0;0m"},
	{u8"⚑", "\x1B[38;2;159;0;1m"},
	{u8"⎕", "\x1B[38;2;0;0;0m"},
};

template <typename T, int R, int C>
class Campo
{
private:
	int	righe;								// righe > 0
	int colonne;							// colonne > 0
	std::vector<std::vector<T>> campo;
public:
	Campo();								// costruttore
	int _righe() const { return righe; };	
	int _colonne() const { return colonne; };
	std::vector<T> operator[](int) const;	// leggi riga Campo
	std::vector<T>& operator[](int);		// scrivi riga Campo
	template <typename T, int R, int C> friend std::ostream& operator<<(std::ostream&, const Campo<T, R, C>&);
	void reset();
	bool nelCampo(int, int) const;
};

template <typename T, int R, int C>
Campo<T, R, C>::Campo()
{
	if (R < 1 || C < 1) throw std::domain_error("dimensioni campo invalide");
	righe = R;
	colonne = C;
	std::vector<T> v;
	v.resize(C, T());
	campo.resize(R, v);
}

template <typename T, int R, int C>
std::vector<T> Campo<T, R, C>::operator[](int i) const
{
	return campo.at(i);
}

template <typename T, int R, int C>
std::vector<T>& Campo<T, R, C>::operator[](int i)
{
	return campo.at(i);
}

template <typename T, int R, int C>
std::ostream& operator<<(std::ostream& os, const Campo<T, R, C>& c)
{
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			os << c[i][j];
		}
		os << std::endl;
	}
	return os;
}

template <int R, int C>
std::ostream& operator<<(std::ostream& os, const Campo<std::string, R, C>& c)
{
	os << std::setw(3);
	for (int j = 0; j < C; j++)
	{
		os << j+1;
	}
	os << std::endl;
	//os << std::setw(2) << std::setfill('-') << std::setw(C) << std::endl;
	for (int i = 0; i < R; i++)
	{
		os << i+1 << u8"│";
		os << "\x1B[48;2;192;192;192m";
		for (int j = 0; j < C; j++)
		{
			os << mappa_colori[c[i][j]] << c[i][j];
		}
		os << "\x1B[0m" << std::endl;
	}
	return os;
}

template <typename T, int R, int C>
void Campo<T, R, C>::reset() {
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

template <int R, int C>
void campo_casuale(Campo<bool, R, C>& campo, int mine)
{
	int k = 1;
	while (k <= mine)
	{
		for (int i = 0; i < R; i++)
		{
			for (int j = 0; j < C; j++)
			{
				if (k <= mine)
				{
					int random = std::rand() % 99;
					if (random <= (mine / (R * C) * 100) && campo[i][j] != 1)
					{
						campo[i][j] = 1;
						k++;
					}
				}
			}
		}
	}
}

template <int R, int C>
void campo_casuale2(Campo<bool, R, C>& campo, int mine)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % R;
		int random2 = std::rand() % C;
		if (campo[random1][random2] != 1)
		{
			campo[random1][random2] = 1;
			k++;
		}
		if (k > mine) break;
	}
}

template <typename T, int R, int C>
bool Campo<T, R, C>::nelCampo(int i, int j) const
{
	return i >= 0 && i < R && j >= 0 && j < C;
}

template <int R, int C>
int ContaMine(const Campo<bool, R, C>& campo, int i, int j)
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

template <int R, int C>
Campo<std::string, R, C> ConvertiCampo(const Campo<bool, R, C>& campo)
{
	Campo<std::string, R, C> res;
	for (int i = 0; i < R; i++)
	{
		for (int j = 0; j < C; j++)
		{
			if (ContaMine(campo, i, j) == -1) res[i][j] = u8"◉";
			else if (ContaMine(campo, i, j) == 0) res[i][j] = "-";
			else res[i][j] = std::to_string(ContaMine(campo, i, j));
		}
	}
	return res;
}

template <int R, int C>
void cambia_stato(Campo<std::string, R, C>& campo_giocatore, const Campo<bool, R, C>& campo_gioco, int i, int j, char comando)
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
			if (ContaMine(campo_gioco, i, j) == 0) campo_giocatore[i][j] = "-";
			else campo_giocatore[i][j] = std::to_string(ContaMine(campo_gioco, i, j));
			//if (mappa_converti[campo_giocatore[i][j]] > 0) {
			//	return;
			//}
			scava_celle(campo_giocatore, campo_gioco, i, j);
			return;
		}
	}
}

template <int R, int C>
void scava_celle(Campo<std::string, R, C>& campo_giocatore, const Campo<bool, R, C>& campo_gioco, int i, int j)
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
					if (ContaMine(campo_gioco, n, m) == 0) campo_giocatore[n][m] = "-";
					else campo_giocatore[n][m] = std::to_string(ContaMine(campo_gioco, n, m));

					scava_celle(campo_giocatore, campo_gioco, n, m);
				}
			}
		}
	}
	return;
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

// TO DO: comando che interviene e termina la partita se vede in campo_giocatore un asterisco(metodo di Gioco)

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
	//std::cout << c << std::endl;
	//std::cout << ConvertiCampo(c) << std::endl;
	//std::cout << c_char << std::endl;
	int x;
	int y;
	char comando;
	while (true)
	{
		std::cout << c_char << std::endl;
		while (true)
		{
			std::cout << "Fai la tua mossa (x, y, infine comando), altrimenti scrivi un numero negativo per uscire o resettare." << std::endl;
			std::cin >> x;
			if (x < 0) break;
			std::cin >> y;
			std::cin >> comando;
			cambia_stato(c_char, c, x-1, y-1, comando);
			std::cout << c_char << std::endl;
		}
		std::cout << "Vuoi ricominciare la stessa partita (scrivi R) o uscire dal gioco (scrivi U)?\a" << std::endl;
		do
		{
			std::cin >> comando;
			if (comando != 'R' && comando != 'U') {
				std::cout << "Comando non conosciuto, riprova!" << std::endl;
			}
		} while (comando != 'R' && comando != 'U');
		if (comando == 'U') break;
		c_char.reset();
	}
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