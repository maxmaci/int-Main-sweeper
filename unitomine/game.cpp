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
	Campo partita;						// ...
	int bandiere_precedenti;			// 0 <= bandiere_precedenti < partita._altezza() * partita._larghezza()		// TO DO: controllare per l'uguale
	int celle_non_scavate_precedenti;	// 0 <= bandiere_precedenti <= partita._altezza() * partita._larghezza()
	
	/* METODI PER RICAVARE INFORMAZIONI SUL CAMPO */
	std::vector<Coord> estrai_numeri_bordo();
	std::vector<Coord> estrai_celle_incognite();

	std::vector<std::vector<Coord> > separa_numeri();

	std::vector< std::vector<Coord> > separa_bordo2();
	bool disposizione_lecita(const std::vector<Coord>&, const std::vector<Coord>&, std::vector<bool>&);

	/* METODI RISOLUTIVI */
	void metodo_meccanico();
	void metodo_gaussiano();
	void metodo_probabilistico();

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

	//std::pair<std::vector<Coord>, std::vector<Coord>> numeri_e_incognite = estrai_numeri_bordo_celle_incognite();

	
	std::vector< std::vector<Coord>> coordinate_numeri_bordo_per_sezione = separa_numeri();
	std::vector< std::vector<Coord>> incognite_bordo_per_sezione = separa_bordo2();

	for (int a = 0; a < coordinate_numeri_bordo_per_sezione.size(); a++)
	{
		std::vector<Coord> coordinate_numeri_bordo = coordinate_numeri_bordo_per_sezione[a];
		std::vector<Coord> incognite_bordo = incognite_bordo_per_sezione[a];

		std::vector<int> numeri_bordo;

		Matrice<int> matrice(static_cast<int>(coordinate_numeri_bordo.size()), static_cast<int>(incognite_bordo.size()));

		for (int n = 0; n < matrice._righe(); n++)
		{
			int i = coordinate_numeri_bordo[n].first;
			int j = coordinate_numeri_bordo[n].second;

			numeri_bordo.push_back(partita._campo_visibile()[i][j] - partita.conta_bandiere_vicine(i, j));

			if (trova_elemento(incognite_bordo, Coord(i - 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i - 1, j - 1))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i - 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i - 1, j))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i - 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i - 1, j + 1))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i, j - 1)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i, j - 1))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i, j + 1)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i, j + 1))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i + 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j - 1))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i + 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j))] = 1;
			if (trova_elemento(incognite_bordo, Coord(i + 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord(i + 1, j + 1))] = 1;
		}

		// FASE 2: riduzione gaussiana della matrice.

		std::pair<Matrice<int>, std::vector<int>> matrice_completa = matrice.riduzione_gaussiana_con_termine_noto(numeri_bordo);

		// FASE 3: controllo delle soluzioni.

		//Consideriamo solamente le righe non vuote (il sistema originale non sempre ha soluzioni - anzi, quasi mai!)

		Matrice<int> matrice_ridotta(0, matrice_completa.first._colonne());
		std::vector<int> termine_noto;
		//std::vector<Coord> coordinate_numeri_bordo_ridotta;
		for (int i = 0; i < matrice_completa.first._righe(); i++)
		{
			for (int j = 0; j < matrice_completa.first._colonne(); j++)
			{
				if (matrice[i][j] != 0)
				{
					matrice_ridotta.push_back(matrice_completa.first[i]);
					termine_noto.push_back(matrice_completa.second[i]);
					//coordinate_numeri_bordo_ridotta.push_back(coordinate_numeri_bordo[i]);
					break;
				}
			}
		}

		std::cout << matrice_ridotta << std::endl;

		for (int i = 0; i < termine_noto.size(); i++)
		{
			std::cout << termine_noto[i];
		}
		std::cout << std::endl;
		system("PAUSE");

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

	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

/*
void Risolutore::metodo_gaussiano()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1: creazione della matrice di incognite
	
	//std::pair<std::vector<Coord>, std::vector<Coord>> numeri_e_incognite = estrai_numeri_bordo_celle_incognite();

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
		if (trova_elemento(incognite_bordo, Coord (i, j + 1)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord (i, j + 1))]			= 1;
		if (trova_elemento(incognite_bordo, Coord (i + 1, j - 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord (i + 1, j - 1))]		= 1;
		if (trova_elemento(incognite_bordo, Coord (i + 1, j)))		matrice[n][trova_indice_elemento(incognite_bordo, Coord (i + 1, j))]			= 1;
		if (trova_elemento(incognite_bordo, Coord (i + 1, j + 1)))	matrice[n][trova_indice_elemento(incognite_bordo, Coord (i + 1, j + 1))]		= 1;
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
	
	std::cout << matrice_ridotta << std::endl;

	for (int i = 0; i < termine_noto.size(); i++)
	{
		std::cout << termine_noto[i];
	}
	std::cout << std::endl;
	system("PAUSE");

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
*/

/* METODO PROBABILISTICO */

// TO DO: mettere l'eccezione posta nel paint allegato : separare il bordo se accanto alla cella successiva ci sono solo bandiere

bool ordina_per_dimensione(std::vector<Coord> p, std::vector<Coord> q)
{
	return p.size() < q.size();
}

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

				coda.push(Coord(cella.first - 1, cella.second));
				coda.push(Coord(cella.first, cella.second - 1));
				coda.push(Coord(cella.first, cella.second + 1));
				coda.push(Coord(cella.first + 1, cella.second));

			}
			if (partita._campo_visibile().indici_leciti(cella.first, cella.second) && partita._campo_visibile()[cella.first][cella.second] > 0)
			{
				if (partita._campo_visibile().indici_leciti(cella.first - 1, cella.second - 1)	&& partita._campo_visibile()[cella.first - 1][cella.second - 1] == -3) coda.push(Coord(cella.first - 1, cella.second - 1));
				if (partita._campo_visibile().indici_leciti(cella.first - 1, cella.second)		&& partita._campo_visibile()[cella.first - 1][cella.second] == -3) coda.push(Coord(cella.first - 1, cella.second));
				if (partita._campo_visibile().indici_leciti(cella.first - 1, cella.second + 1)	&& partita._campo_visibile()[cella.first - 1][cella.second + 1] == -3) coda.push(Coord(cella.first - 1, cella.second + 1));
				if (partita._campo_visibile().indici_leciti(cella.first, cella.second - 1)		&& partita._campo_visibile()[cella.first][cella.second - 1] == -3) coda.push(Coord(cella.first, cella.second - 1));
				if (partita._campo_visibile().indici_leciti(cella.first, cella.second + 1)		&& partita._campo_visibile()[cella.first][cella.second + 1] == -3) coda.push(Coord(cella.first, cella.second + 1));
				if (partita._campo_visibile().indici_leciti(cella.first + 1, cella.second - 1)	&& partita._campo_visibile()[cella.first + 1][cella.second - 1] == -3) coda.push(Coord(cella.first + 1, cella.second - 1));
				if (partita._campo_visibile().indici_leciti(cella.first + 1, cella.second)		&& partita._campo_visibile()[cella.first + 1][cella.second] == -3) coda.push(Coord(cella.first + 1, cella.second));
				if (partita._campo_visibile().indici_leciti(cella.first + 1, cella.second + 1)	&& partita._campo_visibile()[cella.first + 1][cella.second + 1] == -3) coda.push(Coord(cella.first + 1, cella.second + 1));
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

				coda.push(Coord(cella.first - 1, cella.second));
				coda.push(Coord(cella.first, cella.second - 1));
				coda.push(Coord(cella.first, cella.second + 1));
				coda.push(Coord(cella.first + 1, cella.second));

			}
		}
		if (incognite_separate_regione.size() != 0)
		{
			//for (int i = 0; i < incognite_separate_regione.size(); i++)
			//{
			//	std::cout << "(" << incognite_separate_regione[i].first + 1 << ", " << incognite_separate_regione[i].second + 1 << ") ";
			//}

			incognite_separate.push_back(incognite_separate_regione);

			std::cout << std::endl;
		}	
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return incognite_separate;
}
*/
/*
std::map<std::vector<Coord>, std::vector<Coord>> Risolutore::separa_bordo()
{
	std::vector<Coord> incognite = estrai_celle_incognite();

	std::vector<Coord> numeri = estrai_numeri_bordo();

	std::map<std::vector<Coord>, std::vector<Coord>> bordo; // INCOGNITE SEPARATE, NUMERI ACCANTO

	for (int i = 0; i < incognite.size(); i++)
	{
		std::vector<Coord> numeri_accanto;

		if (trova_elemento(numeri, Coord(incognite[i].first - 1, incognite[i].second - 1)))	numeri_accanto.push_back(Coord(incognite[i].first - 1, incognite[i].second - 1));
		if (trova_elemento(numeri, Coord(incognite[i].first - 1, incognite[i].second)))		numeri_accanto.push_back(Coord(incognite[i].first - 1, incognite[i].second));
		if (trova_elemento(numeri, Coord(incognite[i].first - 1, incognite[i].second + 1)))	numeri_accanto.push_back(Coord(incognite[i].first - 1, incognite[i].second + 1));
		if (trova_elemento(numeri, Coord(incognite[i].first, incognite[i].second - 1)))		numeri_accanto.push_back(Coord(incognite[i].first, incognite[i].second - 1));
		if (trova_elemento(numeri, Coord(incognite[i].first, incognite[i].second + 1)))		numeri_accanto.push_back(Coord(incognite[i].first, incognite[i].second + 1));
		if (trova_elemento(numeri, Coord(incognite[i].first + 1, incognite[i].second - 1)))	numeri_accanto.push_back(Coord(incognite[i].first + 1, incognite[i].second - 1));
		if (trova_elemento(numeri, Coord(incognite[i].first + 1, incognite[i].second)))		numeri_accanto.push_back(Coord(incognite[i].first + 1, incognite[i].second));
		if (trova_elemento(numeri, Coord(incognite[i].first + 1, incognite[i].second + 1)))	numeri_accanto.push_back(Coord(incognite[i].first + 1, incognite[i].second + 1));
	
		if (numeri_accanto.size() == 0) continue;	// magari non è necessario

		bordo[numeri_accanto].push_back(incognite[i]);
	}
	return bordo;
}
*/
std::vector<std::vector<Coord> > Risolutore::separa_numeri()
{
	std::vector<Coord> numeri = estrai_numeri_bordo();

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

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

		std::vector<Coord> numeri_accanto_vettore (numeri_accanto.begin(), numeri_accanto.end());

		numeri_separati.push_back(numeri_accanto_vettore);

	}

	return numeri_separati;
}
/*
std::map<Coord, std::vector<Coord>> Risolutore::incognite_per_numero()
{
	std::vector<Coord> numeri = estrai_numeri_bordo();
	
	std::map<std::vector<Coord>, std::vector<Coord>> bordo_separato = separa_bordo();

	std::map<Coord, std::vector<Coord>> incognite_per_numero;

	for (int i = 0; i < numeri.size(); i++)
	{
		std::set<Coord> sezione;

		for (std::map<std::vector<Coord>, std::vector<Coord> >::iterator it = bordo_separato.begin(); it != bordo_separato.end(); it++)
		{
			if (trova_elemento((*it).first, numeri[i]))
			{
				for (int j = 0; j < (*it).second.size(); j++)
				{
					sezione.insert((*it).second[j]);
				}
				 // inserisce le coordinate nel vettore sezione in fondo
			}
		}

		std::vector<Coord> sezione_vettore(sezione.begin(), sezione.end());

		incognite_per_numero[numeri[i]] = sezione_vettore;
	}

	return incognite_per_numero;
}
*/
/*
std::vector<std::map<Coord, std::vector<Coord>>> Risolutore::incognite_per_numero_per_bordo()
{
	std::map<Coord, std::vector<Coord>> incognite_per_numeri = incognite_per_numero();
	
	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

	std::vector<std::map<Coord, std::vector<Coord>>> incognite_per_numero_per_bordo (bordo_separato.size());

	for (std::map<Coord, std::vector<Coord>>::iterator it = incognite_per_numeri.begin(); it != incognite_per_numeri.end(); it++)
	{
		for (int i = 0; i < bordo_separato.size(); i++)
		{
			for (int j = 0; j < (*it).second.size(); j++)
			{
				if (trova_elemento(bordo_separato[i], (*it).second[j])) (incognite_per_numero_per_bordo[i])[(*it).first] = (*it).second;
			}
		}
		
	}

	return incognite_per_numero_per_bordo;
}
*/
/*
std::vector<std::vector<std::map<Coord, bool>>> Risolutore::trova_soluzioni(const std::vector<std::vector<std::map<Coord, bool>>>& possibilita_per_numero)
{
	std::vector<std::vector<std::map<Coord, bool>>> res = { {} };
	for (const auto& u : possibilita_per_numero)
	{
		std::vector<std::vector<std::map<Coord, bool>>> temp;
		for (const auto& x : res)
		{
			for (const auto y : u)
			{
				temp.push_back(x);
				temp.back().push_back(y);
			}
		}
		res = std::move(temp);
	}
	return res;
}
*/
/*
bool compara(std::pair<Coord, std::pair<double, double>> p, std::pair<Coord, std::pair<double, double>> q)
{
	return p.second.first < q.second.first;
}
*/
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

typedef std::pair<Coord, bool> Boolcoord;

void Risolutore::metodo_probabilistico()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1 : Preparazione. Prendiamo il bordo di celle incognite (quelle accanto a dei numeri e il cui numero di bandiere non è sufficiente) e segmentiamo il  le celle in modo che siano 

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

	std::vector<std::vector<Coord> > numeri_separati = separa_numeri();

	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			//std::cout << "(" << numeri_separati[i][j].first + 1 << ", " << numeri_separati[i][j].second + 1 << ") ";
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		std::cout << std::endl;
		mine_max_separate.push_back(mine_max);
	}

	std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::map<int, std::vector<std::vector<Boolcoord>>> disposizioni_per_sezione;

		std::vector<bool> disposizione(bordo_separato[a].size());

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(bordo_separato[a].size()));
		estremi.push_back(partita._mine() - partita._numero_bandiere());

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << estremi[0] << std::endl;
		std::cout << "singola_disposizione.size(): " << estremi[1] << std::endl;
		std::cout << "mine_rimanenti: " << estremi[2] << std::endl;

		for (int i = 0; i < estremo; i++)
		{
			disposizione[i] = true;
			std::sort(disposizione.begin(), disposizione.end());
			do
			{
				if (disposizione_lecita(bordo_separato[a], numeri_separati[a], disposizione))
				{
					std::vector<Boolcoord> disposizione_con_coordinate;
					for (int j = 0; j < disposizione.size(); j++)
					{
						disposizione_con_coordinate.push_back(std::make_pair(bordo_separato[a][j], disposizione[j]));
					}
					disposizioni_per_sezione[somma_elementi(disposizione)].push_back(disposizione_con_coordinate);
				}
			} while (std::next_permutation(disposizione.begin(), disposizione.end()));
		}
		//std::vector<std::vector<Boolcoord>> disposizioni_per_sezione_vettore(disposizioni_per_sezione.begin(), disposizioni_per_sezione.end());

		possibilita_per_sezione.push_back(disposizioni_per_sezione);
	}

	std::cout << u8"N° SEZIONI: " << possibilita_per_sezione.size() << std::endl;		// std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>>

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{		
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;

		for (std::map<int, std::vector<std::vector<Boolcoord>>>::iterator it = possibilita_per_sezione[i].begin(); it != possibilita_per_sezione[i].end(); it++)
		{
			std::cout << u8"N° MINE: " << (*it).first << std::endl;
			for (int j = 0; j < (*it).second.size(); j++)
			{
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << "(" << (*it).second[j][k].first.first + 1 << "," << (*it).second[j][k].first.second + 1 << ")";
				}
				std::cout << std::endl;
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << (*it).second[j][k].second;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

/*
void Risolutore::metodo_probabilistico()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1 : Preparazione. Prendiamo il bordo di celle incognite (quelle accanto a dei numeri e il cui numero di bandiere non è sufficiente) e segmentiamo il  le celle in modo che siano

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

	std::vector<std::vector<Coord> > numeri_separati = separa_numeri();

	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			//std::cout << "(" << numeri_separati[i][j].first + 1 << ", " << numeri_separati[i][j].second + 1 << ") ";
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		std::cout << std::endl;
		mine_max_separate.push_back(mine_max);
	}

	std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::map<int, std::vector<std::vector<Boolcoord>>> disposizioni_per_sezione;

		std::vector<bool> disposizione(bordo_separato[a].size());

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(bordo_separato[a].size()));
		estremi.push_back(partita._mine() - partita._numero_bandiere());

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << estremi[0] << std::endl;
		std::cout << "singola_disposizione.size(): " << estremi[1] << std::endl;
		std::cout << "mine_rimanenti: " << estremi[2] << std::endl;

		for (int i = 0; i < estremo; i++)
		{
			disposizione[i] = true;
			std::sort(disposizione.begin(), disposizione.end());
			do
			{
				if (disposizione_lecita(bordo_separato[a], numeri_separati[a], disposizione))
				{
					std::vector<Boolcoord> disposizione_con_coordinate;
					for (int j = 0; j < disposizione.size(); j++)
					{
						disposizione_con_coordinate.push_back(std::make_pair(bordo_separato[a][j], disposizione[j]));
					}
					disposizioni_per_sezione[somma_elementi(disposizione)].push_back(disposizione_con_coordinate);
				}
			} while (std::next_permutation(disposizione.begin(), disposizione.end()));
		}
		//std::vector<std::vector<Boolcoord>> disposizioni_per_sezione_vettore(disposizioni_per_sezione.begin(), disposizioni_per_sezione.end());

		possibilita_per_sezione.push_back(disposizioni_per_sezione);
	}

	std::cout << u8"N° SEZIONI: " << possibilita_per_sezione.size() << std::endl;		// std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>>

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;

		for (std::map<int, std::vector<std::vector<Boolcoord>>>::iterator it = possibilita_per_sezione[i].begin(); it != possibilita_per_sezione[i].end(); it++)
		{
			std::cout << u8"N° MINE: " << (*it).first << std::endl;
			for (int j = 0; j < (*it).second.size(); j++)
			{
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << "(" << (*it).second[j][k].first.first + 1 << "," << (*it).second[j][k].first.second + 1 << ")";
				}
				std::cout << std::endl;
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << (*it).second[j][k].second;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}

void Risolutore::metodo_probabilistico()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1 : Preparazione. Prendiamo il bordo di celle incognite (quelle accanto a dei numeri e il cui numero di bandiere non è sufficiente) e segmentiamo il  le celle in modo che siano 

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

	std::vector<std::vector<Coord> > numeri_separati = separa_numeri();

	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			//std::cout << "(" << numeri_separati[i][j].first + 1 << ", " << numeri_separati[i][j].second + 1 << ") ";
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		std::cout << std::endl;
		mine_max_separate.push_back(mine_max);
	}

	std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::map<int, std::vector<std::vector<Boolcoord>>> disposizioni_per_sezione;

		std::vector<bool> disposizione(bordo_separato[a].size());

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(bordo_separato[a].size()));
		estremi.push_back(partita._mine() - partita._numero_bandiere());

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << estremi[0] << std::endl;
		std::cout << "singola_disposizione.size(): " << estremi[1] << std::endl;
		std::cout << "mine_rimanenti: " << estremi[2] << std::endl;

		for (int i = 0; i < estremo; i++)
		{
			disposizione[i] = true;
			std::sort(disposizione.begin(), disposizione.end());
			do
			{
				if (disposizione_lecita(bordo_separato[a], numeri_separati[a], disposizione))
				{
					std::vector<Boolcoord> disposizione_con_coordinate;
					for (int j = 0; j < disposizione.size(); j++)
					{
						disposizione_con_coordinate.push_back(std::make_pair(bordo_separato[a][j], disposizione[j]));
					}
					disposizioni_per_sezione[somma_elementi(disposizione)].push_back(disposizione_con_coordinate);
				}
			} while (std::next_permutation(disposizione.begin(), disposizione.end()));
		}
		//std::vector<std::vector<Boolcoord>> disposizioni_per_sezione_vettore(disposizioni_per_sezione.begin(), disposizioni_per_sezione.end());

		possibilita_per_sezione.push_back(disposizioni_per_sezione);
	}

	std::cout << u8"N° SEZIONI: " << possibilita_per_sezione.size() << std::endl;		// std::vector<std::map<int, std::vector<std::vector<Boolcoord>>>>

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{		
		std::cout << u8"SEZIONE N°: " << i + 1 << std::endl;

		for (std::map<int, std::vector<std::vector<Boolcoord>>>::iterator it = possibilita_per_sezione[i].begin(); it != possibilita_per_sezione[i].end(); it++)
		{
			std::cout << u8"N° MINE: " << (*it).first << std::endl;
			for (int j = 0; j < (*it).second.size(); j++)
			{
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << "(" << (*it).second[j][k].first.first + 1 << "," << (*it).second[j][k].first.second + 1 << ")";
				}
				std::cout << std::endl;
				for (int k = 0; k < (*it).second[j].size(); k++)
				{
					//std::cout << possibilita_per_sezione[i][j][k].second;
					std::cout << (*it).second[j][k].second;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}
void Risolutore::metodo_probabilistico()
{
	auto start = std::chrono::steady_clock::now();

	// FASE 1 : Preparazione. Prendiamo il bordo di celle incognite (quelle accanto a dei numeri e il cui numero di bandiere non è sufficiente) e segmentiamo il  le celle in modo che siano 

	std::vector<std::vector<Coord> > bordo_separato = separa_bordo2();

	std::vector<std::vector<Coord> > numeri_separati = separa_numeri();
	
	std::vector<int> mine_max_separate;

	for (int i = 0; i < numeri_separati.size(); i++)
	{
		int mine_max = 0;
		for (int j = 0; j < numeri_separati[i].size(); j++)
		{
			std::cout << "(" << numeri_separati[i][j].first + 1 << ", " << numeri_separati[i][j].second + 1 << ") ";
			mine_max += partita._campo_visibile()[numeri_separati[i][j].first][numeri_separati[i][j].second] - partita.conta_bandiere_vicine(numeri_separati[i][j].first, numeri_separati[i][j].second);
		}
		std::cout << std::endl;
		mine_max_separate.push_back(mine_max);
	}

	std::vector<std::vector<std::map<Coord, bool>>> possibilita_per_sezione;

	for (int a = 0; a < bordo_separato.size(); a++)
	{
		std::set<std::map<Coord, bool>> disposizioni_per_sezione;

		std::vector<bool> disposizione(bordo_separato[a].size());

		std::vector<int> estremi;
		estremi.push_back(mine_max_separate[a]);
		estremi.push_back(static_cast<int>(bordo_separato[a].size()));
		estremi.push_back(partita._mine() - partita._numero_bandiere());

		int estremo = *std::min_element(estremi.begin(), estremi.end());

		std::cout << "estremo: " << estremo << std::endl;
		std::cout << "mine_max_separate: " << estremi[0] << std::endl;
		std::cout << "singola_disposizione.size(): " << estremi[1] << std::endl;
		std::cout << "mine_rimanenti: " << estremi[2] << std::endl;

		for (int i = 0; i < estremo; i++)
		{
			disposizione[i] = true;
			std::sort(disposizione.begin(), disposizione.end());
			do
			{
				if (disposizione_lecita(bordo_separato[a], numeri_separati[a], disposizione))
				{
					std::map<Coord, bool> disposizione_singolo_numero;
					for (int j = 0; j < disposizione.size(); j++)
					{
						//std::cout << disposizione[j];
						disposizione_singolo_numero[bordo_separato[a][j]] = disposizione[j];
					}
					disposizioni_per_sezione.insert(disposizione_singolo_numero);
					//std::cout << std::endl;
				}
			} while (std::next_permutation(disposizione.begin(), disposizione.end()));
		}
		std::vector<std::map<Coord, bool>> disposizioni_per_sezione_vettore(disposizioni_per_sezione.begin(), disposizioni_per_sezione.end());

		possibilita_per_sezione.push_back(disposizioni_per_sezione_vettore);
	}
		
	std::cout << "N° SEZIONI: " << possibilita_per_sezione.size() << std::endl;

	for (int i = 0; i < possibilita_per_sezione.size(); i++)
	{
		std::cout << "N° DISPOSIZONI: " << possibilita_per_sezione[i].size() << std::endl;

		for (int j = 0; j < possibilita_per_sezione[i].size(); j++)
		{
			for (std::map<Coord, bool>::iterator it = possibilita_per_sezione[i][j].begin(); it != possibilita_per_sezione[i][j].end(); it++)
			{
				std::cout << (*it).second;
				//std::cout << "(" << (*it).first.first + 1 << ", " << (*it).first.second + 1 << "): " << (*it).second << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	auto end = std::chrono::steady_clock::now();

	auto diff = end - start;

	std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

}
*/

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
