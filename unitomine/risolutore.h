#ifndef __RISOLUTORE_H__
#define __RISOLUTORE_H__
#include <set>
#include <algorithm>

#include "menu.h"

// Calcolo del coefficiente binomiale con produttoria. Il valore non è accurato 100%, ma è sufficientemente accurato per il calcolo delle probabilità
// che ci servono, è computazionalmente veloce da calcolare e meno prono a problemi di overflow rispetto all'implementazione ricorsiva, soprattutto
// nelle situazioni in cui viene usato dal metodo probabilistico che richiede calcoli di coefficienti binomiali molto grandi.
// INPUT: 
// •  (int)  n.
// •  (int)  k: deve essere tale per cui 0 <= k <= n
// OUTPUT:
// •  (long double) : il coefficiente binomiale

long double bin(int n, int k) {
	long double res = 1;
	for (int i = 1; i <= k; i++) res *= (n + 1 - i) / long double(i);
	return res;
}

// Restituisce vero se la dimensione del primo vettore (di coordinate) è minore della dimensione del secondo.
// INPUT: 
// •  (const std::vector<Coord>&) p: primo vettore.
// •  (const std::vector<Coord>&) q: secondo vettore.
bool ordina_per_dimensione(const std::vector<Coord>& p, const std::vector<Coord>& q)
{
	return p.size() < q.size();
}

/* Classe Risolutore */
class Risolutore
{
private:
	Campo partita;						
	int bandiere_precedenti;			// 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()
	int celle_non_scavate_precedenti;	// 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()

/* METODI PER RICAVARE INFORMAZIONI SUL CAMPO */

	// Restituisce un vettore con le coordinate dei numeri che hanno ancora delle celle non scavate e non bandierate attorno.
	std::vector<Coord> estrai_numeri_bordo();
	// Restituisce un vettore con le coordinate delle celle non scavate  che hanno accanto dei numeri (ovvero il 'bordo' delle celle non scavate con quelle scavate).
	std::vector<Coord> estrai_incognite_bordo();
	// Restituisce un vettore con le coordinate delle celle non scavate che NON hanno accanto dei numeri
	std::vector<Coord> estrai_incognite_non_bordo();

	// Partiziona le celle incognite del bordo in sezioni indipendenti l'una dall'altra (cioè tali per cui i numeri accanto ad una certa sezione influiscono solo ed esclusivamente su quella sezione).
	std::vector<std::vector<Coord> > separa_incognite_bordo();
	// Partiziona i numeri accanto alle incognite del bordo sulla base delle sezioni individuate con 'separa_incognite_bordo'
	std::vector<std::vector<Coord> > separa_numeri(const std::vector<std::vector<Coord> >&);

	// Verifica che una eventuale disposizione delle mine sul bordo sia lecita sulla base delle informazioni date dai numeri accanto al bordo.
	bool disposizione_lecita(const std::vector<Coord>&, const std::vector<Coord>&, std::vector<bool>&);

/* METODI RISOLUTIVI */
	void metodo_meccanico();
	void metodo_gaussiano(const std::vector< std::vector<Coord>>&, const std::vector< std::vector<Coord>>&);
	bool metodo_probabilistico(const std::vector< std::vector<Coord>>&, const std::vector< std::vector<Coord>>&);
	void metodo_casuale(const std::vector< std::vector<Coord>>&);

	// Stampa a schermo un breve report sulla situazione attuale (lo status della partita, le bandiere piazzate sul totale del mine, le celle rimanenti - cioè non scavate o bandierate) confrontato al passo precedente.
	void stampa_situazione();
public:
/* COSTRUTTORE */
	// Crea un oggetto Risolutore a partire da una partita (iniziata).
	Risolutore(const Campo&);

/* METODI DI LETTURA DI CAMPI PRIVATI */
	// Restituisce la partita all'utente.
	Campo _partita() { return partita; };

/* RISOLUZIONE  */
	// Inizia il processo risolutivo, chiamando i vari metodi risolutivi definiti privatamente. L'intero strettamento positivo dato in input è un parametro di compilazione che definisce la dimensione massima delle sezioni di bordo di incognite che viene analizzato dal metodo probabilistico: se la lunghezza di una sezione supera tale valore, il metodo probabilistico (per motivi computazionali) viene bypassato e si passa al metodo casuale.
	void risolvi(int);
};

// COSTRUTTORE: Inizializza il risolutore, impostando la partita, il numero delle bandiere e delle celle non scavati in base al Campo fornito
// INPUT: 
// •  (const Campo&) campo: il campo della partita da risolvere in automatico
Risolutore::Risolutore(const Campo& campo)
{
	if (campo._numero_bandiere() != 0) throw std::domain_error("Il campo non deve avere alcuna bandiera piazzata");
	partita = campo;
	bandiere_precedenti = campo._numero_bandiere();
	celle_non_scavate_precedenti = campo._campo_visibile().conta_tutti_elementi(-3);
}

// Restituisce un vettore con le coordinate dei numeri che hanno ancora delle celle non scavate e non bandierate attorno.
std::vector<Coord> Risolutore::estrai_numeri_bordo()
{
	std::vector<Coord> numeri;

	// recuperiamo tutti i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno: immagazziniamo tutto in un vettore

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
			{
				numeri.push_back(Coord(i, j));
			}
		}
	}

	// convertiamo l'insieme in un vettore per applicare più facilmente il passaggio successivo

	return numeri;
}

// Restituisce un vettore con le coordinate delle celle non scavate che hanno accanto dei numeri (ovvero il 'bordo' delle celle non scavate con quelle scavate).
std::vector<Coord> Risolutore::estrai_incognite_bordo()
{
	std::set<Coord> incognite;

	// recuperiamo tutte i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno: immagazziniamo tutto in un vettore

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
			{
				if (partita._campo_visibile().is_elemento(i - 1, j - 1, -3))	incognite.insert(Coord(i - 1, j - 1));
				if (partita._campo_visibile().is_elemento(i - 1, j, -3))		incognite.insert(Coord(i - 1, j));
				if (partita._campo_visibile().is_elemento(i - 1, j + 1, -3))	incognite.insert(Coord(i - 1, j + 1));
				if (partita._campo_visibile().is_elemento(i, j - 1, -3))		incognite.insert(Coord(i, j - 1));
				if (partita._campo_visibile().is_elemento(i, j + 1, -3))		incognite.insert(Coord(i, j + 1));
				if (partita._campo_visibile().is_elemento(i + 1, j - 1, -3))	incognite.insert(Coord(i + 1, j - 1));
				if (partita._campo_visibile().is_elemento(i + 1, j, -3))		incognite.insert(Coord(i + 1, j));
				if (partita._campo_visibile().is_elemento(i + 1, j + 1, -3))	incognite.insert(Coord(i + 1, j + 1));
			}
		}
	}

	std::vector<Coord> incognite_vettore(incognite.begin(), incognite.end());	// Convertiamo l'insieme in un vettore.


	return incognite_vettore;
}

// Restituisce un vettore con le coordinate delle celle non scavate che NON hanno accanto dei numeri
std::vector<Coord> Risolutore::estrai_incognite_non_bordo()
{
	std::vector<Coord> incognite;

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] == -3 && partita.conta_numeri_vicini(i, j) == 0) incognite.push_back(Coord(i, j));
		}
	}

	return incognite;
}

// Partiziona le celle incognite del bordo in sezioni indipendenti l'una dall'altra (cioè tali per cui i numeri accanto ad una certa sezione influiscono solo ed esclusivamente su quella sezione).
// OUTPUT: 
// •  (const std::vector<std::vector<Coord> >&): un vettore di vettori di coordinate; ogni elemento del vettore 'esterno' contiene una sezione del bordo
std::vector<std::vector<Coord> > Risolutore::separa_incognite_bordo()
{
	auto start = std::chrono::steady_clock::now();

	std::vector<Coord> incognite = estrai_incognite_bordo();

	std::vector<std::vector<Coord> > incognite_separate;

	std::queue<Coord> coda;
	coda.push(incognite[0]);

	while (incognite.size() != 0)
	{
		std::vector<Coord> incognite_separate_regione;
		if (incognite.size() != 0 && coda.size() == 0)
		{
			coda.push(incognite[0]);
		}
		while (incognite.size() != 0 && coda.size() != 0)
		{
			Coord cella = coda.front();
			coda.pop();

			if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && !trova_elemento(incognite_separate_regione, cella) && trova_elemento(incognite, cella))
			{
				incognite_separate_regione.push_back(cella);
				incognite.erase(incognite.begin() + trova_indice_elemento(incognite, cella));

				// bordi laterali
				if (partita._campo_visibile().indici_leciti(cella.first - 1, cella.second) && partita._campo_visibile()[cella.first - 1][cella.second] > 0)
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second - 1, -2)) coda.push(Coord(cella.first - 1, cella.second - 1)); // NORD-OVEST 
					else coda.push(Coord(cella.first, cella.second - 1)); // OVEST
					if (partita._campo_visibile().is_elemento(cella.first, cella.second + 1, -2)) coda.push(Coord(cella.first - 1, cella.second + 1)); // NORD-EST
					else coda.push(Coord(cella.first, cella.second + 1)); // EST
				}
				if (partita._campo_visibile().indici_leciti(cella.first + 1, cella.second) && partita._campo_visibile()[cella.first + 1][cella.second] > 0)
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second - 1, -2)) coda.push(Coord(cella.first + 1, cella.second - 1)); // SUD-OVEST
					else coda.push(Coord(cella.first, cella.second - 1)); // OVEST
					if (partita._campo_visibile().is_elemento(cella.first, cella.second + 1, -2)) coda.push(Coord(cella.first + 1, cella.second + 1)); // SUD-EST
					else coda.push(Coord(cella.first, cella.second + 1)); // EST
				}
				if (partita._campo_visibile().indici_leciti(cella.first, cella.second - 1) && partita._campo_visibile()[cella.first][cella.second - 1] > 0)
				{
					if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second, -2)) coda.push(Coord(cella.first - 1, cella.second - 1)); // NORD-OVEST
					else coda.push(Coord(cella.first - 1, cella.second)); // NORD
					if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second, -2)) coda.push(Coord(cella.first + 1, cella.second - 1)); // SUD-OVEST
					else coda.push(Coord(cella.first + 1, cella.second)); // SUD
				}
				if (partita._campo_visibile().indici_leciti(cella.first, cella.second + 1) && partita._campo_visibile()[cella.first][cella.second + 1] > 0)
				{
					if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second, -2)) coda.push(Coord(cella.first - 1, cella.second + 1)); // NORD-EST
					else coda.push(Coord(cella.first - 1, cella.second)); // NORD
					if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second, -2)) coda.push(Coord(cella.first + 1, cella.second + 1)); // SUD-EST
					else coda.push(Coord(cella.first + 1, cella.second)); // SUD
				}
				// angoli
				if ((partita._campo_visibile().indici_leciti(cella.first - 1, cella.second - 1) && partita._campo_visibile()[cella.first - 1][cella.second - 1] > 0))
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second - 1, -2)) coda.push(Coord(cella.first, cella.second - 2)); // OVEST-OVEST
					else coda.push(Coord(cella.first, cella.second - 1)); // OVEST
					if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second, -2)) coda.push(Coord(cella.first - 2, cella.second)); // NORD-NORD
					else coda.push(Coord(cella.first - 1, cella.second)); // NORD	
				}
				if ((partita._campo_visibile().indici_leciti(cella.first - 1, cella.second + 1) && partita._campo_visibile()[cella.first - 1][cella.second + 1] > 0))
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second + 1, -2)) coda.push(Coord(cella.first, cella.second + 2)); // OVEST-OVEST
					else coda.push(Coord(cella.first, cella.second + 1)); // EST
					if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second, -2)) coda.push(Coord(cella.first - 2, cella.second)); // NORD-NORD
					else coda.push(Coord(cella.first - 1, cella.second)); // NORD
				}
				if ((partita._campo_visibile().indici_leciti(cella.first + 1, cella.second - 1) && partita._campo_visibile()[cella.first + 1][cella.second - 1] > 0))
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second - 1, -2)) coda.push(Coord(cella.first, cella.second - 2)); // OVEST-OVEST
					else coda.push(Coord(cella.first, cella.second - 1)); // OVEST
					if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second, -2)) coda.push(Coord(cella.first + 2, cella.second)); // NORD-NORD
					else coda.push(Coord(cella.first + 1, cella.second)); // NORD
				}
				if ((partita._campo_visibile().indici_leciti(cella.first + 1, cella.second + 1) && partita._campo_visibile()[cella.first + 1][cella.second + 1] > 0))
				{
					if (partita._campo_visibile().is_elemento(cella.first, cella.second + 1, -2)) coda.push(Coord(cella.first, cella.second + 2)); // OVEST-OVEST
					else coda.push(Coord(cella.first, cella.second + 1)); // EST
					if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second, -2)) coda.push(Coord(cella.first + 2, cella.second)); // SUD-SUD
					else coda.push(Coord(cella.first + 1, cella.second)); // SUD
				}
			}
			if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && partita._campo_visibile()[cella.first][cella.second] > 0 && partita.conta_non_scavati_vicini(cella.first, cella.second) > 0)
			{
				if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second - 1, -3))	coda.push(Coord(cella.first - 1, cella.second - 1));
				if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second, -3))		coda.push(Coord(cella.first - 1, cella.second));
				if (partita._campo_visibile().is_elemento(cella.first - 1, cella.second + 1, -3))	coda.push(Coord(cella.first - 1, cella.second + 1));
				if (partita._campo_visibile().is_elemento(cella.first, cella.second - 1, -3))		coda.push(Coord(cella.first, cella.second - 1));
				if (partita._campo_visibile().is_elemento(cella.first, cella.second + 1, -3))		coda.push(Coord(cella.first, cella.second + 1));
				if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second - 1, -3))	coda.push(Coord(cella.first + 1, cella.second - 1));
				if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second, -3))		coda.push(Coord(cella.first + 1, cella.second));
				if (partita._campo_visibile().is_elemento(cella.first + 1, cella.second + 1, -3))	coda.push(Coord(cella.first + 1, cella.second + 1));
			}
		}
		if (incognite_separate_regione.size() != 0)
		{
			incognite_separate.push_back(incognite_separate_regione);
		}
	}

	std::sort(incognite_separate.begin(), incognite_separate.end(), ordina_per_dimensione);

	return incognite_separate;
}

// Partiziona i numeri accanto alle incognite del bordo sulla base delle sezioni individuate con 'separa_incognite_bordo'.
// INPUT: 
// •  (const std::vector<std::vector<Coord> >&) bordo_separato: il bordo di incognite separato per sezioni ottenuto con il metodo 'separa_incognite_bordo'
// OUTPUT: 
// •  (const std::vector<std::vector<Coord> >&): i numeri accanto al bordo separati secondo le sezioni individuate in precedenza con 'separa_incognite_bordo'
std::vector<std::vector<Coord> > Risolutore::separa_numeri(const std::vector<std::vector<Coord> >& bordo_separato)
{
	std::vector<Coord> numeri = estrai_numeri_bordo();

	std::vector<std::vector<Coord> > numeri_separati;

	for (int i = 0; i < bordo_separato.size(); i++)
	{
		std::set<Coord> numeri_accanto;

		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second - 1)))	numeri_accanto.insert(Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second - 1));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second)))		numeri_accanto.insert(Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second + 1)))	numeri_accanto.insert(Coord(bordo_separato[i][j].first - 1, bordo_separato[i][j].second + 1));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first, bordo_separato[i][j].second - 1)))		numeri_accanto.insert(Coord(bordo_separato[i][j].first, bordo_separato[i][j].second - 1));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first, bordo_separato[i][j].second + 1)))		numeri_accanto.insert(Coord(bordo_separato[i][j].first, bordo_separato[i][j].second + 1));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second - 1)))	numeri_accanto.insert(Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second - 1));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second)))		numeri_accanto.insert(Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second));
			if (trova_elemento(numeri, Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second + 1)))	numeri_accanto.insert(Coord(bordo_separato[i][j].first + 1, bordo_separato[i][j].second + 1));
		}
		std::vector<Coord> numeri_accanto_vettore(numeri_accanto.begin(), numeri_accanto.end());

		numeri_separati.push_back(numeri_accanto_vettore);
	}
	return numeri_separati;
}

/* METODO MECCANICO */
// Il metodo itera ogni cella del campo e fa due controlli alternativi su ciascuna di essa:
// • Se la cella è un numero n e ci sono attorno n celle non scavate (con potenzialmente già delle bandierine) mette delle bandierine su tutte le celle non scavate attorno
// • Se la cella è un numero n e ci sono attorno già n celle bandierinate scava tutte le celle non scavate attorno.
void Risolutore::metodo_meccanico()
{
	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			// Mette le bandierine se attorno al numero n trova n celle non scavate (con potenzialmente già delle bandierine)
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) != 0 && partita.conta_non_scavati_vicini(i, j) + partita.conta_bandiere_vicine(i, j) == partita._campo_visibile()[i][j])
			{
				for (int n = i - 1; n <= i + 1; n++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (partita._campo_visibile().indici_leciti(n, m))
						{
							partita.gioca(n, m, 'B');
						}
					}
				}
			}
			// Scava tutto ciò che non è bandierinato attorno ad una cella numerata con n se sono già presenti esattamente n bandierine attorno.
			else if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) != 0 && partita.conta_bandiere_vicine(i, j) == partita._campo_visibile()[i][j])
			{
				for (int n = i - 1; n <= i + 1; n++)
				{
					for (int m = j - 1; m <= j + 1; m++)
					{
						if (partita._campo_visibile().indici_leciti(n, m))
						{
							partita.gioca(n, m, 'S');
						}
					}
				}

			}
		}
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

/* METODO GAUSSIANO */
// Il metodo legge le informazioni del campo (indizi/numeri e incognite), le interpreta come se fosse un sistema di equazioni lineari e cerca
// alcune soluzioni certe a partire da tale sistema. Il metodo opera in diverse fasi (spiegate più in dettaglio nel codice) e si ripete per ciascuna
// sezione in cui è separato il bordo per semplificare i calcoli.
// INPUT:
// • (const std::vector< std::vector<Coord>>&) bordo_separato: il bordo di incognite separato per sezioni ottenuto con il metodo 'separa_incognite_bordo'
// • (const std::vector< std::vector<Coord>>&) numeri_separati: i numeri accanto al bordo separati per sezioni ottenuti con il metodo 'separa_numeri'
void Risolutore::metodo_gaussiano(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati)
{
	auto start = std::chrono::steady_clock::now();

	for (int a = 0; a < numeri_separati.size(); a++)
	{
		// • FASE 1: creiamo una matrice di interi (inizialmente tutta di zeri) in cui assegniamo ad ogni numero associato ad una sezione una riga e ad ogni colonna una e una sola incognita della sezione;
		//	 per ogni numero/riga controlliamo quali incognite del bordo ha attorno e mettiamo '1' nella corrispondente colonna. Creiamo allo stesso tempo un vettore che funge da' termine_noto', i cui elementi
		//   sono i numeri accanto alla sezione, tolto il numero di bandiere già poste attorno a quel numero (l'ordine dei numeri è lo stesso delle righe).
		
		std::vector<Coord> sezione_numeri = numeri_separati[a];
		std::vector<Coord> sezione_bordo = bordo_separato[a];

		Matrice<int> matrice(static_cast<int>(sezione_numeri.size()), static_cast<int>(sezione_bordo.size()));
		std::vector<int> termine_noto;

		for (int n = 0; n < matrice._righe(); n++)
		{
			int i = sezione_numeri[n].first;
			int j = sezione_numeri[n].second;

			termine_noto.push_back(partita._campo_visibile()[i][j] - partita.conta_bandiere_vicine(i, j));

			if (trova_elemento(sezione_bordo, Coord(i - 1, j - 1)))	matrice[n][trova_indice_elemento(sezione_bordo, Coord(i - 1, j - 1))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i - 1, j)))		matrice[n][trova_indice_elemento(sezione_bordo, Coord(i - 1, j))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i - 1, j + 1)))	matrice[n][trova_indice_elemento(sezione_bordo, Coord(i - 1, j + 1))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i, j - 1)))		matrice[n][trova_indice_elemento(sezione_bordo, Coord(i, j - 1))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i, j + 1)))		matrice[n][trova_indice_elemento(sezione_bordo, Coord(i, j + 1))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i + 1, j - 1)))	matrice[n][trova_indice_elemento(sezione_bordo, Coord(i + 1, j - 1))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i + 1, j)))		matrice[n][trova_indice_elemento(sezione_bordo, Coord(i + 1, j))] = 1;
			if (trova_elemento(sezione_bordo, Coord(i + 1, j + 1)))	matrice[n][trova_indice_elemento(sezione_bordo, Coord(i + 1, j + 1))] = 1;
		}

		// • FASE 2: riduciamo la matrice e il termine noto con la riduzione Gaussiana definita come metodo della classe matrice.

		std::pair<Matrice<int>, std::vector<int>> matrice_completa = matrice.riduzione_gaussiana_con_termine_noto(termine_noto);

		// • FASE 3: controlliamo i risultati ottenuti. Eliminiamo tutte le righe vuote della matrice ridotta (e di conseguenza eliminiamo i valori corrispondenti dal termine_noto;
		//   poiché ogni incognite è Booleana (mina presente/mina non presente) per ogni riga della matrice - che rappresenta una equazione con termine noto - possiamo calcolare il valore massimo e minimo assunto da essa
		//   al variare delle possibili mine: se uno di questi valori massimo/minimo corrisponde al valore nel termine noto abbiamo la certezza se alcune specifiche incognite saranno mine e quali no (secondo delle regole che dipendono dal segno del coefficiente dell'incognita).

		Matrice<int> matrice_ridotta(0, matrice_completa.first._colonne());
		std::vector<int> termine_noto_ridotto;
		for (int i = 0; i < matrice_completa.first._righe(); i++)
		{
			for (int j = 0; j < matrice_completa.first._colonne(); j++)
			{
				if (matrice_completa.first[i][j] != 0)		// Consideriamo solamente le righe non vuote (il sistema originale non sempre ha soluzioni - anzi, quasi mai!)
				{
					matrice_ridotta.push_back(matrice_completa.first[i]);
					termine_noto_ridotto.push_back(matrice_completa.second[i]);
					break;
				}
			}
		}

		for (int i = matrice_ridotta._righe() - 1; i >= 0; i--)
		{
			int upper_bound = 0;
			int lower_bound = 0;

			for (int j = 0; j < matrice_ridotta._colonne(); j++)
			{
				if (matrice_ridotta[i][j] > 0)	upper_bound += matrice_ridotta[i][j];
				else lower_bound += matrice_ridotta[i][j];

			}

			if (upper_bound == termine_noto_ridotto[i])
			{
				// tutti i termini con coefficiente >0 sono mine, tutti quelli con coefficiente <0 non lo sono

				for (int j = 0; j < matrice_ridotta._colonne(); j++)
				{
					if (matrice_ridotta[i][j] > 0)
					{
						partita.gioca(sezione_bordo[j].first, sezione_bordo[j].second, 'B');
					}
					else if (matrice_ridotta[i][j] < 0)
					{
						partita.gioca(sezione_bordo[j].first, sezione_bordo[j].second, 'S');
					}
				}
			}
			else if (lower_bound == termine_noto_ridotto[i])
			{
				// tutti i termini con coefficiente <0 sono mine, tutti quelli con coefficiente >0 non lo sono

				for (int j = 0; j < matrice_ridotta._colonne(); j++)
				{
					if (matrice_ridotta[i][j] < 0)
					{
						partita.gioca(sezione_bordo[j].first, sezione_bordo[j].second, 'B');
					}
					else if (matrice_ridotta[i][j] > 0)
					{
						partita.gioca(sezione_bordo[j].first, sezione_bordo[j].second, 'S');
					}
				}
			}
			if (i != 0)
			{
				for (int b = i - 1; b > 0; b--)
				{
					for (int a = 0; a < matrice_ridotta._colonne(); a++)
					{
						matrice_ridotta[b][a] -= matrice_ridotta[i][a];
					}
					termine_noto_ridotto[b] -= termine_noto_ridotto[i];
				}
			}

		}

	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

/* METODO PROBABILISTICO */
bool Risolutore::disposizione_lecita(const std::vector<Coord>& bordo_separato, const std::vector<Coord>& numeri_separati, std::vector<bool>& disposizione)
{
	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int conta_mine_cella = 0;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second - 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second - 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second + 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second + 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second - 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second - 1))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second + 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second + 1))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second - 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second - 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second + 1)) && disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second + 1))])	conta_mine_cella++;

		if (conta_mine_cella != partita._campo_visibile()[numeri_separati[i].first][numeri_separati[i].second] - partita.conta_bandiere_vicine(numeri_separati[i].first, numeri_separati[i].second)) return false;
	}

	return true;

}

// Il metodo calcola per ciascuna cella la probabilità di essere o meno una mina sulla base di diversi fattori: le possibili disposizioni lecite delle mine
// sulle sezioni di bordo, come poter porre nelle celle rimanenti tutte le mine non poste sul bordo e quante volte una mina è presente in una data cella al variare delle disposizioni.
// Se ci sono celle la cui probabilità è 1 (quasi certamente una mina) o 0 (quasi certamente non una mina) tali celle vengono opportunamente bandierate o scavate, rispettivamente e si esce dal metodo.
// Se non ci sono celle quasi certe si prende la probabilità più bassa, si scava la prima cella con tale probabilità e si esce dal metodo. 
// INPUT:
// • (const std::vector< std::vector<Coord>>&) bordo_separato: il bordo di incognite separato per sezioni ottenuto con il metodo 'separa_incognite_bordo'
// • (const std::vector< std::vector<Coord>>&) numeri_separati: i numeri accanto al bordo separati per sezioni ottenuti con il metodo 'separa_numeri'
// OUTPUT:
// • (bool): flag che serve per il metodo casuale nel caso non convenga per motivi probabilistic scavare alcuna cella del bordo.
bool Risolutore::metodo_probabilistico(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati)
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1. CALCOLO DELLE POSSIBILI DISPOSIZIONI LECITE

	// FASE 1.1. Prendiamo i numeri del bordo, precedentemente "separati" rispetto alle sezioni di bordo incognito a cui sono adiacenti,
	// e contiamo per ciascuna sezione la somma di tutti questi numeri tolto il numero di bandiere adiacienti già piazziate: in questo modo
	// avremo un tetto massimo al numero di mine presenti in tale sezione di bordo. Chiaramente questo è un valore potenzialmente molto più
	// alto dell'effettivo numero di mine presenti, ma è un buon limite sicuro per evitare di calcolare troppe combinazioni inutili.

	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		mine_max_separate.push_back(mine_max);
	}

	// FASE 1.2. Contiamo il numero di mine rimanenti. Questo ci permette, soprattutto quando ci avviamo alla conclusione della partita,
	// di limitare molto il calcolo delle combinazioni possibili.

	int mine_rimanenti = partita._mine() - partita._numero_bandiere();

	// FASE 1.3. Elenchiamo, per ciascuna sezione, le possibili disposizioni delle mine lecite (cioè che soddisfano le condizioni
	// imposte dai numeri) su tale sezione di bordo. Inoltre, per ciascuna sezione separiamo le disposizioni con un mappa le cui chiavi
	// rappresentano il numero di mine per tali disposizioni.
	// Una disposizione delle mine sulla sezione è posta come un vettore booleano, corrispondente 1 a 1 con il vettore di coordinate 'sezione_bordo':
	// se l'elemento n-esimo della disposizione è 'true', significa che la coordinata (i, j), pari all'elemento n-esimo di 'sezione_bordo', sarà una mina.

	std::vector<std::map<int, Matrice<bool>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::vector<Coord> sezione_numeri = numeri_separati[a];
		std::vector<Coord> sezione_bordo = bordo_separato[a];

		std::map<int, Matrice<bool>> disposizioni_per_sezione;

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(sezione_bordo.size()));
		estremi.push_back(mine_rimanenti);

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		for (int i = 0; i < estremo; i++)
		{
			std::vector<bool> disposizione(sezione_bordo.size());

			for (int j = 0; j <= i; j++) disposizione[j] = true;

			std::sort(disposizione.begin(), disposizione.end());

			do
			{
				if (disposizione_lecita(bordo_separato[a], numeri_separati[a], disposizione))
				{
					disposizioni_per_sezione[somma_elementi(disposizione)].push_back(disposizione);
				}
			} while (std::next_permutation(disposizione.begin(), disposizione.end()));
		}

		possibilita_per_sezione.push_back(disposizioni_per_sezione);
	}

	// FASE 2: CALCOLO DELLE PROBABILITÀ

	// FASE 2.1: Contiamo quante sono le celle non scavate di cui non abbiamo alcuna informazione, cioè al di fuori di quelle identificate dal bordo.

	int celle_non_scavate_fuori_bordo = partita._campo_visibile().conta_tutti_elementi(-3);

	for (int i = 0; i < bordo_separato.size(); i++)
	{
		celle_non_scavate_fuori_bordo -= static_cast<int>(bordo_separato[i].size());
	}

	// FASE 2.2: per ogni singola cella del bordo calcoliamo la sua probabilità di essere una mina. Per informazioni più specifiche su come viene calcolata
	// rimandiamo al paper presente nelle fonti del file "LEGGIMI.txt"

	std::vector<std::vector<long double> > probabilita_per_sezione;

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{
		std::vector<long double> probabilita_singola_sezione;

		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			long double numeratore = 0;
			long double denominatore = 0;

			for (std::map<int, Matrice<bool>>::const_iterator it = possibilita_per_sezione[i].cbegin(); it != possibilita_per_sezione[i].cend(); it++)
			{
				long double calcolo_parziale = 0;

				if (celle_non_scavate_fuori_bordo == 0) calcolo_parziale = 1;
				else
				{
					for (int k = 0; k < possibilita_per_sezione.size(); k++)
					{
						if (k != i)
						{
							for (std::map<int, Matrice<bool>>::const_iterator kt = possibilita_per_sezione[k].cbegin(); kt != possibilita_per_sezione[k].cend(); kt++)
							{
								calcolo_parziale += bin(celle_non_scavate_fuori_bordo, mine_rimanenti - (*it).first - (*kt).first);
							}
						}
					}
					calcolo_parziale += bin(celle_non_scavate_fuori_bordo, mine_rimanenti - (*it).first);
				}

				denominatore += (*it).second._righe() * calcolo_parziale;
				numeratore += somma_elementi((*it).second.colonna(j)) * calcolo_parziale;
			}
			probabilita_singola_sezione.push_back(numeratore / denominatore);
		}

		probabilita_per_sezione.push_back(probabilita_singola_sezione);
	}

	// FASE 3: SCELTA DELLE CELLE
	
	// FASE 3.1: Stampiamo ciascuna sezione con le coordinate e le probabilità corrispondenti

	for (int i = 0; i < probabilita_per_sezione.size(); i++)
	{
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;
		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			std::cout << "(" << bordo_separato[i][j].first + 1 << ", " << bordo_separato[i][j].second + 1 << ") ";
		}
		std::cout << std::endl;
		std::cout << u8"PROBABILITÀ: " << std::endl;
		for (int j = 0; j < probabilita_per_sezione[i].size(); j++)
		{
			std::cout << probabilita_per_sezione[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// FASE 3.2: Cerchiamo la probabilità più bassa; al contempo qualunque cella con probabilità quasi certa 0 o 1 viene opportunamente scavata o bandierata.

	Coord indice_minore;
	long double probabilita_minore = 1;

	for (int i = 0; i < probabilita_per_sezione.size(); i++)
	{
		for (int j = 0; j < probabilita_per_sezione[i].size(); j++)
		{
			if (probabilita_per_sezione[i][j] == 0)
			{
				std::cout << u8"Scaverò la cella (" << bordo_separato[i][j].first + 1 << ", " << bordo_separato[i][j].second + 1 << u8"), che ha probabilità certa di non essere una mina." << std::endl;
				partita.gioca(bordo_separato[i][j].first, bordo_separato[i][j].second, 'S');
			}
			else if (probabilita_per_sezione[i][j] == 1)
			{
				std::cout << u8"Metterò una bandiera sulla cella (" << bordo_separato[i][j].first + 1 << ", " << bordo_separato[i][j].second + 1 << u8"), che ha probabilità certa di essere una mina." << std::endl;
				partita.gioca(bordo_separato[i][j].first, bordo_separato[i][j].second, 'B');
			}
			else if (probabilita_per_sezione[i][j] < probabilita_minore)
			{
				indice_minore = std::make_pair(i, j);
				probabilita_minore = probabilita_per_sezione[i][j];
			}
		}
	}

	// FASE 3.3: Calcoliamo la probabilità media. Se abbiamo già scavata o bandierata delle celle nella fase 3.2 saltiamo il passaggio in toto
	// Se la probabilità più bassa del bordo è più alta della probabilità media di beccare una mina casualmente saltiamo il passaggio e restituiamo un booleano
	// che segnalerà al metodo casuale di scavare una cella non sul bordo.

	long double probabilita_media = long double(partita._campo_visibile().conta_tutti_elementi(-3)) / long double(mine_rimanenti);
	if (partita._numero_bandiere() != bandiere_precedenti || partita._campo_visibile().conta_tutti_elementi(-3) != celle_non_scavate_precedenti) return false;
	else if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elementi(-3) == celle_non_scavate_precedenti && probabilita_minore < probabilita_media)
	{
		partita.gioca(bordo_separato[indice_minore.first][indice_minore.second].first, bordo_separato[indice_minore.first][indice_minore.second].second, 'S');
		std::cout << u8"Scaverò la cella (" << bordo_separato[indice_minore.first][indice_minore.second].first + 1 << ", " << bordo_separato[indice_minore.first][indice_minore.second].second + 1 << u8"), che ha probabilità di essere una mina pari a " << probabilita_per_sezione[indice_minore.first][indice_minore.second] * 100 << "%." << std::endl;
	
		auto end = std::chrono::steady_clock::now();

		auto diff = end - start;

		std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
		return false;
	}
	return true;
}

	// Il metodo sceglie casualmente una cella (sul bordo se esiste, altrimenti una cella incognita qualunque) e la scava.
	// INPUT:
	// • (const std::vector< std::vector<Coord>>&) bordo_separato: il bordo di incognite separato per sezioni ottenuto con il metodo 'separa_incognite_bordo'
void Risolutore::metodo_casuale(const std::vector< std::vector<Coord>>& bordo_separato)
{
	if (bordo_separato.size() == 0)
	{
		std::vector<Coord> incognite_non_bordo = estrai_incognite_non_bordo();

		int random = std::rand() % incognite_non_bordo.size();

		partita.gioca(incognite_non_bordo[random].first, incognite_non_bordo[random].second, 'S');
	}
	else
	{
		
		int random1 = std::rand() % bordo_separato.size();
		int random2 = std::rand() % bordo_separato[random1].size();

		partita.gioca(bordo_separato[random1][random2].first, bordo_separato[random1][random2].second, 'S');
	}
}

// Stampa a schermo un breve report sulla situazione attuale (lo status della partita, le bandiere piazzate sul totale del mine,
// le celle rimanenti - cioè non scavate o bandierate) confrontato al passo precedente.
void Risolutore::stampa_situazione()
{
	std::cout << partita << std::endl;
	std::cout << "STATUS: " << partita._status() << std::endl;
	std::cout << "NUMERO BANDIERE: " << partita._numero_bandiere() << "/" << partita._mine() << ", NUMERO BANDIERE AL PASSO PRECEDENTE: " << bandiere_precedenti << "/" << partita._mine() << std::endl;
	std::cout << "CELLE RIMANENTI: " << partita._campo_visibile().conta_tutti_elementi(-3) << ", CELLE RIMANENTI AL PASSO PRECEDENTE: " << celle_non_scavate_precedenti << std::endl;
	system("PAUSE");
}

// Inizia il processo risolutivo, chiamando i vari metodi risolutivi definiti privatamente.
// INPUT:
// • (int) dimensione_massima: parametro di compilazione che definisce la dimensione massima delle sezioni di bordo di incognite che viene analizzato dal metodo
// probabilistico: se la lunghezza di una sezione supera tale valore, il metodo probabilistico (per motivi computazionali) viene bypassato e
// si passa al metodo casuale.
void Risolutore::risolvi(int dimensione_massima)
{
	if (dimensione_massima <= 0) throw std::domain_error("la lunghezza massima deve essere positiva");
	celle_non_scavate_precedenti = partita._campo_visibile().conta_tutti_elementi(-3);

	system("PAUSE");

	while (partita._status() == '-')
	{
		std::cout << "Applico il metodo meccanico." << std::endl << std::endl;

		metodo_meccanico();
		stampa_situazione();

		if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elementi(-3) == celle_non_scavate_precedenti)
		{
			std::cout << u8"Non ho messo nuove bandiere nè scavato celle, applico ora il metodo Gaussiano." << std::endl << std::endl;

			std::vector<std::vector<Coord> > bordo_separato = separa_incognite_bordo();

			std::vector<std::vector<Coord> > numeri_separati = separa_numeri(bordo_separato);

			metodo_gaussiano(bordo_separato, numeri_separati);
			stampa_situazione();

			if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elementi(-3) == celle_non_scavate_precedenti)
			{
				bool passare_al_metodo_casuale = true;
				
				if (std::max_element(bordo_separato.begin(), bordo_separato.end(), ordina_per_dimensione)->size() <= dimensione_massima)
				{
					std::cout << u8"Non ho messo nuove bandiere nè scavato celle, applico ora il metodo probabilistico. Potrei impiegare un po' di tempo per farlo." << std::endl << std::endl;
					passare_al_metodo_casuale = metodo_probabilistico(bordo_separato, numeri_separati);
					stampa_situazione();
				}
				else
				{
					std::cout << u8"Per motivi computazionali non applicherò il metodo probabilistico." << std::endl;
					system("PAUSE");
				}

				if (passare_al_metodo_casuale)// (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elementi(-3) == celle_non_scavate_precedenti)
				{
					std::cout << u8"Non ho messo nuove bandiere nè scavato celle, applico ora metodo casuale." << std::endl << std::endl;

					metodo_casuale(bordo_separato);

					stampa_situazione();
				}
			}
		}

		bandiere_precedenti = partita._numero_bandiere();
		celle_non_scavate_precedenti = partita._campo_visibile().conta_tutti_elementi(-3);

	}
}

#endif // __RISOLUTORE_H__