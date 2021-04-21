#include <iostream>		// per input/output
#include <vector>		// classe vector
#include <list>
#include <array>
#include <iomanip>
#include <ctime>

#ifdef _WIN32
#include <windows.h> // needed for Windows u8 compability (use Dejavù Sans Mono on console because there are many unicode chars); use (std::min) and (std::max) otherwise some macro incompatibily will happen
#endif

/*
Definiamo due tavole da gioco: CampoMine, che contiene le mine, e CampoGiocatore, che è quello che verrà stampato e aggiornato date le mosse del giocatore
In entrambi i casi definiamo le due tavole come matrici template nell'implementazione "vettore" di "vettori".
// TO DO: adattare l'implementazione di Padovani dei vettori sparsi per la matrice di mine?
*/

/*
typedef std::pair<bool, char> Cella;

class Cella
{
private:
	bool mina;
	char stato; // n (normale), s (scavato), b (bandiera)
public:
	Cella(bool, char);												// costruttore
	void Cambia_stato();
	friend std::ostream& operator<<(std::ostream&, const Cella&);	// stampa cella su terminale
};

Cella::Cella(bool m = false, char s = 'n')
{
	if (s != 'n' && s != 's' && s != 'b') throw std::domain_error("stato illegittimo");
	mina = m;
	stato = s;
}

std::ostream& operator<<(std::ostream& os, const Cella& c)
{
	os << "(" << c.first << ", " << c.second << ")";
	return os;
}

class VettoreCella
{
private:
	int lunghezza;				// lunghezza > 0
	std::vector<Cella> data;
public:
	VettoreCella(int);														// costruttore
	int dimensione() const { return lunghezza; };
	Cella operator[](int) const;											// leggi elemento di data
	Cella& operator[](int);													// scrivi elemento di data
	friend std::ostream& operator<<(std::ostream&, const VettoreCella&);	// stampa vettore cella su terminale
};

VettoreCella::VettoreCella(int l = 1)
{
	if (l < 1) throw std::domain_error("dimensioni vettore invalide");
	lunghezza = l;
	data.resize(l, Cella(0, 'n'));
}

Cella VettoreCella::operator[](int i) const
{
	return data.at(i);
}

Cella& VettoreCella::operator[](int i)
{
	return data.at(i);
}

std::ostream& operator<<(std::ostream& os, const VettoreCella& c)
{
	for (int i = 0; i < c.dimensione(); i++)
		os << c[i];
	return os;
}
*/

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
	for (int i = 0; i < c.righe; i++)
	{
		for (int j = 0; j < c.colonne; j++)
		{
			os << c[i][j];
		}
		os << std::endl;
	}
	return os;
}

template <typename T, int R, int C>
void Campo<T, R, C>::reset() {
	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			campo[i][j] = T();
		}
	}
}

template <int R, int C>
void campo_casuale(Campo<bool, R, C>& c, int mine)
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
					if (random <= (mine / (R * C) * 100) && c[i][j] != 1)
					{
						c[i][j] = 1;
						k++;
					}
				}
			}
		}
	}
}

template <int R, int C>
void campo_casuale2(Campo<bool, R, C>& c, int mine)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % R;
		int random2 = std::rand() % C;
		if (c[random1][random2] != 1)
		{
			c[random1][random2] = 1;
			k++;
		}
		if (k > mine) break;
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

	/* test per verificare la "casualità" (molto poco da CPS) dei due metodi di generatozione*/
	double mean = 0;
	int kk = 0;
	while (kk <= 20) {
	Campo<bool, 16, 16> c1;
	Campo<bool, 16, 16> c2;
	campo_casuale(c1, 40);
	campo_casuale2(c2, 40);
	std::cout << c1 << std::endl << c2 << std::endl;
	int sum = 0;
	int sum2 = 0;
	for (int i = 0; i < c1._righe(); i++)
	{
		for (int j = 0; j < c1._colonne(); j++)
		{
			if (c1[i][j] == 1) sum++;
			if (c1[i][j] == 1) sum2++;
		}
		mean =+ (double) sum / c1._colonne();
		std::cout << (double) sum/ c1._colonne() << std::endl;
		sum = 0;
	}
	std::cout << (double) mean / c1._righe() << std::endl;
	std::cout << sum2 << std::endl;
	sum2 = 0;
	for (int i = 0; i < c2._righe(); i++)
	{
		for (int j = 0; j < c2._colonne(); j++)
		{
			if (c2[i][j] == 1) sum++;
			if (c2[i][j] == 1) sum2++;
		}
		mean = +(double)sum / c2._colonne();
		std::cout << (double)sum / c2._colonne() << std::endl;
		sum = 0;
	}
	std::cout << (double)mean / c2._righe() << std::endl;
	std::cout << sum2 << std::endl;
	sum2 = 0;
	system("PAUSE");
	kk++;
	system("CLS");
	}
	/*
	int sum;
	std::cout << "\x1B[41mTexting\033[0m\t\t";
	std::cout << u8"";
	std::cout << u8"⚐";
	std::cout << u8"⚑";
	*/

}