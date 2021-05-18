#ifndef __CAMPO_H__
#define __CAMPO_H__

#include <utility>		// classe pair
#include <string>		// classe string
#include <map>			// classe map
#include <queue>		// classe queue

#include "matrice.h"
#include "input.h"

// TO DO: sostituire campo "numero bandiere" con il conta elementi per le bandiere

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

typedef std::pair<int, int> Coord;

class Campo
{
private:
	int altezza;						// 0 < altezza < 50
	int larghezza;						// 0 < larghezza < 50
	int mine;							// 0 < mine < altezza * larghezza

	int numero_bandiere;				// 0 <= numero_bandiere < altezza * larghezza

	char status;						// '-': nè persa, nè vinta; 'S': sconfitta; 'V': vittoria. 

	Matrice<bool> campo_nascosto;
	Matrice<int> campo_visibile;

	/* METODI DI GIOCO */
	void scava_celle(int, int, Matrice<bool>&);				// metodo che 'scava' le celle (utilizzando l'algoritmo di Fill usato ad es. in Microsoft Paint)
	void aggiorna_cella(int, int);

	/* METODI DI LETTURA DI GIOCO */
	Matrice<bool> _campo_nascosto() const { return campo_nascosto; };			// to do: non lo uso mai ed è prono ad essere abusato per gli scopi malvagi di risolutore: ci converrà metterlo in privato
	int conta_mine_vicine(int, int) const;

	/* SCONFITTA / VITTORIA */
	void sconfitta(int, int);								// visualizzando il campo con le mine, imposta lo status a S(confitta)
	void vittoria();										// controlla se le mine sono state tutte segnate con la bandierina. Se è così, imposta lo status a V(ittoria)

public:
	/* COSTRUTTORE */
	Campo(int = 9, int = 9, int = 10);
	Campo(Matrice<bool>);

	/* LEGGI CAMPI PRIVATE */
	int _altezza() const { return altezza; };
	int _larghezza() const { return larghezza; };
	int _mine() const { return mine; };

	int _numero_bandiere() const { return numero_bandiere; };
	
	char _status() const { return status; };
	
	Matrice<int> _campo_visibile() const { return campo_visibile; };

	/* FUNZIONI DI GENERAZIONE DEL CAMPO */
	void randomizza_campo(int, int);						// randomizza il campo con le mine 

	/* FUNZIONI DI RESET */
	void reset_gioco() { campo_nascosto.reset(); };			// pulisce il campo da gioco dalle mine
	void reset_giocatore() { campo_visibile.reset(-3); };	// pulisce il campo dell giocatore allo stato originale
	void reset_status() { status = '-'; };					// resetta lo status
	void reset_numero_bandiere() { numero_bandiere = 0; };	// resetta il numero di bandiere segnate
	void reset();											// pulisce il campo da gioco e del giocatore, resetta lo status a '-'

	/* METODI DI GIOCO */
	void resize(int, int, int);								// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine
	void gioca(int, int, char);								// compie le azioni di gioco
	void rivela();											// rivela il campo da gioco al giocatore nelle opzioni

	/* FUNZIONI DI LETTURA STATUS */
	int conta_non_scavati_vicini(int, int) const;
	int conta_bandiere_vicine(int, int) const;
	bool conta_se_numeri_vicini(int, int) const;
	int conta_numeri_vicini(int i, int j) const;
	bool bordo_non_scavato(int, int) const;

	/* FUNZIONI DI STAMPA */
	friend std::ostream& operator<<(std::ostream&, const Campo&);
};

Campo::Campo(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_altezza > 50 ||  input_larghezza < 1 || input_larghezza > 50) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	
	campo_nascosto = Matrice<bool>(input_altezza, input_larghezza);
	campo_visibile = Matrice<int>(input_altezza, input_larghezza, -3);

	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	numero_bandiere = 0;
	status = '-';
}

Campo::Campo(Matrice<bool> campo_input)
{
	if (campo_input._righe() < 1 || campo_input._righe() > 50 || campo_input._colonne() < 1 || campo_input._colonne() > 50) throw std::domain_error("dimensioni del campo invalide");
	if (campo_input.conta_tutti_elemento(true) < 1 || campo_input.conta_tutti_elemento(true) >= campo_input._righe() * campo_input._colonne()) throw std::domain_error("numero delle mine illegale");

	campo_nascosto = campo_input;
	campo_visibile = Matrice<int>(campo_input._righe(), campo_input._colonne(), -3);

	altezza = campo_input._righe();
	larghezza = campo_input._colonne();
	mine = campo_input.conta_tutti_elemento(true);
	numero_bandiere = 0;
	status = '-';
}

std::ostream& operator<<(std::ostream& os, const Campo& campo)
{
	if (campo.altezza > 9)
	{
		os << std::setw(3) << std::setfill(' ');
		for (int j = 0; j < campo.larghezza + 1; j++)
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
	for (int j = 0; j < campo.larghezza; j++)
	{
		os << (j + 1) % 10;
	}
	os << '\n';
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"┌";
	for (int j = 0; j < campo.larghezza; j++)
	{
		os << u8"─";
	}
	os << u8"┐\n";
	for (int i = 0; i < campo._altezza(); i++)
	{
		os << std::right << std::setw(2) << std::setfill(' ') << i + 1 << u8"│\x1B[48;2;192;192;192m";
		for (int j = 0; j < campo._larghezza(); j++)
		{
			os << mappa_conversione[campo.campo_visibile[i][j]];
		}
		os << u8"\x1B[0m│" << std::left << std::setw(2) << std::setfill(' ') << i + 1 << '\n';
	}
	os << std::right << std::setw(2) << std::setfill(' ') << " " << u8"└";
	for (int j = 0; j < campo.larghezza; j++)
	{
		os << u8"─";
	}
	os << u8"┘\n";
	if (campo.larghezza > 9)
	{
		os << std::setw(3) << std::setfill(' ');
		for (int j = 0; j < campo.larghezza + 1; j++)
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
	for (int j = 0; j < campo.larghezza; j++)
	{
		os << (j + 1) % 10;
	}
	os << '\n';

	return os;
}

int Campo::conta_non_scavati_vicini(int i, int j) const
{
	return campo_visibile.conta_vicini(i, j, -3);
}

int Campo::conta_bandiere_vicine(int i, int j) const
{
	return campo_visibile.conta_vicini(i, j, -2);
}

int Campo::conta_mine_vicine(int i, int j) const
{
	return campo_nascosto.conta_vicini(i, j, true);
}

int Campo::conta_numeri_vicini(int i, int j) const
{
	return campo_visibile.conta_vicini(i, j, 1) + campo_visibile.conta_vicini(i, j, 2) + campo_visibile.conta_vicini(i, j, 3) + campo_visibile.conta_vicini(i, j, 4) + campo_visibile.conta_vicini(i, j, 5) + campo_visibile.conta_vicini(i, j, 6) + campo_visibile.conta_vicini(i, j, 7) + campo_visibile.conta_vicini(i, j, 8);
}

bool Campo::conta_se_numeri_vicini(int i, int j) const
{
	return campo_visibile.conta_se_vicini(i, j, 1) || campo_visibile.conta_se_vicini(i, j, 2) || campo_visibile.conta_se_vicini(i, j, 3) || campo_visibile.conta_se_vicini(i, j, 4) || campo_visibile.conta_se_vicini(i, j, 5) || campo_visibile.conta_se_vicini(i, j, 6) || campo_visibile.conta_se_vicini(i, j, 7) || campo_visibile.conta_se_vicini(i, j, 8);
}

bool Campo::bordo_non_scavato(int i, int j) const
{
	return campo_visibile.is_elemento(i, j, -3) && conta_se_numeri_vicini(i, j);
}

void Campo::aggiorna_cella(int i, int j)
{
	if (campo_nascosto[i][j]) campo_visibile[i][j] = -1;
	else if (conta_mine_vicine(i, j) == 0) campo_visibile[i][j] = 0;
	else campo_visibile[i][j] = conta_mine_vicine(i, j);
}

bool comando_lecito(char comando)
{
	return comando == 'B' || comando == 'T' || comando == 'S';
}

void Campo::scava_celle(int i, int j, Matrice<bool>& celle_processate)
{
	std::queue<Coord > coda;
	coda.push(Coord(i, j));
	while (coda.size() != 0)
	{
		Coord cella = coda.front();
		coda.pop();

		if (campo_visibile.indici_leciti(cella.first, cella.second) && campo_visibile[cella.first][cella.second] != -2 && !campo_nascosto[cella.first][cella.second] && !celle_processate[cella.first][cella.second])
		{
			aggiorna_cella(cella.first, cella.second);

			celle_processate[cella.first][cella.second] = true;
			if (conta_mine_vicine(cella.first, cella.second) == 0) // controlla che la casella non sia un fiore; in caso contrario non aggiungerà i nodi alla coda
			{
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
	}

	return;
}

void Campo::gioca(int i, int j, char comando)
{
	comando = std::toupper(comando);
	if (!comando_lecito(comando)) throw std::invalid_argument("comando illecito");
	
	if (comando == 'B' && campo_visibile[i][j] == -3)
	{
		campo_visibile[i][j] = -2;
		numero_bandiere++;
	}
	else if (comando == 'T' && campo_visibile[i][j] == -2)
	{
		campo_visibile[i][j] = -3;
		numero_bandiere--;
	}
	else if (campo_visibile[i][j] != -2)
	{
		if (campo_nascosto[i][j])
		{
			sconfitta(i, j);
			return;
		}
		else
		{
			aggiorna_cella(i, j);

			Matrice<bool> celle_processate(altezza, larghezza);

			scava_celle(i, j, celle_processate);
		}	
	}
	vittoria(); // controllo della vittoria TO DO: migliorare, se prendo tutte le mine con le bandiere dovrei poter comunque vincere
}

void Campo::randomizza_campo(int i, int j)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % altezza;
		int random2 = std::rand() % larghezza;
		if (mine >= altezza * larghezza - 8)
		{
			if ((random1 != i || random2 != j) && campo_nascosto[random1][random2] != 1)
			{
				campo_nascosto[random1][random2] = 1;
				k++;
			}
		}
		else
		{
			if (((random1 < i - 1 || random1 > i + 1) || (random2 < j - 1 || random2 > j + 1)) && campo_nascosto[random1][random2] != 1)
			{
				campo_nascosto[random1][random2] = 1;
				k++;
			}
		}
		if (k > mine) break;
	}
}

void Campo::reset()
{
	reset_gioco();
	reset_giocatore();
	reset_status();
	reset_numero_bandiere();
}
/*
void Campo::resize(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_altezza > 50 || input_larghezza < 1 || input_larghezza > 50) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	campo_nascosto = Matrice<bool> (altezza, larghezza);
	campo_visibile = Matrice<int> (altezza, larghezza, -3);
}
*/
void Campo::rivela()
{
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			aggiorna_cella(i, j);
		}
	}
	std::cout << *this;
}

void Campo::sconfitta(int x, int y)
{
	status = 'S';
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if (campo_nascosto[i][j]) campo_visibile[i][j] = -1;
		}
	}
	campo_visibile[x][y] = -4;
}

void Campo::vittoria()
{
	if (campo_visibile.conta_tutti_elemento(-2) + campo_visibile.conta_tutti_elemento(-3) == mine)
	{
		int mine_identificate = 0;
		for (int i = 0; i < altezza; i++)
		{
			for (int j = 0; j < larghezza; j++)
			{
				if ((campo_visibile.is_elemento(i, j, -2) || campo_visibile.is_elemento(i, j, -3)) && campo_nascosto[i][j]) mine_identificate++;
			}
		}
		if (mine_identificate == mine)
		{
			status = 'V';
			for (int i = 0; i < altezza; i++)
			{
				for (int j = 0; j < larghezza; j++)
				{
					if (campo_nascosto[i][j]) campo_visibile[i][j] = -2;
				}
			}
		}
	}
}

#endif // __CAMPO_H__