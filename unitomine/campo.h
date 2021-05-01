#ifndef __CAMPO_H__
#define __CAMPO_H__

#include <iostream>		// std::cout / std::cin
#include <string>		// classe string
#include <vector>		// classe vector
#include <map>			// classe map
#include <iomanip>		// std::setw() / std::setfill()

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// È consigliato usare Dejavù Sans Mono dato che supporta molti caratteri unicode.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max).
#include <windows.h>
#endif

/* MAPPE */

// Definiamo due mappe: la prima associa ad un carattere del campo visibile dal giocatore un numero e ci servirà per il conteggio delle mine 
// (con alcuni valori inutilizzati ma presenti per 'debugging'), la seconda invece associa ad ogni carattere un numero e ci serve per la stampa
// del campo visto dal giocatore

/* TO DO: dove usiamo la mappa converti? possiamo eliminarla? */
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
	/* COSTRUTTORE */
	Campo(int = 1, int = 1);						// costruttore

	/* LETTORE NUMERO DI RIGHE/COLONNE */
	int _righe() const { return righe; };
	int _colonne() const { return colonne; };

	/* LETTURA/SCRITTURA DELLE RIGHE */
	std::vector<T> operator[](int) const;					// leggi riga Campo
	std::vector<T>& operator[](int);						// scrivi riga Campo

	/* OVERLOADING OPERATORE >> (GENERICO, USATO QUASI ESCLUSIVAMENTE PER T = BOOL - VERSIONE PER T = STD::STRING SOTTO?) */
	/* TO DO: mettere in public l'altro overloading (se possibile) */
	template <typename T> friend std::ostream& operator<<(std::ostream&, const Campo<T>&);

	/* METODI PER MODIFICARE IL CAMPO (DIMENSIONI, RESET) */
	void resize(int, int, T);
	void reset();

	/* METODI PER INFORMAZIONI SUL CAMPO */
	bool nel_campo(int, int) const;
	bool is_nullo(int, int) const;
	int conta_tutti_nulli() const;
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

/* TO DO: mettere a posto il printing con n = 10 (manca una colonna vuota)*/

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
	os << std::right << std::setw(cifre(campo._righe() - 1)) << std::setfill(' ') << " " << u8"┌";
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

/* TO DO: scegliere quale versione di campo casuale tenere */
/* TO DO: rimuovere le versioni di campo casuali non in classe */

/*
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
*/

template <typename T>
bool Campo<T>::nel_campo(int i, int j) const
{
	return i >= 0 && i < righe&& j >= 0 && j < colonne;
}

template <typename T>
bool Campo<T>::is_nullo(int i, int j) const
{
	return campo[i][j] == T();
}

template <>
bool Campo<std::string>::is_nullo(int i, int j) const
{
	return campo[i][j] == u8"⎕" || campo[i][j] == u8"⚑";
}

template <typename T>
int Campo<T>::conta_tutti_nulli() const
{
	int k = 0;

	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			if (is_nullo(i, j)) k++;
		}
	}
	return k;
}

template <>
int Campo<std::string>::conta_tutti_nulli() const
{
	int k = 0;

	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			if (is_nullo(i, j)) k++;
		}
	}
	return k;
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

bool ha_vicini_nulli(const Campo<std::string>& campo, int i, int j)
{
	if (!campo.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo[i][j] == u8"-") return true;

	// controlli
	if (campo.nel_campo(i - 1, j - 1) && campo[i - 1][j] == u8"-") return true;
	if (campo.nel_campo(i - 1, j) && campo[i][j + 1] == u8"-") return true;
	if (campo.nel_campo(i - 1, j + 1) && campo[i][j + 1] == u8"-") return true;
	if (campo.nel_campo(i, j - 1) && campo[i][j - 1] == u8"-") return true;
	if (campo.nel_campo(i, j + 1) && campo[i][j + 1] == u8"-") return true;
	if (campo.nel_campo(i + 1, j - 1) && campo[i + 1][j] == u8"-") return true;
	if (campo.nel_campo(i + 1, j) && campo[i + 1][j] == u8"-") return true;
	if (campo.nel_campo(i + 1, j + 1) && campo[i + 1][j] == u8"-") return true;
	return false;
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

/* TO DO: spostarlo in posizione migliore? */

bool comando_lecito(char comando)
{
	return comando == 'B' || comando == 'T' || comando == 'S';
}

#endif // __CAMPO_H__