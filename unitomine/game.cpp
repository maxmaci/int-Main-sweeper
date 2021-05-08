#include <ctime>
#include <set>
#include <stack>
#include <algorithm>

#include "campo.h"
#include "menu.h"



/* Classe Risolutore */
class Risolutore
{
private:
	Campo partita;				// ...
	int bandiere_precedenti;	// 0 <= bandiere_precedenti < partita._altezza() * partita._larghezza()		// TO DO: controllare per l'uguale
	int celle_non_scavate_precedenti; // 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()
	
	/* METODI PER RICAVARE INFORMAZIONI SUL CAMPO */
	std::pair<std::vector<Coord>, std::vector<Coord> > estrai_numeri_bordo_celle_incognite();
	std::vector<Coord> estrai_numeri_bordo();
	std::vector<Coord> estrai_celle_incognite();
	std::vector< std::vector<Coord> > separa_bordo();
	std::vector< std::vector<Coord> > separa_numeri();
	bool disposizione_lecita(std::vector<bool>, std::vector<Coord>);

	/* METODI RISOLUTIVI */
	void metodo_meccanico();
	void metodo_gaussiano();
	void metodo_probabilistico();
	//void metodo_probabilistico_ricorsivo(Campo&, std::map<int, std::vector<std::vector<bool> > >&, std::vector<Coord>, int);

public:
	/* COSTRUTTORE */
	Risolutore(const Campo&);
	void aggiorna(const Campo&);
	void risolve();
};

Risolutore::Risolutore(const Campo& campo)
{
	partita = campo;
	bandiere_precedenti = 0;
	celle_non_scavate_precedenti = 0;
}

void Risolutore::aggiorna(const Campo& campo)
{
	partita = campo;
	bandiere_precedenti = campo._numero_bandiere();
	celle_non_scavate_precedenti = campo._campo_visibile().conta_tutti_elemento(-3);
}

void Risolutore::metodo_meccanico()
{
	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			// PRIMA FASE: mette le bandierine se attorno al numero n trova n celle non scavata (con potenzialmente già delle bandierine
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

std::pair<std::vector<Coord>, std::vector<Coord> > Risolutore::estrai_numeri_bordo_celle_incognite()
{
	std::vector<Coord> numeri;

	std::set<Coord> incognite;
	
	// recuperiamo tutte i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno: immagazziniamo tutto in un vettore

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
			{
				numeri.push_back(Coord(i, j));

				if (partita._campo_visibile().indici_leciti(i - 1, j - 1)	&& partita._campo_visibile()[i - 1][j - 1] == -3)	incognite.insert(Coord(i - 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i - 1, j)		&& partita._campo_visibile()[i - 1][j] == -3)		incognite.insert(Coord(i - 1, j));
				if (partita._campo_visibile().indici_leciti(i - 1, j + 1)	&& partita._campo_visibile()[i - 1][j + 1] == -3)	incognite.insert(Coord(i - 1, j + 1));
				if (partita._campo_visibile().indici_leciti(i, j - 1)		&& partita._campo_visibile()[i][j - 1] == -3)		incognite.insert(Coord(i, j - 1));
				if (partita._campo_visibile().indici_leciti(i, j + 1)		&& partita._campo_visibile()[i][j + 1] == -3)		incognite.insert(Coord(i, j + 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j - 1)	&& partita._campo_visibile()[i + 1][j - 1] == -3)	incognite.insert(Coord(i + 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j)		&& partita._campo_visibile()[i + 1][j] == -3)		incognite.insert(Coord(i + 1, j));
				if (partita._campo_visibile().indici_leciti(i + 1, j + 1)	&& partita._campo_visibile()[i + 1][j + 1] == -3)	incognite.insert(Coord(i + 1, j + 1));

			}
		}
	}
	
	// convertiamo l'insieme in un vettore per applicare più facilmente il passaggio successivo

	std::vector<Coord> incognite_vettore(incognite.begin(), incognite.end());

	return std::pair<std::vector<Coord>, std::vector<Coord> >(numeri, incognite_vettore);
}

std::vector<Coord> Risolutore::estrai_celle_incognite()
{
	std::set<Coord> incognite;

	// recuperiamo tutte i numeri che hanno ancora delle celle non scavate (e non bandierate) attorno: immagazziniamo tutto in un vettore

	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{
			if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
			{
				if (partita._campo_visibile().indici_leciti(i - 1, j - 1) && partita._campo_visibile()[i - 1][j - 1] == -3)	incognite.insert(Coord(i - 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i - 1, j) && partita._campo_visibile()[i - 1][j] == -3)		incognite.insert(Coord(i - 1, j));
				if (partita._campo_visibile().indici_leciti(i - 1, j + 1) && partita._campo_visibile()[i - 1][j + 1] == -3)	incognite.insert(Coord(i - 1, j + 1));
				if (partita._campo_visibile().indici_leciti(i, j - 1) && partita._campo_visibile()[i][j - 1] == -3)		incognite.insert(Coord(i, j - 1));
				if (partita._campo_visibile().indici_leciti(i, j + 1) && partita._campo_visibile()[i][j + 1] == -3)		incognite.insert(Coord(i, j + 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j - 1) && partita._campo_visibile()[i + 1][j - 1] == -3)	incognite.insert(Coord(i + 1, j - 1));
				if (partita._campo_visibile().indici_leciti(i + 1, j) && partita._campo_visibile()[i + 1][j] == -3)		incognite.insert(Coord(i + 1, j));
				if (partita._campo_visibile().indici_leciti(i + 1, j + 1) && partita._campo_visibile()[i + 1][j + 1] == -3)	incognite.insert(Coord(i + 1, j + 1));

			}
		}
	}

	// convertiamo l'insieme in un vettore per applicare più facilmente il passaggio successivo

	std::vector<Coord> incognite_vettore(incognite.begin(), incognite.end());

	return incognite_vettore;
}

std::vector<Coord> Risolutore::estrai_numeri_bordo()
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

void Risolutore::metodo_gaussiano()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1: creazione della matrice di incognite
	
	std::pair<std::vector<Coord>, std::vector<Coord>> numeri_e_incognite = estrai_numeri_bordo_celle_incognite();

	std::vector<Coord> coordinate_numeri_bordo = estrai_numeri_bordo();
	std::vector<Coord> incognite_bordo = estrai_celle_incognite();

	std::vector<int> numeri_bordo;
	
	Matrice<int> matrice(static_cast<int>( coordinate_numeri_bordo.size() ), static_cast<int>( incognite_bordo.size() ));
	
	for (int n = 0; n < matrice._righe(); n++)
	{
		int i = coordinate_numeri_bordo[n].first;
		int j = coordinate_numeri_bordo[n].second;

		numeri_bordo.push_back(partita._campo_visibile()[i][j] - partita.conta_bandiere_vicine(i,j));
				
		if (trova_elemento(incognite_bordo, Coord (i - 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord (i - 1, j - 1))]	= 1;
		if (trova_elemento(incognite_bordo, Coord (i - 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord (i - 1, j))]		= 1;
		if (trova_elemento(incognite_bordo, Coord (i - 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord (i - 1, j + 1))]	= 1;
		if (trova_elemento(incognite_bordo, Coord (i, j - 1)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord (i, j - 1))]		= 1;
		if (trova_elemento(incognite_bordo, Coord(i, j + 1)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i, j + 1))]			= 1;
		if (trova_elemento(incognite_bordo, Coord(i + 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j - 1))]		= 1;
		if (trova_elemento(incognite_bordo, Coord(i + 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j))]			= 1;
		if (trova_elemento(incognite_bordo, Coord(i + 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j + 1))]		= 1;
	}

	// FASE 2: riduzione gaussiana della matrice.

	matrice.riduzione_gaussiana(numeri_bordo);

	// FASE 3: sostituzione all'indietro e controllo delle soluzioni.

	//Consideriamo solamente le righe non vuote (il sistema originale non sempre ha soluzioni - anzi, quasi mai!)

	Matrice<int> matrice_ridotta(0, matrice._colonne());
	std::vector<int> termine_noto;
	std::vector<Coord> coordinate_numeri_bordo_ridotta;
	for (int i = 0; i < matrice._righe(); i++)
	{
		for (int j = 0; j < matrice._colonne(); j++)
		{
			if (matrice[i][j] != 0)
			{
				matrice_ridotta.push_back(matrice[i]);
				termine_noto.push_back(numeri_bordo[i]);
				coordinate_numeri_bordo_ridotta.push_back(coordinate_numeri_bordo[i]);
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

		if (upper_bound == termine_noto[i])
		{
			
			for (int j = 0; j < matrice_ridotta._colonne(); j++)
			{
				if (matrice_ridotta[i][j] > 0)
				{
					partita.gioca(incognite_bordo[j].first, incognite_bordo[j].second, 'B');
				}
				else if (matrice_ridotta[i][j] < 0)
				{
					partita.gioca(incognite_bordo[j].first, incognite_bordo[j].second, 'S');
				}
			}
		}
		else if (lower_bound == termine_noto[i])
		{
			// tutti i termini con coefficiente -1 sono mine, tutti quelli con coefficiente 1 non lo sono

			for (int j = 0; j < matrice_ridotta._colonne(); j++)
			{
				if (matrice_ridotta[i][j] < 0)
				{
					partita.gioca(incognite_bordo[j].first, incognite_bordo[j].second, 'B');
				}
				else if (matrice_ridotta[i][j] > 0)
				{
					partita.gioca(incognite_bordo[j].first, incognite_bordo[j].second, 'S');
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
				termine_noto[b] -= termine_noto[i];
			}
		}

	}
	
	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}


bool compara(std::pair<Coord, std::pair<double, double>> p, std::pair<Coord, std::pair<double, double>> q)
{
	return p.second.first < q.second.first;
}

std::vector<std::vector<Coord> > Risolutore::separa_bordo()
{
	auto start = std::chrono::steady_clock::now();

	std::vector<Coord> incognite = estrai_celle_incognite();

	std::vector<std::vector<Coord> > incognite_separate;

	std::queue<Coord> coda;
	coda.push(incognite[0]);

	while (incognite.size() != 0)
	{
		for (int a = 1; a < 9; a++)
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

				if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && !trova_elemento(incognite_separate_regione, cella) && trova_elemento(incognite, cella) && partita.conta_numeri_vicini(cella.first, cella.second) == a) //  
				{	
					std::cout << "(" << cella.first + 1 << ", " << cella.second + 1 << ") ";

					incognite_separate_regione.push_back(cella);
					incognite.erase(incognite.begin() + trova_indice_elemento(incognite, cella));

					//coda.push(Coord(cella.first - 1, cella.second - 1));
					coda.push(Coord(cella.first - 1, cella.second));
					//coda.push(Coord(cella.first - 1, cella.second + 1));
					coda.push(Coord(cella.first, cella.second - 1));
					coda.push(Coord(cella.first, cella.second + 1));
					//coda.push(Coord(cella.first + 1, cella.second - 1));
					coda.push(Coord(cella.first + 1, cella.second));
					//coda.push(Coord(cella.first + 1, cella.second + 1));

				}
			}

			if (incognite_separate_regione.size() != 0)
			{
				incognite_separate.push_back(incognite_separate_regione);
			}
			
		}
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return incognite_separate;
}

/*
std::vector<std::vector<Coord> > Risolutore::separa_bordo2()
{
	auto start = std::chrono::steady_clock::now();
	
	std::vector<Coord> incognite = estrai_celle_incognite();

	std::vector<std::vector<Coord> > incognite_separate;

	std::queue<Coord> coda;
	coda.push(incognite[0]);

	while (incognite.size() != 0)
	{
		std::vector<Coord> incognite_separate_regione;
		
		if (coda.size() == 0)
		{
			coda.push(incognite[0]);
		}

		while (coda.size() != 0)
		{
			Coord cella = coda.front();
			coda.pop();

			if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && !trova_elemento(incognite_separate_regione, cella) && trova_elemento(incognite, cella) && partita.conta_se_numeri_vicini(cella.first, cella.second))
			{
				incognite_separate_regione.push_back(cella);
				incognite.erase(incognite.begin() + trova_indice_elemento(incognite, cella));

				coda.push(Coord(cella.first - 1, cella.second - 1));
				coda.push(Coord(cella.first - 1, cella.second));
				coda.push(Coord(cella.first - 1, cella.second + 1));
				coda.push(Coord(cella.first, cella.second - 1));
				coda.push(Coord(cella.first, cella.second + 1));
				coda.push(Coord(cella.first + 1, cella.second - 1));
				coda.push(Coord(cella.first + 1, cella.second));
				coda.push(Coord(cella.first + 1, cella.second + 1));

			}
		}

		incognite_separate.push_back(incognite_separate_regione);
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return incognite_separate;
}
*/

std::vector<std::vector<Coord> > Risolutore::separa_numeri()
{
	auto start = std::chrono::steady_clock::now();

	std::vector<Coord> numeri = estrai_numeri_bordo();

	std::vector<std::vector<Coord> > numeri_separati;

	std::vector<std::vector<Coord> > incognite_separate = separa_bordo();

	for (int i = 0; i < incognite_separate.size(); i++)
	{
		std::vector<Coord> numeri_sezione;

		for (int j = 0; j < incognite_separate[i].size(); j++)
		{
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second - 1)))	numeri_sezione.push_back(Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second - 1));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second)))		numeri_sezione.push_back(Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second + 1)))	numeri_sezione.push_back(Coord(incognite_separate[i][j].first - 1, incognite_separate[i][j].second + 1));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first, incognite_separate[i][j].second - 1)))		numeri_sezione.push_back(Coord(incognite_separate[i][j].first, incognite_separate[i][j].second - 1));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first, incognite_separate[i][j].second + 1)))		numeri_sezione.push_back(Coord(incognite_separate[i][j].first, incognite_separate[i][j].second + 1));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second - 1)))	numeri_sezione.push_back(Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second - 1));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second)))		numeri_sezione.push_back(Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second));
			if (trova_elemento(numeri, Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second + 1)))	numeri_sezione.push_back(Coord(incognite_separate[i][j].first + 1, incognite_separate[i][j].second + 1));
		}

		numeri_separati.push_back(numeri_sezione);
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return numeri_separati;
}

bool Risolutore::disposizione_lecita(std::vector<bool> disposizione_booleana, std::vector<Coord> incognite)
{
	for (int i = 0; i < disposizione_booleana.size(); i++)
	{
		if (disposizione_booleana[i])
		{
			incognite[i].first;
		}
	}
	return true;
}

void Risolutore::metodo_probabilistico()
{	
	auto start = std::chrono::steady_clock::now();

	std::vector<std::vector<Coord> > numeri_separati = separa_numeri();

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

	std::vector<std::vector<Coord> > incognite_separate = separa_bordo();

	for (int i = 0; i < incognite_separate.size(); i++)
	{
		for (int j = 0; j < incognite_separate[i].size(); j++)
		{
			std::cout << "(" << incognite_separate[i][j].first + 1 << ", " << incognite_separate[i][j].second + 1<< ") ";
		}
		std::cout << std::endl;
	}

	system("PAUSE");

	
	// FASE 1: generare tutte le possibili disposizioni delle mine sezione per sezione

	std::vector<std::vector<std::vector<bool> > > possibili_disposizioni_separate;

	for (int k = 0; k < incognite_separate.size(); k++)
	{
		std::vector<bool> singola_disposizione (incognite_separate[k].size());
		std::set<std::vector<bool> > possibili_disposizioni;

		int estremo = (std::min)(mine_max_separate[k], static_cast<int>(singola_disposizione.size()));

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << mine_max_separate[k] << std::endl;
		std::cout << "singola_disposizione.size(): " << singola_disposizione.size() << std::endl;
		
		for (int i = 0; i < estremo; i++)
		{
			singola_disposizione[i] = true;

			std::sort(singola_disposizione.begin(), singola_disposizione.end());

			do
			{
				possibili_disposizioni.insert(singola_disposizione); // if (disposizione_lecita(singola_disposizione, incognite_separate[k])) 
				
				for (int j = 0; j < singola_disposizione.size(); j++)
				{
					std::cout << singola_disposizione[j];
				}
				std::cout << std::endl;

			} while (std::next_permutation(singola_disposizione.begin(), singola_disposizione.end()));
		}

		possibili_disposizioni_separate.push_back(std::vector< std::vector<bool> > (possibili_disposizioni.cbegin(), possibili_disposizioni.cend() ));

	}

	std::cout << "STAMPO ROBA ORA: " << std::endl;

	for (int i = 0; i < possibili_disposizioni_separate.size(); i++)
	{
		for (int j = 0; j < possibili_disposizioni_separate[i].size(); j++)
		{
			for (int k = 0; k < possibili_disposizioni_separate[i][j].size(); k++)
			{
				std::cout << possibili_disposizioni_separate[i][j][k];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
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

			metodo_gaussiano();
			std::cout << partita << std::endl;
			std::cout << "STATUS: " << partita._status() << std::endl;
			std::cout << "NUMERO BANDIERE: " << partita._numero_bandiere() << "/" << partita._mine() << ", bandiere precedenti: " << bandiere_precedenti << "/" << partita._mine() << std::endl;
			std::cout << "CELLE NON SCAVATE: " << partita._campo_visibile().conta_tutti_elemento(-3) << ", non scavate precedenti: " << celle_non_scavate_precedenti << std::endl;

			system("PAUSE");

			if (partita._numero_bandiere() == bandiere_precedenti && partita._campo_visibile().conta_tutti_elemento(-3) == celle_non_scavate_precedenti)
			{
				std::cout << "Non ho messo nuove bandiere nè scavato celle, passo al metodo probabilistico." << std::endl;

				metodo_probabilistico();
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

	/* LOOP DEL PROGRAMMA: finchè non gli viene dato l'input di uscita del menù tornerà qui */

	// inizializza la tabella di gioco

	while (!uscita_programma)
	{
		Campo partita(9, 9, 10);
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
		
		menu_principale(partita, uscita_programma);
		
		menu_risolutore(in_risolutore, in_gioco);

	/* TO DO: menù per chiedere risolutore */

	/* LOOP DI GIOCO*/
	// A meno di uscire per tornare al menù principale, tutte le azioni vengono svolte in questo loop
		while (!uscita_programma && in_gioco)
		{
			while (partita._status() == '-')
			{
				std::cout << partita << std::endl;
				std::cout << u8"Inserisci una mossa nel formato 'riga colonna comando' oppure anche solo 'riga colonna' se intendi scavare quella cella.\nPer il menù delle opzioni, scrivi nel prompt 'O'." << std::endl;
				std::cout << "Hai messo " << partita._numero_bandiere() << " bandiere su " << partita._mine() << " mine presenti." << std::endl;
				
				interpreta_mossa(partita, riga, colonna, comando);
				
				// Esce dalla partita per le opzioni
				if (comando == 'O') break;

				/* RANDOMIZZATORE: */
				// Se la mossa è la prima (tranne quando si ricomincia la partita), il campo viene popolato dalle mine secondo le regole di 'randomizza_campo'
				if (!prima_mossa_effettuata)
				{
					partita.randomizza_campo(riga - 1, colonna - 1);
					prima_mossa_effettuata = true;
				}
				
				partita.gioca(riga - 1, colonna - 1, comando);

			}

			// NON entra nelle opzioni se il gioco è stato vinto o perso.

			if (partita._status() == 'S' || partita._status() == 'V')
			{
				break;
			}

			std::cout	<< "OPZIONI:\n"
						<< u8"• Torna al gioco.\t\t(1)\n"
						<< u8"• Ricomincia la partita.\t(2)\n"
						<< u8"• Rivela il campo.\t\t(3)\n"
						<< u8"• Torna al menù principale.\t(4)\n"
						<< u8"• Esci dal gioco.\t\t(5)" << std::endl;
			
			menu_opzioni(partita, uscita_programma, in_risolutore, in_gioco);

		}

		Risolutore risolutore(partita);

		while (!uscita_programma && in_risolutore)
		{
			std::cout << partita << std::endl;
			std::cout << u8"Inserisci le coordinate (nella forma 'riga colonna') per inizializzare il costruttore." << std::endl;
			inizializza_risolutore(partita);

			std::cout << partita << std::endl;
			
			risolutore.aggiorna(partita);
			risolutore.risolve();
			break;
		}

		if (partita._status() == 'S' || partita._status() == 'V')
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

		prima_mossa_effettuata = false;

	}
}


/*std::map<Coord, int> bordo_conteggio;

for (int i = 0; i < partita._altezza(); i++)
{
	for (int j = 0; j < partita._larghezza(); j++)
	{
		if (partita._campo_visibile()[i][j] > 0 && partita.conta_non_scavati_vicini(i, j) > 0)
		{
			if (partita._campo_visibile().indici_leciti(i - 1, j - 1) && partita._campo_visibile()[i - 1][j - 1] == -3)	bordo_conteggio[Coord(i - 1, j - 1)]++;
			if (partita._campo_visibile().indici_leciti(i - 1, j) && partita._campo_visibile()[i - 1][j] == -3)		bordo_conteggio[Coord(i - 1, j)]++;
			if (partita._campo_visibile().indici_leciti(i - 1, j + 1) && partita._campo_visibile()[i - 1][j + 1] == -3)	bordo_conteggio[Coord(i - 1, j + 1)]++;
			if (partita._campo_visibile().indici_leciti(i, j - 1) && partita._campo_visibile()[i][j - 1] == -3)		bordo_conteggio[Coord(i, j - 1)]++;
			if (partita._campo_visibile().indici_leciti(i, j + 1) && partita._campo_visibile()[i][j + 1] == -3)		bordo_conteggio[Coord(i, j + 1)]++;
			if (partita._campo_visibile().indici_leciti(i + 1, j - 1) && partita._campo_visibile()[i + 1][j - 1] == -3)	bordo_conteggio[Coord(i + 1, j - 1)]++;
			if (partita._campo_visibile().indici_leciti(i + 1, j) && partita._campo_visibile()[i + 1][j] == -3)		bordo_conteggio[Coord(i + 1, j)]++;
			if (partita._campo_visibile().indici_leciti(i + 1, j + 1) && partita._campo_visibile()[i + 1][j + 1] == -3)	bordo_conteggio[Coord(i + 1, j + 1)]++;
		}
	}
}

std::vector<std::pair<Coord, int> > bordo_conteggio_vettore(bordo_conteggio.begin(), bordo_conteggio.end());

std::sort(bordo_conteggio_vettore.begin(), bordo_conteggio_vettore.end(), compara);

for (int i = 0; i < bordo_conteggio_vettore.size(); i++)
{
	std::cout << "\"" << bordo_conteggio_vettore[i].first.first << ", " << bordo_conteggio_vettore[i].first.second << "\": " << bordo_conteggio_vettore[i].second << std::endl;
}
*/

// TO DO: recuperare QUESTA SOTTO per qualche versione

/*	std::pair<std::vector<Coord>, std::vector<Coord>> numeri_e_incognite = estrai_celle_incognite();

if (numeri_e_incognite.first.size() != 0)
{
	std::vector<Coord> coordinate_numeri_bordo = numeri_e_incognite.first;
	std::vector<Coord> incognite_bordo = numeri_e_incognite.second;

	std::map<Coord, std::pair<double, double>> incognite_probabilita_minmax;

	for (int n = 0; n < incognite_bordo.size(); n++)
	{
		int i = incognite_bordo[n].first;
		int j = incognite_bordo[n].second;

		partita._campo_visibile()[i][j] - partita.conta_bandiere_vicine(i, j);

		std::vector<double> probabilita(0);

		if (trova_elemento(coordinate_numeri_bordo, Coord(i - 1, j - 1)))	probabilita.push_back(double(partita._campo_visibile()[i - 1][j - 1] - partita.conta_bandiere_vicine(i - 1, j - 1)) / double(partita.conta_non_scavati_vicini(i - 1, j - 1)) );
		if (trova_elemento(coordinate_numeri_bordo, Coord(i - 1, j)))		probabilita.push_back(double(partita._campo_visibile()[i - 1][j] - partita.conta_bandiere_vicine(i - 1, j)) / double(partita.conta_non_scavati_vicini(i - 1, j)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i - 1, j + 1)))	probabilita.push_back(double(partita._campo_visibile()[i - 1][j + 1] - partita.conta_bandiere_vicine(i - 1, j + 1)) / double(partita.conta_non_scavati_vicini(i - 1, j + 1)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i, j - 1)))		probabilita.push_back(double(partita._campo_visibile()[i][j - 1] - partita.conta_bandiere_vicine(i, j - 1)) / double(partita.conta_non_scavati_vicini(i, j - 1)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i, j + 1)))		probabilita.push_back(double(partita._campo_visibile()[i][j + 1] - partita.conta_bandiere_vicine(i, j + 1)) / double(partita.conta_non_scavati_vicini(i, j + 1)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i + 1, j - 1)))	probabilita.push_back(double(partita._campo_visibile()[i + 1][j - 1] - partita.conta_bandiere_vicine(i + 1, j - 1)) / double(partita.conta_non_scavati_vicini(i + 1, j - 1)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i + 1, j)))		probabilita.push_back(double(partita._campo_visibile()[i + 1][j] - partita.conta_bandiere_vicine(i + 1, j)) / double(partita.conta_non_scavati_vicini(i + 1, j)));
		if (trova_elemento(coordinate_numeri_bordo, Coord(i + 1, j + 1)))	probabilita.push_back(double(partita._campo_visibile()[i + 1][j + 1] - partita.conta_bandiere_vicine(i + 1, j + 1)) / double(partita.conta_non_scavati_vicini(i + 1, j + 1)));

		double probabilita_min = *std::min_element(probabilita.cbegin(), probabilita.cend());
		double probabilita_max = *std::max_element(probabilita.cbegin(), probabilita.cend());

		for (int k = 0; k < probabilita.size(); k++) std::cout << probabilita[k] << " ";
		std::cout << std::endl;

		incognite_probabilita_minmax[incognite_bordo[n]] = std::pair <double, double>(probabilita_min, probabilita_max);

	}

	for (std::map<Coord, std::pair<double, double> >::const_iterator it = incognite_probabilita_minmax.cbegin(); it != incognite_probabilita_minmax.cend(); it++)
	{
		std::cout << "(" << it->first.first << ", " << it->first.second << "): prob.min = " << it->second.first << ", prob.max = " << it->second.second << std::endl;
	}

	std::pair<Coord, std::pair<double, double> > incognite_probabilita_minmax_sort = *std::min_element(incognite_probabilita_minmax.cbegin(), incognite_probabilita_minmax.cend(), compara);
	incognite_probabilita_minmax_sort.first;
}
else
{

}
*/

/*
	Campo copia_campo = partita;

	std::map<int, std::vector<std::vector<bool> > > raccolta_disposizioni;

	std::vector<std::vector<Coord> > incognite_separate = separa_bordo();

	for (int i = 0; i < incognite_separate.size(); i++)
	{
		metodo_probabilistico_ricorsivo(copia_campo, raccolta_disposizioni, incognite_separate[i], 0);
	}

	for (std::map<int, std::vector<std::vector<bool> > >::iterator it = raccolta_disposizioni.begin(); it != raccolta_disposizioni.end(); it++)
	{
		for (int j = 0; j < (*it).second.size(); j++)
		{
			for (int k = 0; k < (*it).second[j].size(); k++)
			{
				std::cout << (*it).second[j][k];
			}
			std::cout << std::endl;
		}
		std::cout << std::endl << std::endl;
	}
	*/

	// TO DO: eliminare
	// using recursion find all the mine configurations
	// that one section can possibly have
/*
void Risolutore::metodo_probabilistico_ricorsivo(Campo& copia_campo, std::map<int, std::vector<std::vector<bool> > >& raccolta_disposizioni,
	std::vector<Coord> bordo, int ramo)
{
	int totalMineNum = partita._mine();
	int flagCount = partita._numero_bandiere();
	int _col = partita._larghezza(), _row = partita._altezza();

	// total flagcount cannot exceed the total mineNum
	if (flagCount > totalMineNum) return;

	// if the flagcount is bigger than the neighborcount already?
	// then, return before it goes through more recursion.
	// for more optimization, the open tiles for the section can be
	// put into a container to be looped

	if (ramo != bordo.size()) {
		for (int i = 0; i < bordo.size(); i++)//auto& anyTile : section
		{
			int tileCol = bordo[i].second, tileRow = bordo[i].second;
			for (int yoff = -1; yoff <= 1; ++yoff)
			{
				for (int xoff = -1; xoff <= 1; ++xoff)
				{
					int c = tileCol + xoff;
					int r = tileRow + yoff;
					// within in the range
					if (c > -1 && c < _col &&
						r > -1 && r < _row)
					{
						// should be revealed
						if (!copia_campo._campo_visibile().is_elemento(r, c, -3))
						{
							// if the flagcount is bigger than the neighborcount already?
							// then, return before it goes through more recursion.
							if (copia_campo.conta_bandiere_vicine(c, r) > copia_campo._campo_visibile().conta_caselle_vicine(c, r))
								return;
						}
					}
				}
			}
		}
		int qCol = bordo[ramo].second;
		int qRow = bordo[ramo].first;
		copia_campo.gioca(qCol, qRow, 'B');		// guess set
		metodo_probabilistico_ricorsivo(copia_campo, raccolta_disposizioni, bordo, ramo + 1);
		copia_campo.gioca(qCol, qRow, 'B');		// guess unset
		metodo_probabilistico_ricorsivo(copia_campo, raccolta_disposizioni, bordo, ramo + 1);
	}
	// DFS Search
	// go to depth k and check it is correct
	else if (ramo == bordo.size())
	{
		for (int i = 0; i < bordo.size(); i++)//auto& anyTile : section
		{
			int tCol = bordo[i].second, tRow = bordo[i].second;
			for (int yoff = -1; yoff <= 1; ++yoff)
			{
				for (int xoff = -1; xoff <= 1; ++xoff)
				{
					int c = tCol + xoff, r = tRow + yoff;
					// within in the range
					if (c > -1 && c < _col &&
						r > -1 && r < _row)
					{
						// should be revealed
						if (!copia_campo._campo_visibile().is_elemento(r, c, -3))
						{
							// if the counting is not the same?
							// then it is not the possible answer.
							if (copia_campo.conta_bandiere_vicine(c, r) != copia_campo._campo_visibile().conta_caselle_vicine(c, r))
								return;
						}
					}
				}
			}
		}
		// when it is the end of the game and there isn't the same number of the flags
		// that totals to be the totalMineNum, then it is not a solution.
		if (copia_campo._numero_bandiere() != totalMineNum) // borderOptimization&&
			return;
		std::vector<bool> oneSectionSolution;	// store a one configuration
		int mineCountForSection = 0;
		for (int i = 0; i < bordo.size(); i++)
		{
			int tCol = bordo[i].second, tRow = bordo[i].first;
			bool flagged = copia_campo._campo_visibile().is_elemento(tRow, tCol, -3);
			oneSectionSolution.push_back(flagged);
			if (flagged) ++mineCountForSection;
		}
		std::map<int, std::vector<std::vector<bool> > >::iterator mineCountFound = raccolta_disposizioni.find(mineCountForSection);
		// if there isnt a vector for the mine count
		if (mineCountFound == raccolta_disposizioni.end())
		{
			std::vector<std::vector<bool> > vectorSectionSolution;
			vectorSectionSolution.push_back(oneSectionSolution);
			raccolta_disposizioni.insert(make_pair(mineCountForSection, vectorSectionSolution));
		}
		// if there is
		else
		{
			mineCountFound->second.push_back(oneSectionSolution);
		}
		return;
	}
}
*/


/*
void Risolutore::metodo_probabilistico_ricorsivo(std::vector<Matrice<bool> > disposizioni, const std::vector<Coord>& bordo, int k)
{
		// Return if at this point, it's already inconsistent
	for (int i = 0; i < partita._altezza(); i++)
	{
		for (int j = 0; j < partita._larghezza(); j++)
		{

			std::cout << "Ciao: " << i + 1 << ", " << j + 1 << std::endl;
			// Count flags for endgame cases
			int currentBlockValue = partita._campo_visibile()[i][j];
			if (currentBlockValue < 0) continue;

			// Scenario 1: too many mines
			if (partita.conta_bandiere_vicine(i, j) > currentBlockValue) return;

			// Total bordering blocks	// TO DO: sostituire con una funzione di quelle mie
			int countBorderingBlocks;
			if ((i == 0 && j == 0) || (i == partita._larghezza() - 1 && i == partita._altezza() - 1)) countBorderingBlocks = 3;
			else if (i == 0 || j == 0 || j == partita._larghezza() - 1 || j == partita._altezza() - 1) countBorderingBlocks = 5;
			else countBorderingBlocks = 8;

			// Scenario 2: too many empty
			if (countBorderingBlocks - partita.conta_bandiere_vicine(i, j) < currentBlockValue) return;
		}
	}

		std::cout << "Sono qua:" << std::endl;


		// We have too many flags
		if (partita._numero_bandiere() > partita._mine()) return;

		// Solution found!
		if (k == bordo.size()) {

			// We don't have the exact mine count, so no
			if (partita._numero_bandiere() < partita._mine()) return;

			std::vector<bool> solution (bordo.size());
			Matrice<bool> disposizione(partita._altezza(), partita._larghezza());
			for (int i = 0; i < bordo.size(); i++) {
				Coord block = bordo[i];
				partita.gioca(block.first, block.second, 'B');
				//disposizione[block.first][block.second] = true;
			}
			disposizioni.push_back(disposizione);
			return;
		}

		Coord block = bordo[k];

		// Recurse two positions: mine and no mine
		partita.gioca(block.first, block.second, 'B');
		metodo_probabilistico_ricorsivo(disposizioni, bordo, k + 1);
		partita.gioca(block.first, block.second, 'S');

		partita.gioca(block.first, block.second, 'S');
		metodo_probabilistico_ricorsivo(disposizioni, bordo, k + 1);
		partita.gioca(block.first, block.second, 'B');
}
*/
