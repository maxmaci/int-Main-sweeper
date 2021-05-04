#ifndef __GIOCO_H__
#define __GIOCO_H__

#include <iterator>		// classe iterator
#include <utility>		// classe pair
#include <queue>

#include "campo.h"
#include "input.h"

class Gioco
{
private:
	int altezza;						// altezza > 0
	int larghezza;						// larghezza > 0
	int mine;							// 0 < mine < altezza * larghezza
	int numero_bandiere;				// 0 <= numero_bandiere < altezza * larghezza
	char status;						// '-': nè persa, nè vinta; 'S': sconfitta; 'V': vittoria. 
	Campo<bool> campo_gioco;
	Campo<int> campo_giocatore;

	/* METODI DI GIOCO */
	void scava_celle(int, int, Campo<bool>&);				// metodo che 'scava' le celle (utilizzando l'algoritmo di Fill usato ad es. in Microsoft Paint)
	void aggiorna_cella(int, int);

	/* SCONFITTA / VITTORIA */
	void sconfitta(int, int);								// visualizzando il campo con le mine, imposta lo status a S(confitta)
	void vittoria();										// controlla se le mine sono state tutte segnate con la bandierina. Se è così, imposta lo status a V(ittoria)

public:
	/* COSTRUTTORE */
	Gioco(int, int, int);

	/* LEGGI CAMPI PRIVATE DI CLASS GIOCO */
	int _altezza() const { return altezza; };
	int _larghezza() const { return larghezza; };
	int _mine() const { return mine; };
	int _numero_bandiere() const { return numero_bandiere; };
	char _status() const { return status; };
	Campo<bool> _campo_gioco() const { return campo_gioco; };
	Campo<int> _campo_giocatore() const { return campo_giocatore; };

	/* FUNZIONI DI GENERAZIONE DEL CAMPO */
	void randomizza_campo(int, int);						// randomizza il campo con le mine 

	/* FUNZIONI DI RESET */
	void reset_gioco() { campo_gioco.reset(); };			// pulisce il campo da gioco dalle mine
	void reset_giocatore() { campo_giocatore.reset(-3); };	// pulisce il campo dell giocatore allo stato originale
	void reset_status() { status = '-'; };					// resetta lo status
	void reset_numero_bandiere() { numero_bandiere = 0; };	// resetta il numero di bandiere segnate
	void reset();											// pulisce il campo da gioco e del giocatore, resetta lo status a '-'

	/* METODI DI GIOCO */
	void resize(int, int, int);								// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine
	void gioca(int, int, char);								// compie le azioni di gioco
	void rivela();											// rivela il campo da gioco al giocatore nelle opzioni

	/* FUNZIONI DI LETTURA */
	int conta_mine_vicine(int, int) const;
	int conta_non_scavati_vicini(int, int) const;
	int conta_bandiere_vicine(int, int) const;
	bool bordo_non_scavato(int, int) const;
};

Gioco::Gioco(int input_altezza, int input_larghezza, int input_mine)
	: campo_gioco(input_altezza, input_larghezza), campo_giocatore(input_altezza, input_larghezza, -3)
{
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	numero_bandiere = 0;
	status = '-';
}

int Gioco::conta_non_scavati_vicini(int i, int j) const
{
	return campo_giocatore.conta_vicini(i, j, -3);
	
	/*
	if (!campo_gioco.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo_gioco[i][j] == 1) return -1;

	int k = 0;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo_gioco.nel_campo(n, m) && campo_giocatore[n][m] == -3) k++;
		}
	}
	return k;
	*/
}

int Gioco::conta_bandiere_vicine(int i, int j) const
{
	return campo_giocatore.conta_vicini(i, j, -2);
	
	/*
	if (!campo_gioco.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo_gioco[i][j] == 1) return -1;

	int k = 0;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo_gioco.nel_campo(n, m) && campo_giocatore[n][m] == -2) k++;
		}
	}
	return k;
	*/
}

int Gioco::conta_mine_vicine(int i, int j) const
{
	return campo_gioco.conta_vicini(i, j, true);
	
	/*
	if (!campo_gioco.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo_gioco[i][j] == 1) return -1;

	int k = 0;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo_gioco.nel_campo(n, m) && campo_gioco[n][m] == 1) k++;
		}
	}
	return k;
	*/
}

bool Gioco::bordo_non_scavato(int i, int j) const
{
	return campo_giocatore[i][j] == -3 && (campo_giocatore.conta_se_vicini(i, j, 1) || campo_giocatore.conta_se_vicini(i, j, 2) || campo_giocatore.conta_se_vicini(i, j, 3) ||	campo_giocatore.conta_se_vicini(i, j, 4) ||	campo_giocatore.conta_se_vicini(i, j, 5) ||	campo_giocatore.conta_se_vicini(i, j, 6) ||	campo_giocatore.conta_se_vicini(i, j, 7) ||	campo_giocatore.conta_se_vicini(i, j, 8));
	/*
	if (!campo_gioco.nel_campo(i, j)) throw std::domain_error("controllo su cella illegittima");
	if (campo_giocatore[i][j] != -3) return false;

	for (int n = i - 1; n <= i + 1; n++)
	{
		for (int m = j - 1; m <= j + 1; m++)
		{
			if (campo_gioco.nel_campo(n, m) && campo_giocatore[n][m] > 0) return true;
		}
	}
	return false; */
}

void Gioco::aggiorna_cella(int i, int j)
{
	if (conta_mine_vicine(i, j) == -1) campo_giocatore[i][j] = -1;
	else if (conta_mine_vicine(i, j) == 0) campo_giocatore[i][j] = 0;
	else campo_giocatore[i][j] = conta_mine_vicine(i, j);
}

bool comando_lecito(char comando)
{
	return comando == 'B' || comando == 'T' || comando == 'S';
}

void Gioco::scava_celle(int i, int j, Campo<bool>& celle_processate)
{
	//auto start = std::chrono::steady_clock::now();
	
	std::queue<std::pair<int, int> > coda;
	coda.push(std::pair<int, int>(i, j));
	while (coda.size() != 0)
	{
		std::pair<int, int> cella = coda.front();
		coda.pop();

		if (campo_giocatore.nel_campo(cella.first, cella.second) && campo_giocatore[cella.first][cella.second] != -2 && !campo_gioco[cella.first][cella.second] && !celle_processate[cella.first][cella.second])
		{
			aggiorna_cella(cella.first, cella.second);

			celle_processate[cella.first][cella.second] = true;
			if (conta_mine_vicine(cella.first, cella.second) == 0) // controlla che la casella non sia un fiore; in caso contrario non aggiungerà i nodi alla coda
			{
				coda.push(std::pair<int, int>(cella.first - 1, cella.second - 1));
				coda.push(std::pair<int, int>(cella.first - 1, cella.second));
				coda.push(std::pair<int, int>(cella.first - 1, cella.second + 1));
				coda.push(std::pair<int, int>(cella.first, cella.second - 1));
				coda.push(std::pair<int, int>(cella.first, cella.second + 1));
				coda.push(std::pair<int, int>(cella.first + 1, cella.second - 1));
				coda.push(std::pair<int, int>(cella.first + 1, cella.second));
				coda.push(std::pair<int, int>(cella.first + 1, cella.second + 1));
			}
		}
	}

	//auto end = std::chrono::steady_clock::now();

	//auto diff = end - start;

	//std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;

	return;
}

void Gioco::gioca(int i, int j, char comando)
{
	comando = std::toupper(comando);
	if (!comando_lecito(comando)) throw std::invalid_argument("comando illecito");
	
	if (comando == 'B')
	{
		if (campo_giocatore[i][j] == -3)
		{
			campo_giocatore[i][j] = -2;
			numero_bandiere++;
			return;
		}
		else return;
	}
	else if (comando == 'T')
	{
		if (campo_giocatore[i][j] == -2)
		{
			campo_giocatore[i][j] = -3;
			numero_bandiere--;
			return;
		}
		else return;
	}
	else
	{
		if (campo_giocatore[i][j] == -2)
		{
			return;
		}
		else if (campo_gioco[i][j])
		{
			sconfitta(i, j);
			return;
		}
		else
		{
			aggiorna_cella(i, j);
			
			Campo<bool> celle_processate(altezza, larghezza);

			scava_celle(i, j, celle_processate);

			// controllo della vittoria TO DO: migliorare, se prendo tutte le mine con le bandiere dovrei poter comunque vincere
			if (campo_giocatore.conta_tutti_nulli() == mine)
			{
				vittoria();
			}

			return;
		}
	}
	
}

void Gioco::randomizza_campo(int i, int j)
{
	int k = 1;
	while (true)
	{
		int random1 = std::rand() % altezza;
		int random2 = std::rand() % larghezza;
		if (mine >= altezza * larghezza - 8)
		{
			if ((random1 != i || random2 != j) && campo_gioco[random1][random2] != 1)
			{
				campo_gioco[random1][random2] = 1;
				k++;
			}
		}
		else
		{
			if (((random1 < i - 1 || random1 > i + 1) || (random2 < j - 1 || random2 > j + 1)) && campo_gioco[random1][random2] != 1)
			{
				campo_gioco[random1][random2] = 1;
				k++;
			}
		}
		if (k > mine) break;
	}
}

void Gioco::reset()
{
	reset_gioco();
	reset_giocatore();
	reset_status();
	reset_numero_bandiere();
}

void Gioco::resize(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_altezza > 99 || input_larghezza < 1 || input_larghezza > 99) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	campo_gioco.resize(altezza, larghezza, 0);
	campo_giocatore.resize(altezza, larghezza, -3);
}

void Gioco::rivela()
{
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			aggiorna_cella(i, j);
		}
	}
	std::cout << campo_giocatore;
}

void Gioco::sconfitta(int x, int y)
{
	campo_giocatore[x][y] = -4;
	status = 'S';
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if ((i != x || j != y) && conta_mine_vicine(i, j) == -1) campo_giocatore[i][j] = -1;
		}
	}
}

void Gioco::vittoria()
{
	int mine_identificate = 0;
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if (campo_giocatore.is_nullo(i, j) && campo_gioco[i][j] == 1) mine_identificate++;
		}
	}
	if (mine_identificate == mine)
	{
		status = 'V';
	}
}

#endif // __GIOCO_H__