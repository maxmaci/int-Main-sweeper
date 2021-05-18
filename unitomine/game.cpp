#include <ctime>
#include <set>
#include <stack>
#include <algorithm>

#include "campo.h"
#include "menu.h"
/*
int fact(int n) {
	return n == 0 ? 1 : n * fact(n - 1);
}

int bin(int n, int k) {
	return fact(n) / (fact(k) * fact(n - k));
}
*/
long double bin(int n, int k) {
	long double res = 1;
	for (int i = 1; i <= k; i++) res *= (n + 1 - i) / long double(i);
	return res;
}

/* Classe Risolutore */
class Risolutore
{
private:
	Campo partita;						// ...
	int bandiere_precedenti;			// 0 <= bandiere_precedenti < partita._altezza() * partita._larghezza()		// TO DO: controllare per l'uguale
	int celle_non_scavate_precedenti;	// 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()
	
	/* METODI PER RICAVARE INFORMAZIONI SUL CAMPO */
	std::vector<Coord> estrai_numeri();
	std::vector<Coord> estrai_bordo();

	std::vector<std::vector<Coord> > separa_numeri();

	std::vector<std::vector<Coord> > separa_bordo();
	bool disposizione_lecita(const std::vector<Coord>&, const std::vector<Coord>&, std::vector<bool>&);

	/* METODI RISOLUTIVI */
	void metodo_meccanico();
	void metodo_gaussiano(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati);
	void metodo_probabilistico(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati);
	void metodo_probabilistico2(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati);
public:
	/* COSTRUTTORE */
	Risolutore(const Campo&);
	void aggiorna(const Campo&);
	Campo _partita() { return partita; };
	void risolve();
};

Risolutore::Risolutore(const Campo& campo)
{
	partita = campo;
	bandiere_precedenti = 0;
	celle_non_scavate_precedenti = campo._altezza() * campo._larghezza();
}

void Risolutore::aggiorna(const Campo& campo)
{
	partita = campo;
	bandiere_precedenti = campo._numero_bandiere();
	celle_non_scavate_precedenti = campo._campo_visibile().conta_tutti_elemento(-3);
}

// TO DO: mettere l'eccezione posta nel paint allegato : separare il bordo se accanto alla cella successiva ci sono solo bandiere

bool ordina_per_dimensione(std::vector<Coord> p, std::vector<Coord> q)
{
	return p.size() < q.size();
}

std::vector<Coord> Risolutore::estrai_bordo()
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

	// convertiamo l'insieme in un vettore per applicare più facilmente il passaggio successivo

	std::vector<Coord> incognite_vettore(incognite.begin(), incognite.end());

	return incognite_vettore;
}

std::vector<Coord> Risolutore::estrai_numeri()
{
	std::vector<Coord> numeri;

	// recuperiamo tutte i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno: immagazziniamo tutto in un vettore

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

std::vector<std::vector<Coord> > Risolutore::separa_bordo()
{
	auto start = std::chrono::steady_clock::now();

	std::vector<Coord> incognite = estrai_bordo();

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

			if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && !trova_elemento(incognite_separate_regione, cella) && trova_elemento(incognite, cella)) //   && partita.conta_numeri_vicini(cella.first, cella.second) == a
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
				
				//coda.push(Coord(cella.first - 1, cella.second)); // NORD
				//coda.push(Coord(cella.first, cella.second - 1)); // OVEST
				//coda.push(Coord(cella.first, cella.second + 1)); // EST
				//coda.push(Coord(cella.first + 1, cella.second)); // SUD

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

	/*
	std::sort(incognite_separate.begin(), incognite_separate.end(), ordina_per_dimensione);

	for (int i = 0; i < incognite_separate.size(); i++)
	{
		for (int j = 0; j < incognite_separate[i].size(); j++)
		{
			std::cout << "(" << incognite_separate[i][j].first + 1 << ", " << incognite_separate[i][j].second + 1 << ")";
		}
		std::cout << std::endl;
	}
	system("PAUSE");
	*/
	return incognite_separate;
}

std::vector<std::vector<Coord> > Risolutore::separa_numeri()
{
	std::vector<Coord> numeri = estrai_numeri();

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo();

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

void Risolutore::metodo_meccanico()
{
	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			// PRIMA FASE: mette le bandierine se attorno al numero n trova n celle non scavata (con potenzialmente già delle bandierine
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) != 0 && partita.conta_non_scavati_vicini(i, j) + partita.conta_bandiere_vicine(i, j) == partita._campo_visibile()[i][j])	// TO DO: controllare se l'ultima condizione cambia qualcosa o menopausa
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
			if (partita._status() == 'V')
			{
				std::cout << u8"Il mio lavoro qua è finito\n";
				auto end = std::chrono::steady_clock::now();

				auto diff = end - start;

				std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
				return;
			}
			// SECONDA FASE: scava tutto ciò che non è bandierinato attorno ad una cella numerata con n se sono già presenti esattamente n bandierine attorno
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

void Risolutore::metodo_gaussiano(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati)
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1: creazione della matrice di incognite

	for (int a = 0; a < numeri_separati.size(); a++)
	{
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

		// FASE 2: riduzione gaussiana della matrice.

		std::pair<Matrice<int>, std::vector<int>> matrice_completa = matrice.riduzione_gaussiana_con_termine_noto(termine_noto);

		// FASE 3: controllo delle soluzioni.
		// Consideriamo solamente le righe non vuote (il sistema originale non sempre ha soluzioni - anzi, quasi mai!)

		Matrice<int> matrice_ridotta(0, matrice_completa.first._colonne());
		std::vector<int> termine_noto_ridotto;
		for (int i = 0; i < matrice_completa.first._righe(); i++)
		{
			for (int j = 0; j < matrice_completa.first._colonne(); j++)
			{
				if (matrice_completa.first[i][j] != 0)
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
				// tutti i termini con coefficiente -1 sono mine, tutti quelli con coefficiente 1 non lo sono

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
						// TO DO: rendere l'operatore - parte di una specifica classe vettore che funge da base ad una classe matrix adattata - > campo deve diventare solo nel caso della classe Gioco (che potremmo rinominare Campo).
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
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second - 1))	&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second - 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second))		&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second + 1))	&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first - 1, numeri_separati[i].second + 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second - 1))		&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second - 1))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second + 1))		&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first, numeri_separati[i].second + 1))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second - 1))	&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second - 1))])	conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second))		&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second))])		conta_mine_cella++;
		if (trova_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second + 1))	&& disposizione[trova_indice_elemento(bordo_separato, Coord(numeri_separati[i].first + 1, numeri_separati[i].second + 1))])	conta_mine_cella++;
	
		if (conta_mine_cella != partita._campo_visibile()[numeri_separati[i].first][numeri_separati[i].second] - partita.conta_bandiere_vicine(numeri_separati[i].first, numeri_separati[i].second)) return false;
	}

	return true;

}

void Risolutore::metodo_probabilistico(const std::vector< std::vector<Coord>>& bordo_separato, const std::vector< std::vector<Coord>>& numeri_separati)
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1.1 : Preparazione. Prendiamo il bordo di celle incognite (quelle accanto a dei numeri e il cui numero
	// di bandiere non è sufficiente) e segmentiamo le celle in modo che siano 

	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			std::cout << "(" << numeri_separati[i][j].first + 1 << ", " << numeri_separati[i][j].second + 1 << ")";
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		//std::cout << std::endl;
		mine_max_separate.push_back(mine_max);
	}

	// FASE 1.2:

	int mine_rimanenti = partita._mine() - partita._numero_bandiere();
	
	// FASE 1.3:

	std::vector<std::map<int, Matrice<bool>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::vector<Coord> sezione_numeri = numeri_separati[a];
		std::vector<Coord> sezione_bordo = bordo_separato[a];

		std::map<int, Matrice<bool>> disposizioni_per_sezione;

		//std::vector<bool> disposizione(sezione_bordo.size());

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(sezione_bordo.size()));
		estremi.push_back(mine_rimanenti);

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << estremi[0] << std::endl;
		std::cout << "singola_disposizione.size(): " << estremi[1] << std::endl;
		std::cout << "mine_rimanenti: " << estremi[2] << std::endl;

		for (int i = 0; i < estremo; i++)
		{
			std::vector<bool> disposizione(sezione_bordo.size());

			for (int j = 0; j < i; j++)
			{
				disposizione[j] = true;
			}

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

	std::cout << u8"N° SEZIONI: " << possibilita_per_sezione.size() << std::endl;

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;
		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			std::cout << "(" << bordo_separato[i][j].first + 1 << ", " << bordo_separato[i][j].second + 1 << ") ";
		}
		std::cout << std::endl;

		for (std::map<int, Matrice<bool>>::iterator it = possibilita_per_sezione[i].begin(); it != possibilita_per_sezione[i].end(); it++)
		{
			std::cout << u8"N° MINE: " << (*it).first << std::endl;
			std::cout << u8"N° COMBINAZIONI: " << (*it).second._righe() << std::endl;

			std::cout << (*it).second;
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	// FASE 2.1: contiamo tutte le celle non scavate di cui non abbiamo alcuna informazione

	int celle_non_scavate_fuori_bordo = partita._campo_visibile().conta_tutti_elemento(-3);

	for (int i = 0; i < bordo_separato.size(); i++)
	{
		celle_non_scavate_fuori_bordo -= static_cast<int>(bordo_separato[i].size());
	}

	std::cout << celle_non_scavate_fuori_bordo << std::endl;

	// FASE 2.2: per ogni singola cella del bordo calcoliamo la sua probabilità di essere una mina
	// se 

	std::vector<std::vector<long double> > probabilita_per_sezione;

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{
		std::vector<long double> probabilita_singola_sezione;

		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			long double numeratore = 0;
			long double denominatore = 0;
			
			for (std::map<int, Matrice<bool>>::iterator it = possibilita_per_sezione[i].begin(); it != possibilita_per_sezione[i].end(); it++)
			{
				long double calcolo_parziale = 0;
				
				for (int k = 0; k < possibilita_per_sezione.size(); k++)
				{
					if (k != i)
					{
						for (std::map<int, Matrice<bool>>::iterator kt = possibilita_per_sezione[k].begin(); kt != possibilita_per_sezione[k].end(); kt++)
						{
							calcolo_parziale += bin(celle_non_scavate_fuori_bordo, mine_rimanenti - (*it).first - (*kt).first);
						}
					}
					//else
					//{
					//	calcolo_parziale += bin(celle_non_scavate_fuori_bordo, mine_rimanenti - (*it).first);
					//}

				}
				calcolo_parziale += bin(celle_non_scavate_fuori_bordo, mine_rimanenti - (*it).first);

				denominatore += (*it).second._righe() * calcolo_parziale;
				std::cout << somma_elementi((*it).second.colonna(j)) << " ";
				numeratore += somma_elementi((*it).second.colonna(j)) * calcolo_parziale;
			}
			std::cout << std::endl;
			probabilita_singola_sezione.push_back(numeratore / denominatore);
		}

		probabilita_per_sezione.push_back(probabilita_singola_sezione);
	}

	for (int i = 0; i < probabilita_per_sezione.size(); i++)
	{		
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;
		for (int j = 0; j < bordo_separato[i].size(); j++)
		{
			std::cout << "(" << bordo_separato[i][j].first + 1 << ", " << bordo_separato[i][j].second + 1 << ") " ;
		}
		std::cout << std::endl;
		std::cout << u8"PROBABILITà: " << std::endl;
		for (int j = 0; j < probabilita_per_sezione[i].size(); j++)
		{
			std::cout << probabilita_per_sezione[i][j] << " ";
		}
		std::cout << std::endl;
	}
	
	Coord indice_minore;
	long double probabilita_minore = 1;

	for (int i = 0; i < probabilita_per_sezione.size(); i++)
	{
		for (int j = 0; j < probabilita_per_sezione[i].size(); j++)
		{
			if (probabilita_per_sezione[i][j] == 0)
			{
				std::cout << "(" << bordo_separato[indice_minore.first][indice_minore.second].first + 1 << ", " << bordo_separato[indice_minore.first][indice_minore.second].second + 1 << ") " << std::endl;
				partita.gioca(bordo_separato[i][j].first, bordo_separato[i][j].second, 'S');
			}
			else if (probabilita_per_sezione[i][j] == 1)
			{
				std::cout << "(" << bordo_separato[indice_minore.first][indice_minore.second].first + 1 << ", " << bordo_separato[indice_minore.first][indice_minore.second].second + 1 << ") " << std::endl;
				partita.gioca(bordo_separato[i][j].first, bordo_separato[i][j].second, 'B');
			}
			else if (probabilita_per_sezione[i][j] < probabilita_minore)
			{
				indice_minore = std::make_pair(i, j);
				probabilita_minore = probabilita_per_sezione[i][j];
			}
		}
	}
	long double probabilita_media = 1; // TO DO: definire la probabilità media come la probabilità che uno becchi una cella non sul bordo a casso e che essa sia una mina.
	if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
	{
		if (probabilita_minore < probabilita_media)
		{
			partita.gioca(bordo_separato[indice_minore.first][indice_minore.second].first, bordo_separato[indice_minore.first][indice_minore.second].second, 'S');
			std::cout << "(" << bordo_separato[indice_minore.first][indice_minore.second].first + 1 << ", " << bordo_separato[indice_minore.first][indice_minore.second].second + 1 << ") " << std::endl;

		}
		else
		{
			// TO DO: scegliere una cella non sul bordo completamente a random
		}
	}

	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
}

void Risolutore::risolve()
{
	celle_non_scavate_precedenti = partita._campo_visibile().conta_tutti_elemento(-3);

	system("PAUSE");

	while (partita._status() == '-')
	{
		std::cout << "Applico il metodo meccanico." << std::endl;

		metodo_meccanico();
		std::cout << partita << std::endl;
		std::cout << "STATUS: " << partita._status() << std::endl;
		std::cout << "NUMERO BANDIERE: " << partita._numero_bandiere() << "/" << partita._mine() << ", bandiere precedenti: " << bandiere_precedenti << "/" << partita._mine() << std::endl;
		std::cout << "CELLE NON SCAVATE: " << partita._campo_visibile().conta_tutti_elemento(-3) << ", non scavate precedenti: " << celle_non_scavate_precedenti << std::endl;

		system("PAUSE");
		if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
		{
			std::cout << "Non ho messo nuove bandiere nè scavato celle, passo al metodo Gaussiano." << std::endl;

			std::vector<std::vector<Coord> > bordo_separato = separa_bordo();

			std::vector<std::vector<Coord> > numeri_separati = separa_numeri();

			metodo_gaussiano(bordo_separato, numeri_separati);
			std::cout << partita << std::endl;
			std::cout << "STATUS: " << partita._status() << std::endl;
			std::cout << "NUMERO BANDIERE: " << partita._numero_bandiere() << "/" << partita._mine() << ", bandiere precedenti: " << bandiere_precedenti << "/" << partita._mine() << std::endl;
			std::cout << "CELLE NON SCAVATE: " << partita._campo_visibile().conta_tutti_elemento(-3) << ", non scavate precedenti: " << celle_non_scavate_precedenti << std::endl;

			system("PAUSE");

			if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
			{
				std::cout << "Non ho messo nuove bandiere nè scavato celle, passo al metodo probabilistico." << std::endl;

				metodo_probabilistico(bordo_separato, numeri_separati);

				//std::vector<Coord> celle_incognite = estrai_celle_incognite();
				//int random = std::rand() % celle_incognite.size();
				//partita.gioca(celle_incognite[random].first, celle_incognite[random].second, 'S');
				std::cout << partita << std::endl;
				std::cout << "STATUS: " << partita._status() << std::endl;
				std::cout << "NUMERO BANDIERE: " << partita._numero_bandiere() << "/" << partita._mine() << ", bandiere precedenti: " << bandiere_precedenti << "/" << partita._mine() << std::endl;
				std::cout << "CELLE NON SCAVATE: " << partita._campo_visibile().conta_tutti_elemento(-3) << ", non scavate precedenti: " << celle_non_scavate_precedenti << std::endl;

				system("PAUSE");
			}
		}

		bandiere_precedenti = partita._numero_bandiere();
		celle_non_scavate_precedenti = partita._campo_visibile().conta_tutti_elemento(-3);

	}
}

int main()
{

/* OPZIONI ESCLUSIVE PER WINDOWS PER FAR SÌ CHE VISUALIZZI CARATTERI UNICODE */

#ifdef _WIN32
	// Mette la code page della console in UTF8 cosicché la console sa interpretare le stringhe unicode
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	// Abilita il buffering per evitare che Visual Studio tagli le sequenze di byte UTF-8
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	// Mette l'output in modo da obbligare l'uso di sequenze virtuali del terminale.
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return GetLastError();
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return GetLastError();
	}

#endif

/* VARIABILI DI GIOCO/OPZIONI */

	// resetta il clock per rendere la generazione (pseudo)-casuale più casuale

	std::srand(static_cast<int>( std::time(nullptr) ));

	// Variabili per comandi di gioco: coordinate della cella + comando dell'azione

	int riga = 1;
	int colonna = 1;
	char comando = 'S';

	/* FLAG per controllare i loop di gioco */
	
	// uscita_programma : se è 'true', bypassa tutti i loop successivi ed esce dal programma

	bool uscita_programma = false;

	bool in_gioco = false;

	bool in_risolutore = false;

	// prima_mossa_effettuata : ad inizio partita è posta come 'false'; quando si fa la prima mossa entrando in un IF si randomizza
	// il campo di mine in modo da generare uno spazio vuoto (o con numeri) 3x3 attorno alla casella scelta, poi viene settata come 'true'
	// in modo da non rigenerare le mine ogni volta 

	bool prima_mossa_effettuata = false;

	bool campo_generato = false;

	/* LOOP DEL PROGRAMMA: finchè non gli viene dato l'input di uscita del menù tornerà qui */

	// inizializza la tabella di gioco

	while (!uscita_programma)
	{
		Campo partita(9, 9, 10);
		Risolutore risolutore(partita);

		/* SCHERMATA PRINCIPALE */

		// Piccola ASCII art con il titolo; il prefisso R"( ... )" indica che il contenuto delle parentesi
		// viene stampato così com'è, quindi '\' NON è prefisso per alcun "code" e.g. \n o \t.

			std::cout << R"(
  _    _       _ _        __  __ _            
 | |  | |     (_) |      |  \/  (_)           
 | |  | |_ __  _| |_ ___ | \  / |_ _ __   ___ 
 | |  | | '_ \| | __/ _ \| |\/| | | '_ \ / _ \
 | |__| | | | | | || (_) | |  | | | | | |  __/
  \____/|_| |_|_|\__\___/|_|  |_|_|_| |_|\___|
                                                                                      
)";
		std::cout << "Un gioco in C++ programmato da Bertolotti Massimo e Buffa Guido." << std::endl << std::endl;


		/* MENU */

			// Inserendo come input il corrispettivo numero è possibile scegliere una modalità fra quelle predefinite
			// (PRINCIPANTE, INTERMEDIO, ESPERTO), la EPIC MODE (un piccolo Easter Egg), creare una tabella personalizzata
			// OPPURE uscire dal gioco. Se il comando non è uno di questi sei OPPURE il numero 42 (altro piccolo Easter Egg) non fa nulla
			// eccetto segnalare al giocatore che il commando non è riconosciuto e ripropone il prompt.

		/* TO DO: mettere questo lettore input come l'altro in gioco */

		std::cout << "Seleziona un opzione scrivendo il numero corrispondendente nel prompt:" << std::endl;
		std::cout	<< u8"• PRINCIPIANTE \t\t(Campo  9 x  9, 10 mine) \t(1)\n"
					<< u8"• INTERMEDIO \t\t(Campo 16 x 16, 40 mine) \t(2)\n"
					<< u8"• ESPERTO \t\t(Campo 16 x 30, 99 mine) \t(3)\n"
					<< u8"• EPIC MODE \t\t(Campo ?? x ??, ?? mine)\t(4)\n"
					<< u8"• PERSONALIZZATO\t\t\t\t\t(5)\n"
					<< u8"• ESCI DAL GIOCO \t\t\t\t\t(6)" << std::endl;
		
		menu_principale(partita, uscita_programma, in_gioco);

	/* TO DO: menù per chiedere risolutore */

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			while (!in_risolutore && partita._status() == '-')
			{
				std::cout << partita << std::endl;
				std::cout << u8"Inserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt 'O'." << std::endl;
				std::cout << "Hai messo " << partita._numero_bandiere() << " bandiere su " << partita._mine() << " mine presenti." << std::endl;
				
				interpreta_mossa(partita, riga, colonna, comando, in_risolutore, campo_generato);
				
				// Esce dalla partita per le opzioni
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima (tranne quando si ricomincia la partita), il campo viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!prima_mossa_effettuata)
				{
					partita.randomizza_campo(riga - 1, colonna - 1);
					campo_generato = true;
					prima_mossa_effettuata = true;
				}
				
				partita.gioca(riga - 1, colonna - 1, comando);
			}

			while (!uscita_programma && in_risolutore)
			{
				std::cout << partita << std::endl;
				//inizializza_risolutore(partita);
				//std::cout << partita << std::endl;

				risolutore.aggiorna(partita);
				risolutore.risolve();
				partita = risolutore._partita();
				break;
			}

			// NON entra nelle opzioni se il gioco è stato vinto o perso.

			if (partita._status() == '-')
			{
				std::cout << "OPZIONI:\n"
					<< u8"• Torna al gioco.\t\t(1)\n"
					<< u8"• Ricomincia la partita.\t(2)\n"
					<< u8"• Rivela il campo.\t\t(3)\n"
					<< u8"• Torna al menù principale.\t(4)\n"
					<< u8"• Esci dal gioco.\t\t(5)" << std::endl;

				menu_opzioni(partita, uscita_programma, in_risolutore, in_gioco, campo_generato);

			}
			else
			{
				std::cout << partita << std::endl;

				if (partita._status() == 'S')
				{
					std::cout << u8"OH NO! Hai perso. ☹\nCosa vuoi fare ora?\n";
				}
				else
				{
					std::cout << u8"HAI VINTO! ☺\nCosa vuoi fare ora?\n";
				}

				menu_opzioni_breve(partita, uscita_programma, in_risolutore, in_gioco);

			}

		}

		campo_generato = false;
		prima_mossa_effettuata = false;

	}
}
