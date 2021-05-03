#ifndef __CAMPO_H__
#define __CAMPO_H__

#include <iostream>		// std::cout / std::cin
#include <string>		// classe string
#include <vector>		// classe vector
#include <map>			// classe map
#include <iomanip>		// std::setw() / std::setfill()
#include <chrono>

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// È consigliato usare Dejavù Sans Mono dato che supporta molti caratteri unicode.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max).
#include <windows.h>
#endif

/* MAPPE */

// Definiamo una mappa che associa ad ogni carattere un numero, ci serve per la stampa del campo visto dal giocatore.

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
	{u8"✱", "\x1B[38;2;0;0;0m"},		// nero	
	{"-", "\x1B[38;2;0;0;0m"},			// nero	
	{u8"⚑", "\x1B[38;2;159;0;1m"},		// rosso scurino	
	{u8"⎕", "\x1B[38;2;0;0;0m"},		// nero	
};

template <typename T>
class Campo
{
private:
	/* CAMPI */
	int	righe;								// 0 < righe < 100
	int colonne;							// 0 < colonne < 100
	std::vector<std::vector<T> > campo;

	/* INIZIALIZZATORI DEI COSTRUTTORI */
	void init(int, int, T);
public:
	/* COSTRUTTORI */
	Campo(int, int);														// costruttore con valore di default delle celle pari a T()
	Campo(int, int, T);														// costruttore con valore di default delle celle dato in input iniziale

	/* LETTORE NUMERO DI RIGHE/COLONNE */
	int _righe() const						{ return righe; };				// legge la riga del campo
	int _colonne() const					{ return colonne; };

	/* LETTURA/SCRITTURA DELLE RIGHE */
	std::vector<T> operator[](int i) const	{ return campo.at(i); };		// legge la riga i-esima del campo
	std::vector<T>& operator[](int i)		{ return campo.at(i); };		// scrive la riga del campo

	/* METODI PER MODIFICARE IL CAMPO (DIMENSIONI, RESET) */
	void resize(int, int, T);
	void reset(T = T());

	/* METODI PER INFORMAZIONI SUL CAMPO */
	bool nel_campo(int, int) const;
	bool is_nullo(int, int) const;
	int conta_nulli() const;
};

template <typename T>
void Campo<T>::init(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 1  || numero_righe > 99 || numero_colonne < 1 || numero_colonne > 99) throw std::domain_error("dimensioni del campo invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	campo.resize(numero_righe, std::vector<T>(numero_colonne, elemento));
}

template <typename T>
Campo<T>::Campo(int numero_righe, int numero_colonne)
{
	init(numero_righe, numero_colonne, T());
}

template <typename T>
Campo<T>::Campo(int numero_righe, int numero_colonne, T elemento)
{
	init(numero_righe, numero_colonne, elemento);
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

std::ostream& linea_orizzontale(std::ostream& os, const int& lunghezza)
{
	for (int j = 0; j < lunghezza; j++)
	{
		os << u8"─";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Campo<std::string>& campo)
{
	auto start = std::chrono::steady_clock::now();
	
	if (campo._colonne() > 9)
	{
		os << std::setw(3) << std::setfill(' ');
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
	}
	os << std::setw(4) << std::setfill(' ');
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << (j + 1) % 10;
	}
	os << std::endl;
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"┌";
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << u8"─";
	}
	os << u8"┐" << std::endl;
	for (int i = 0; i < campo._righe(); i++)
	{
		os << std::right << std::setw(2) << std::setfill(' ') << i + 1 << u8"│";
		os << "\x1B[48;2;192;192;192m";
		for (int j = 0; j < campo._colonne(); j++)
		{
			os << mappa_colori[campo[i][j]] << campo[i][j];
		}
		os << "\x1B[0m" << u8"│" << std::left << std::setw(2) << std::setfill(' ') << i + 1 << std::endl;
	}
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"└";
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << u8"─";
	}
	os << u8"┘" << std::endl;
	if (campo._colonne() > 9)
	{
		os << std::setw(3) << std::setfill(' ');
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
	}
	os << std::setw(4) << std::setfill(' ');
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << (j + 1) % 10;
	}
	os << std::endl;

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return os;
}

template <typename T>
void Campo<T>::reset(T elemento) {
	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			campo[i][j] = elemento;
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
int Campo<T>::conta_nulli() const
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

#endif // __CAMPO_H__