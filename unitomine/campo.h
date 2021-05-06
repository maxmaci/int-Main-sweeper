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

// Definiamo una mappa che associa ad ogni numero un carattere colorato, ci serve per la stampa del campo visto dal giocatore.

std::map<int, std::string > mappa_conversione
{
	{8, "\x1B[38;2;128;128;128m8"},		// 8: grigio scurino
	{7, "\x1B[38;2;0;0;0m7"},			// 7: nero
	{6, "\x1B[38;2;0;128;129m6"},		// 6: turchese
	{5, "\x1B[38;2;129;1;2m5"},			// 5: amaranto
	{4, "\x1B[38;2;1;0;128m4"},			// 4: blu scuro
	{3, "\x1B[38;2;254;0;0m3"},			// 3: rosso
	{2, "\x1B[38;2;1;127;1m2"},			// 2: verde
	{1, "\x1B[38;2;1;0;254m1"},			// 1: blu chiaro
	{0, "\x1B[38;2;0;0;0m-"},			// scavato non mina: nero
	{-1, u8"\x1B[38;2;0;0;0m✱"},		// mina: nero	
	{-2, u8"\x1B[38;2;159;0;1m⚑"},		// bandiera: rosso scuro	
	{-3, u8"\x1B[38;2;0;0;0m⎕"},		// non scavato: nero
	{-4, u8"\x1B[48;2;159;0;1m\x1B[38;2;255;255;255m✱\x1B[48;2;192;192;192m"} // mina esplosa: sfondo rosso scuro, mina bianca
};

template <typename T>
class Campo
{
private:
	/* CAMPI */
	int	righe;								// 0 <= righe
	int colonne;							// 0 <= colonne
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
	void push_back(std::vector<T>);
	void swap(int, int);
	void resize(int, int, T);
	void reset(T = T());

	/* METODI PER INFORMAZIONI SUL CAMPO */
	bool nel_campo(int, int) const;
	bool is_nullo(int, int) const;
	int conta_tutti_nulli() const;
	int conta_vicini(int, int, T) const;
	bool conta_se_vicini(int, int, T) const;

	/* METODI PER CONFRONTO DI CAMPI */
	template <typename T> friend bool operator==(const Campo<T>& sinistra, const Campo<T>& destra);
};

template <typename T>
void Campo<T>::init(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 0 || numero_colonne < 0 ) throw std::domain_error("dimensioni del campo invalide");
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

std::ostream& operator<<(std::ostream& os, const Campo<int>& campo)
{
	//auto start = std::chrono::steady_clock::now();
	
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
		os << '\n';
	}
	os << std::setw(4) << std::setfill(' ');
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << (j + 1) % 10;
	}
	os << '\n';
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"┌";
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << u8"─";
	}
	os << u8"┐\n";
	for (int i = 0; i < campo._righe(); i++)
	{
		os << std::right << std::setw(2) << std::setfill(' ') << i + 1 << u8"│\x1B[48;2;192;192;192m";
		for (int j = 0; j < campo._colonne(); j++)
		{
			os << mappa_conversione[campo[i][j]];
		}
		os << u8"\x1B[0m│" << std::left << std::setw(2) << std::setfill(' ') << i + 1 << '\n';
	}
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"└";
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << u8"─";
	}
	os << u8"┘\n";
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
		os << '\n';
	}
	os << std::setw(4) << std::setfill(' ');
	for (int j = 0; j < campo._colonne(); j++)
	{
		os << (j + 1) % 10;
	}
	os << '\n';

	//auto end = std::chrono::steady_clock::now();

	//auto diff = end - start;

	//std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

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
void Campo<T>::push_back(std::vector<T> riga) {
	if (riga.size() != colonne) throw std::domain_error("dimensioni della nuova riga non compatibili con la matrice");
	campo.push_back(riga);
	righe = righe + 1;
}

template <typename T>
void Campo<T>::swap(int i, int j) {
	std::vector<T> temp (campo[j]);
	campo[j] = campo[i];
	campo[i] = temp;
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

/* TO DO: forse fare il check solo per -2 e -3 */
template <>
bool Campo<int>::is_nullo(int i, int j) const
{
	return campo[i][j] == -3 || campo[i][j] == -2;
}

/* TO DO: forse fare con gli if riduce le tempistiche ed è più corretto */
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

template <typename T>
int Campo<T>::conta_vicini(int i, int j, T elemento) const
{
	if (!nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	
	int k = 0;

	if (nel_campo(i - 1, j - 1) && campo[i - 1][j - 1] == elemento)	k++;
	if (nel_campo(i - 1, j)		&& campo[i - 1][j] == elemento)		k++;
	if (nel_campo(i - 1, j + 1) && campo[i - 1][j + 1] == elemento)	k++;
	if (nel_campo(i, j - 1)		&& campo[i][j - 1] == elemento)		k++;
	if (nel_campo(i, j + 1)		&& campo[i][j + 1] == elemento)		k++;
	if (nel_campo(i + 1, j - 1) && campo[i + 1][j - 1] == elemento)	k++;
	if (nel_campo(i + 1, j)		&& campo[i + 1][j] == elemento)		k++;
	if (nel_campo(i + 1, j + 1) && campo[i + 1][j + 1] == elemento)	k++;

	return k;
}

template <typename T>
bool Campo<T>::conta_se_vicini(int i, int j, T elemento) const
{
	if (!nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");

	if (nel_campo(i - 1, j - 1) && campo[i - 1][j - 1] == elemento)	return true;
	if (nel_campo(i - 1, j)		&& campo[i - 1][j] == elemento)		return true;
	if (nel_campo(i - 1, j + 1) && campo[i - 1][j + 1] == elemento)	return true;
	if (nel_campo(i, j - 1)		&& campo[i][j - 1] == elemento)		return true;
	if (nel_campo(i, j + 1)		&& campo[i][j + 1] == elemento)		return true;
	if (nel_campo(i + 1, j - 1) && campo[i + 1][j - 1] == elemento)	return true;
	if (nel_campo(i + 1, j)		&& campo[i + 1][j] == elemento)		return true;
	if (nel_campo(i + 1, j + 1) && campo[i + 1][j + 1] == elemento)	return true;
	
	return false;
}

template <typename T>
bool operator==(const Campo<T>& sinistra, const Campo<T>& destra)
{
	if (sinistra._righe() != destra._righe() || sinistra._colonne() != destra._colonne()) return false;
	for (int i = 0; i < sinistra._righe(); i++)
	{
		for (int j = 0; j < sinistra._colonne(); j++)
		{
			if (sinistra[i][j] != destra[i][j]) return false;
		}
	}
	return true;
}

#endif // __CAMPO_H__