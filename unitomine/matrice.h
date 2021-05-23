﻿#ifndef __MATRICE_H__
#define __MATRICE_H__

#include <iomanip>		// std::setw() / std::setfill()
#include <chrono>

#include "vettore.h"

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
	//void init(int, int, T);
public:
/* COSTRUTTORI */
// Crea una matrice di dimensioni righe x colonne, i cui elementi sono tutti pari all'elemento dato in input.
	Matrice(int = 0, int = 0, T = T());											
	
	//Matrice(int, int, T);	// TO DO: eliminare	// costruttore con valore di default delle celle dato in input iniziale

/* LETTORI CAMPI PRIVATI */

	// Restituisce il numero di righe della matrice. 
	int _righe() const						{ return righe; };
	// Restituisce il numero di colonne della matrice.
	int _colonne() const					{ return colonne; };

/* LETTURA/SCRITTURA DELLE RIGHE */

	// Legge la riga i-esima della matrice.
	std::vector<T> operator[](int i) const	{ return data.at(i); };
	// Scrive la riga i-esima della matrice.
	std::vector<T>& operator[](int i)		{ return data.at(i); };
	// Legge la colonna j-esima della matrice.
	std::vector<T> colonna(int) const;

/* METODI PER MODIFICARE LA MATRICE */

	// Aggiunge una nuova riga in fondo alla matrice, dopo quelle già presenti.
	void push_back(const std::vector<T>&);
	// Scambia la i-esima riga della matrice con la j-esima.
	void scambia_righe(int, int);
	// Sostituisce ogni elemento della matrice con quello dato in input. Di default sostituisce tutti gli elementi con l'elemento di default del tipo T.
	void sostituisci_tutti(T = T());

	//void resize(int, int, T);	// TO DO: cancellare

/* METODI PER OTTENERE INFORMAZIONI SUGLI ELEMENTI DELLA MATRICE */

	// Restituisce 'vero' se gli indici (i, j) dati in input si riferiscono ad un potenziale elemento della matrice o meno.
	bool indici_leciti(int, int) const;
	// Restituisce 'true' se l'elemento nella posizione (i, j) è uguale all'elemento dato in input, 'false' altrimenti.
	bool is_elemento(int, int, T) const;
	// Conta il numero di elementi presenti nella matrice che sono uguali all'elemento dato in input.
	int conta_tutti_elementi(T) const;
		
	// Conta il numero di elementi presenti nelle 8 (se nell'interno della matrice), nelle 5 (se sul bordo) o nelle 3 posizioni (se nell'angolo) attorno alla cella (i, j) che sono uguali all'elemento dato in input.
	int conta_vicini(int, int, T) const;
	
	// Restituisce 'true' se c'è almeno un elemento pari all'elemento dato in input nelle 8 (se nell'interno della matrice), nelle 5 (se sul bordo) o nelle 3 posizioni (se nell'angolo) attorno alla cella (i, j).
	bool conta_se_vicini(int, int, T) const;

	//int conta_caselle_vicine(int, int) const; // TO DO: cancellare // conta i vicini della cella (i, j) in totale (= 3 se è una cella nell'angolo, = 5 se sul bordo e = 8 se in mezzo alla matrice)


/* METODI PER CONFRONTARE DI MATRICI */

	//template <typename T> friend bool operator==(const Matrice<T>& sinistra, const Matrice<T>& destra); // TO DO: cancellare

/* OPERAZIONI CON MATRICI */

	// Restituisce la matrice (potenzialmente anche intesa come matrice riga o colonna) ottenuta dalla moltiplicazione matriciale.
	Matrice<T> mul(const Matrice<T>& m) const;

/* RIDUSSIONE GAUSSIANA E RANGO */
	
	// Opera la riduzione Gaussiana sulla matrice per restituire (come una nuova matrice) la matrice ridotta a scaloni.
	Matrice<T> riduzione_gaussiana();
	// Opera la riduzione Gaussiana sulla matrice e su un opportuno termine noto. La matrice e il termine noto ridotto vengono restituiti come pair. 
	std::pair<Matrice<T>, std::vector<T>> riduzione_gaussiana_con_termine_noto(const std::vector<T>&);
	// Restituisce il rango della matrice grazie alla riduzione Gaussiana.
	int rango();
};

/*
template <typename T>
void Matrice<T>::init(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 0 || numero_colonne < 0 ) throw std::domain_error("dimensioni della matrice invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	data.resize(numero_righe, std::vector<T>(numero_colonne, elemento));
}
*/
/*
template <typename T>
Matrice<T>::Matrice(int numero_righe, int numero_colonne, T elemento)
{
	init(numero_righe, numero_colonne, elemento);
}
*/

template <typename T>
Matrice<T>::Matrice(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 0 || numero_colonne < 0) throw std::domain_error("dimensioni della matrice invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	data.resize(numero_righe, std::vector<T>(numero_colonne, elemento));
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
void Matrice<T>::sostituisci_tutti(T elemento) {
	for (int i = 0; i < righe; i++)
	{
		for (int j = 0; j < colonne; j++)
		{
			data[i][j] = elemento;
		}
	}
}

template <typename T>
std::vector<T> Matrice<T>::colonna(int j) const {
	std::vector<T> col;
	for (int i = 0; i < righe; i++)
	{
		col.push_back(data[i][j]);
	}
	return col;
}

template <typename T>
void Matrice<T>::push_back(const std::vector<T>& riga) {
	if (colonne != 0 && riga.size() != colonne) throw std::domain_error("dimensioni della nuova riga non compatibili con la matrice");
	data.push_back(riga);
	righe = righe + 1;
	if (colonne == 0) colonne = static_cast<int>(riga.size());
}

template <typename T>
void Matrice<T>::scambia_righe(int i, int j) {
	std::vector<T> temp (data[j]);
	data[j] = data[i];
	data[i] = temp;
}
/*
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
*/
template <typename T>
bool Matrice<T>::indici_leciti(int i, int j) const
{
	return i >= 0 && i < righe&& j >= 0 && j < colonne;
}

template <typename T>
bool Matrice<T>::is_elemento(int i, int j, T elemento) const
{
	return indici_leciti(i,  j) && data[i][j] == elemento;			// controlla che l'elemento T è nel posto i e j; NON restituisce un errore nel caso l'indice non sia lecito, bensì restituisce falso
}

template <typename T>
int Matrice<T>::conta_tutti_elementi(T elemento) const
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
/*
template <typename T>
int Matrice<T>::conta_caselle_vicine(int i, int j) const
{
	if ((i == 0 && j == 0) || (i == righe - 1 && j == colonne - 1)) return 3;
	else if (i == 0 || j == 0 || i == righe - 1 || j == colonne - 1) return 5;
	else return 8;
}
*/
template <typename T>
int Matrice<T>::conta_vicini(int i, int j, T elemento) const
{
	if (!indici_leciti(i, j)) throw std::domain_error("controllo su cella illegittima");
	
	int k = 0;

	if (is_elemento(i - 1, j - 1, elemento))	k++;
	if (is_elemento(i - 1, j, elemento))		k++;
	if (is_elemento(i - 1, j + 1, elemento))	k++;
	if (is_elemento(i, j - 1, elemento))		k++;
	if (is_elemento(i, j + 1, elemento))		k++;
	if (is_elemento(i + 1, j - 1, elemento))	k++;
	if (is_elemento(i + 1, j, elemento))		k++;
	if (is_elemento(i + 1, j + 1, elemento))	k++;

	return k;
}

template <typename T>
bool Matrice<T>::conta_se_vicini(int i, int j, T elemento) const
{
	if (!indici_leciti(i, j)) throw std::domain_error("controllo su cella illegittima");

	if (is_elemento(i - 1, j - 1, elemento))	return true;
	if (is_elemento(i - 1, j, elemento))		return true;
	if (is_elemento(i - 1, j + 1, elemento))	return true;
	if (is_elemento(i, j - 1, elemento))		return true;
	if (is_elemento(i, j + 1, elemento))		return true;
	if (is_elemento(i + 1, j - 1, elemento))	return true;
	if (is_elemento(i + 1, j, elemento))		return true;
	if (is_elemento(i + 1, j + 1, elemento))	return true;
	
	return false;
}

/*
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
*/

template <typename T>
Matrice<T> Matrice<T>::riduzione_gaussiana()
{
	Matrice<T> matrice_ridotta = (*this);

	int h = 0;
	int k = 0;

	while (h < righe && k < colonne)
	{

		// TO DO: inserire lettore colonna

		std::vector<int> colonna_completa;
		std::vector<int> colonna;

		for (int p = 0; p < righe; p++)
		{
			colonna_completa.push_back(std::abs(matrice_ridotta[p][k]));
		}

		for (int p = h; p < righe; p++)
		{
			colonna.push_back(std::abs(matrice_ridotta[p][k]));
		}

		int i_max = trova_indice_elemento(colonna_completa, *std::max_element(colonna.cbegin(), colonna.cend()), h);

		if (matrice_ridotta[i_max][k] == 0)
		{
			k += 1;
		}
		else
		{
			matrice_ridotta.scambia_righe(h, i_max);
			for (int i = h + 1; i < righe; i++)
			{
				int mult = matrice_ridotta[i][k] / matrice_ridotta[h][k];
				matrice_ridotta[i][k] = 0;
				for (int j = k + 1; j < colonne; j++)
				{
					matrice_ridotta[i][j] = matrice_ridotta[i][j] - matrice_ridotta[h][j] * mult;
				}
			}

			h++;
			k++;

		}

	}

	return matrice_ridotta;
}

template <typename T>
std::pair<Matrice<T>, std::vector<T>> Matrice<T>::riduzione_gaussiana_con_termine_noto(const std::vector<T>& termine_noto)
{
	Matrice<T> matrice_ridotta = (*this);
	std::vector<T> termine_noto_ridotto = termine_noto;
	int h = 0;
	int k = 0;

	while (h < righe && k < colonne)
	{

		// TO DO: inserire lettore colonna

		std::vector<int> colonna_completa;
		std::vector<int> colonna;

		for (int p = 0; p < righe; p++)
		{
			colonna_completa.push_back(std::abs(matrice_ridotta[p][k]));
		}

		for (int p = h; p < righe; p++)
		{
			colonna.push_back(std::abs(matrice_ridotta[p][k]));
		}

		int i_max = trova_indice_elemento(colonna_completa, *std::max_element(colonna.cbegin(), colonna.cend()), h);

		if (matrice_ridotta[i_max][k] == 0)
		{
			k += 1;
		}
		else
		{
			matrice_ridotta.scambia_righe(h, i_max);
			std::swap(termine_noto_ridotto[h], termine_noto_ridotto[i_max]);
			for (int i = h + 1; i < righe; i++)
			{
				int mult = matrice_ridotta[i][k] / matrice_ridotta[h][k];
				matrice_ridotta[i][k] = 0;
				for (int j = k + 1; j < colonne; j++)
				{
					matrice_ridotta[i][j] = matrice_ridotta[i][j] - matrice_ridotta[h][j] * mult;
				}
				termine_noto_ridotto[i] = termine_noto_ridotto[i] - termine_noto_ridotto[h] * mult;
			}

			h++;
			k++;

		}

	}

	return std::make_pair(matrice_ridotta, termine_noto_ridotto);
}

template <typename T>
int Matrice<T>::rango()
{
	Matrice<T> matrice_ridotta = riduzione_gaussiana();
	
	std::vector<T> riga_vuota(matrice_ridotta.colonne, 0);

	int righe_non_nulle = 0;
	for (int i = 0; i < matrice_ridotta.righe; i++)
	{
		if (matrice_ridotta[i] != riga_vuota) righe_non_nulle++;
	}
	return righe_non_nulle;
}

#endif // __MATRICE_H__