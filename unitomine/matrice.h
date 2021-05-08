#ifndef __MATRICE_H__
#define __MATRICE_H__

#include <iostream>		// std::cout / std::cin
#include <string>		// classe string
#include <vector>		// classe vector
#include <map>			// classe map
#include <iomanip>		// std::setw() / std::setfill()
#include <iterator>		// classe iterator
#include <chrono>

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// È consigliato usare Dejavù Sans Mono dato che supporta molti caratteri unicode.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max).
#include <windows.h>
#endif

// TO DO: usare una classe Vettore personalizzata (vedi Padovani) in modo che non si possa per errore ridimensionare una riga al di fuori delle dimensioni prestabilite

template <typename T>
class Matrice
{
private:
	/* CAMPI PRIVATI */
	int	righe;								// 0 <= righe
	int colonne;							// 0 <= colonne
	std::vector<std::vector<T> > data;

	/* INIZIALIZZATORE DEI COSTRUTTORI */
	void init(int, int, T);
public:
	/* COSTRUTTORI */
	Matrice(int = 0, int = 0);
	Matrice(int, int, T);												// costruttore con valore di default delle celle dato in input iniziale

	/* LETTORE NUMERO DI RIGHE/COLONNE */
	int _righe() const						{ return righe; };				// legge la riga della matrice
	int _colonne() const					{ return colonne; };

	/* LETTURA/SCRITTURA DELLE RIGHE */
	std::vector<T> operator[](int i) const	{ return data.at(i); };		// legge la riga i-esima della matrice
	std::vector<T>& operator[](int i)		{ return data.at(i); };		// scrive la riga i-esima della matrice

	/* METODI PER MODIFICARE LA MATRICE (DIMENSIONI, RESET) */
	void push_back(std::vector<T>);
	void scambia_righe(int, int);
	void resize(int, int, T);
	void reset(T = T());

	/* METODI PER OTTENERE INFORMAZIONI SUGLI ELEMENTI DELLA MATRICE */
	bool indici_leciti(int, int) const;
	bool is_elemento(int, int, T) const;
	int conta_tutti_elemento(T) const;
	int conta_caselle_vicine(int, int) const;			// conta i vicini della cella (i, j) in totale (= 3 se è una cella nell'angolo, = 5 se sul bordo e = 8 se in mezzo alla matrice)
	int conta_vicini(int, int, T) const;		// conta i vicini della cella (i, j) che sono pari all'elemento dato in input
	bool conta_se_vicini(int, int, T) const;

	/* METODI PER CONFRONTO DI MATRICI */
	template <typename T> friend bool operator==(const Matrice<T>& sinistra, const Matrice<T>& destra);

	/* ELIMINAZIONE GAUSSIANA */
	void riduzione_gaussiana(std::vector<T>&);

};

template <typename T>
void Matrice<T>::init(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 0 || numero_colonne < 0 ) throw std::domain_error("dimensioni della matrice invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	data.resize(numero_righe, std::vector<T>(numero_colonne, elemento));
}

template <typename T>
Matrice<T>::Matrice(int numero_righe, int numero_colonne )
{
	init(numero_righe, numero_colonne, T());
}

template <typename T>
Matrice<T>::Matrice(int numero_righe, int numero_colonne, T elemento)
{
	init(numero_righe, numero_colonne, elemento);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrice<T>& matrice)
{
	for (int i = 0; i < matrice._righe(); i++)
	{
		for (int j = 0; j < matrice._colonne(); j++)
		{
			os << matrice[i][j];
		}
		os << std::endl;
	}
	return os;
}

template <typename T>
void Matrice<T>::reset(T elemento) {
	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			data[i][j] = elemento;
		}
	}
}

template <typename T>
void Matrice<T>::push_back(std::vector<T> riga) {
	if (riga.size() != colonne) throw std::domain_error("dimensioni della nuova riga non compatibili con la matrice");
	data.push_back(riga);
	righe = righe + 1;
}

template <typename T>
void Matrice<T>::scambia_righe(int i, int j) {
	std::vector<T> temp (data[j]);
	data[j] = data[i];
	data[i] = temp;
}

template <typename T>
void Matrice<T>::resize(int altezza, int larghezza, T elemento) {
	righe = altezza;
	colonne = larghezza;
	data.resize(altezza, std::vector<T>(larghezza, elemento));
	for (int i = 0; i < altezza; i++)
	{
		data[i].resize(larghezza, elemento);
	}
}

template <typename T>
bool Matrice<T>::indici_leciti(int i, int j) const
{
	return i >= 0 && i < righe&& j >= 0 && j < colonne;
}

template <typename T>
bool Matrice<T>::is_elemento(int i, int j, T elemento) const
{
	return data[i][j] == elemento;
}

template <typename T>
int Matrice<T>::conta_tutti_elemento(T elemento) const
{
	int k = 0;

	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			if (is_elemento(i, j, elemento)) k++;
		}
	}
	return k;
}

template <typename T>
int Matrice<T>::conta_caselle_vicine(int i, int j) const
{
	if ((i == 0 && j == 0) || (i == righe - 1 && j == colonne - 1)) return 3;
	else if (i == 0 || j == 0 || i == righe - 1 || j == colonne - 1) return 5;
	else return 8;
}

template <typename T>
int Matrice<T>::conta_vicini(int i, int j, T elemento) const
{
	if (!indici_leciti(i, j)) throw std::domain_error("controllo su cella illegittima");
	
	int k = 0;

	if (indici_leciti(i - 1, j - 1) && data[i - 1][j - 1] == elemento)	k++;
	if (indici_leciti(i - 1, j)		&& data[i - 1][j] == elemento)		k++;
	if (indici_leciti(i - 1, j + 1) && data[i - 1][j + 1] == elemento)	k++;
	if (indici_leciti(i, j - 1)		&& data[i][j - 1] == elemento)		k++;
	if (indici_leciti(i, j + 1)		&& data[i][j + 1] == elemento)		k++;
	if (indici_leciti(i + 1, j - 1) && data[i + 1][j - 1] == elemento)	k++;
	if (indici_leciti(i + 1, j)		&& data[i + 1][j] == elemento)		k++;
	if (indici_leciti(i + 1, j + 1) && data[i + 1][j + 1] == elemento)	k++;

	return k;
}

template <typename T>
bool Matrice<T>::conta_se_vicini(int i, int j, T elemento) const
{
	if (!indici_leciti(i, j)) throw std::domain_error("controllo su cella illegittima");

	if (indici_leciti(i - 1, j - 1) && data[i - 1][j - 1] == elemento)	return true;
	if (indici_leciti(i - 1, j)		&& data[i - 1][j] == elemento)		return true;
	if (indici_leciti(i - 1, j + 1) && data[i - 1][j + 1] == elemento)	return true;
	if (indici_leciti(i, j - 1)		&& data[i][j - 1] == elemento)		return true;
	if (indici_leciti(i, j + 1)		&& data[i][j + 1] == elemento)		return true;
	if (indici_leciti(i + 1, j - 1) && data[i + 1][j - 1] == elemento)	return true;
	if (indici_leciti(i + 1, j)		&& data[i + 1][j] == elemento)		return true;
	if (indici_leciti(i + 1, j + 1) && data[i + 1][j + 1] == elemento)	return true;
	
	return false;
}

template <typename T>
bool operator==(const Matrice<T>& sinistra, const Matrice<T>& destra)
{
	if (sinistra.righe != destra.righe || sinistra.colonne != destra.colonne) return false;
	for (int i = 0; i < sinistra.righe; i++)
	{
		for (int j = 0; j < sinistra.colonne; j++)
		{
			if (sinistra[i][j] != destra[i][j]) return false;
		}
	}
	return true;
}

// TO DO: spostare nella nuova classe Vettore
template <typename T>
int trova_indice_elemento(const std::vector<T>& vettore, T elemento, int indice_partenza = 0)
{
	if (indice_partenza > vettore.size()) throw std::range_error("indice di partenza non lecito");
	typename std::vector<T>::const_iterator it;
	int res = static_cast<int>(std::find(vettore.cbegin() + indice_partenza, vettore.cend(), elemento) - vettore.cbegin());
	if (res == vettore.size()) throw std::domain_error("elemento non trovato");
	else return res;
}

template <typename T>
bool trova_elemento(const std::vector<T>& vettore, T elemento)
{
	typename std::vector<T>::const_iterator it;
	it = std::find(vettore.cbegin(), vettore.cend(), elemento);
	return it != vettore.end();
}

template <typename T>
void Matrice<T>::riduzione_gaussiana(std::vector<T>& termine_noto)
{

	int h = 0;
	int k = 0;

	while (h < righe && k < colonne)
	{

		// TO DO: inserire lettore colonna

		std::vector<int> colonna_completa;
		std::vector<int> colonna;

		for (int p = 0; p < righe; p++)
		{
			colonna_completa.push_back(std::abs(data[p][k]));
		}

		for (int p = h; p < righe; p++)
		{
			colonna.push_back(std::abs(data[p][k]));
		}

		int i_max = trova_indice_elemento(colonna_completa, *std::max_element(colonna.cbegin(), colonna.cend()), h);

		if (data[i_max][k] == 0)
		{
			k += 1;
		}
		else
		{
			scambia_righe(h, i_max);
			std::swap(termine_noto[h], termine_noto[i_max]);
			for (int i = h + 1; i < righe; i++)
			{
				int mult = data[i][k] / data[h][k];
				data[i][k] = 0;
				for (int j = k + 1; j < colonne; j++)
				{
					data[i][j] = data[i][j] - data[h][j] * mult;
				}
				termine_noto[i] = termine_noto[i] - termine_noto[h] * mult;
			}

			h++;
			k++;

		}

	}
}

#endif // __MATRICE_H__