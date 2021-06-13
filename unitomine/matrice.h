// AUTORI:
// Massimo Bertolotti - massimo.bertolott86@edu.unito.it - Matricola n° 919899
//
// Guido Buffa		  - guido.buffa@edu.unito.it		 - Matricola n° 919899
// 
#ifndef __MATRICE_H__
#define __MATRICE_H__

#include <iomanip>		// std::setw() / std::setfill()
#include <chrono>

#include "vettore.h"

/* CLASSE MATRICE */
// Implementa una classe template per matrici; l'implementazione è simile alla classe Matrix fornita durante le lezioni ma, per motivi di incompatibilità
// di quest'ultima classe (basata su una ulteriore classe Vettore) con il tipo bool, è basata invece su std::vector<std::vector<T> >. Non sono stati
// implementati metodi presenti in Matrix che non venissero utilizzati ai fini del programma, ma è stato implementato un metodo di risoluzione di un
// sistema lineare con il metodo di Gauss, necessario per il metodo Gaussiano della classe Risolutore.

template <typename T>
class Matrice
{
private:
	/* CAMPI PRIVATI */
	int	righe;								// 0 <= righe
	int colonne;							// 0 <= colonne
	std::vector<std::vector<T> > data;
public:
/* COSTRUTTORI */
// Crea una matrice di dimensioni righe x colonne, i cui elementi sono tutti pari all'elemento dato in input.
	Matrice(int = 0, int = 0, T = T());											
	
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

/* RIDUSSIONE GAUSSIANA */
	
	// Opera la riduzione Gaussiana sulla matrice e su un opportuno termine noto. La matrice e il termine noto ridotto vengono restituiti come pair. 
	std::pair<Matrice<T>, std::vector<T>> riduzione_gaussiana_con_termine_noto(const std::vector<T>&);
};

// COSTRUTTORE: Crea una matrice di tipo T con ogni elemento di essa pari ad un elemento dato in input, verificando che le dimensioni fornite dall'utente siano accettabile (ergo non negative).
// Se non viene fornito alcun elemento in input viene messo di default l'elemento nullo di T, cioè T().
// INPUT: 
// •  (int) numero_righe: numero delle righe
// •  (int) numero_colonne: numero delle colonne
// •  (int) elemento: elemento con cui 'riempire' la matrice
template <typename T>
Matrice<T>::Matrice(int numero_righe, int numero_colonne, T elemento)
{
	if (numero_righe < 0 || numero_colonne < 0) throw std::domain_error("dimensioni della matrice invalide");
	righe = numero_righe;
	colonne = numero_colonne;
	data.resize(numero_righe, std::vector<T>(numero_colonne, elemento));
}

// Stampa a terminale di matrici. È tenuta esclusivamente per motivi di DEBUG.
// INPUT: 
// •  (const Matrice<T>&) matrice: la matrice da stampare
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

// Sostituisce tutti gli elementi della matrice con l'elemento dato.
// INPUT: 
// •  (T) elemento: l'elemento con cui sostituire gli elementi della matrice.
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

// Restituisce la j-esima colonna (con j=0 la prima colonna) della matrice come std::vector<T>.
// INPUT: 
// •  (int) j: indice della colonna.
// OUTPUT:
// •  (std::vector<T>): la colonna della matrice
template <typename T>
std::vector<T> Matrice<T>::colonna(int j) const {
	std::vector<T> col;
	for (int i = 0; i < righe; i++)
	{
		col.push_back(data[i][j]);
	}
	return col;
}

// Inserisce una nuova riga in fondo alla matrice data, controllando che abbia dimensioni compatibili.
// INPUT: 
// •  (const std::vector<T>&) riga: riga da aggiungere in fondo alla matrice.
template <typename T>
void Matrice<T>::push_back(const std::vector<T>& riga) {
	if (colonne != 0 && riga.size() != colonne) throw std::domain_error("dimensioni della nuova riga non compatibili con la matrice");
	data.push_back(riga);
	righe = righe + 1;
	if (colonne == 0) colonne = static_cast<int>(riga.size());
}

// Scambia due righe all'interno della matrice data.
// INPUT: 
// •  (int) i: prima riga da scambiare.
// •  (int) j: seconda riga da scambiare.
template <typename T>
void Matrice<T>::scambia_righe(int i, int j) {
	std::vector<T> temp (data[j]);
	data[j] = data[i];
	data[i] = temp;
}

// Controlla se gli indici (i, j) possono essere indici di un cella della matrice.
// INPUT: 
// •  (int) i: indice della riga della potenziale cella.
// •  (int) j: indice della colonna della potenziale cella.
// OUTPUT:
// •  (bool): 'true' se gli indici descrivono una cella effettiva della matrice, 'false' altrimenti
template <typename T>
bool Matrice<T>::indici_leciti(int i, int j) const
{
	return i >= 0 && i < righe && j >= 0 && j < colonne;
}

// Controlla che la cella (i, j) sia pari ad 'elemento'; NON restituisce un errore nel caso gli indici non siano leciti, bensì restituisce 'false'.
// INPUT: 
// •  (int) i: indice della riga della cella scelta.
// •  (int) j: indice della colonna della cella scelta.
// •  (T) elemento: elemento da confrontare con quello della cella (i, j).
// OUTPUT:
// •  (bool): 'true' se la cella (i, j) sta nella matrice ed è uguale a 'elemento', 'false' altrimenti.
template <typename T>
bool Matrice<T>::is_elemento(int i, int j, T elemento) const
{
	return indici_leciti(i,  j) && data[i][j] == elemento;
}

// Conta tutti le celle contenti 'elemento' all'interno della matrice data.
// INPUT: 
// •  (T) elemento: elemento da confrontare con quello delle celle della matrice.
// OUTPUT:
// •  (int): il numero di elementi pari a 'elemento'.
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

// Conta il numero di celle adiacenti a (i, j) uguali a 'elemento'. Controlla prima che la cella (i, j) sia effettivamente appartentente alla matrice.
// INPUT: 
// •  (int) i: indice della riga della cella scelta.
// •  (int) j: indice della colonna della cella scelta.
// •  (T) elemento: elemento da confrontare con quello delle celle adiacenti a (i, j).
// OUTPUT:
// •  (int): il numero di elementi attorno a (i, j) pari a 'elemento'.
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

// Conta se almeno una delle celle adiacenti a (i, j) è uguale a 'elemento'. Controlla prima che la cella (i, j) sia effettivamente appartentente alla matrice.
// INPUT: 
// •  (int) i: indice della riga della cella scelta.
// •  (int) j: indice della colonna della cella scelta.
// •  (T) elemento: elemento da confrontare con quello delle celle adiacenti a (i, j).
// OUTPUT:
// •  (bool): 'true' se almeno una delle celle adiacenti coincide con 'elemento', 'false' altrimenti.
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

// Riduce la matrice (vista come sistema lineare) e un opportuno termine noto con la riduzione Gaussiana.
// INPUT: 
// •  (const std::vector<T>&) termine_noto: vettore che rappresenta il termine noto da ridurre insieme alla matrice a cui applichiamo il metodo
// OUTPUT:
// •  (std::pair<Matrice<T>, std::vector<T> >): coppia il cui primo elemento è la matrice ridotta, mentre il secondo è il termine noto ridotto.
template <typename T>
std::pair<Matrice<T>, std::vector<T> > Matrice<T>::riduzione_gaussiana_con_termine_noto(const std::vector<T>& termine_noto)
{
	Matrice<T> matrice_ridotta = (*this);
	std::vector<T> termine_noto_ridotto = termine_noto;
	int h = 0;
	int k = 0;

	while (h < righe && k < colonne)
	{


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

#endif // __MATRICE_H__