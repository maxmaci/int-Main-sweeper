#ifndef __GIOCO_H__
#define __GIOCO_H__

#include <iterator>		// classe iterator
#include <utility>		// classe pair

#include "campo.h"

class Gioco
{
private:
	int altezza;						// altezza > 0
	int larghezza;						// larghezza > 0
	int mine;							// 0 < mine < altezza * larghezza
	int numero_bandiere;				// 0 <= numero_bandiere < altezza * larghezza
	char status;						// '-': nè persa, nè vinta; 'S': sconfitta; 'V': vittoria. 
	Campo<bool> campo_gioco;
	Campo<std::string> campo_giocatore;
public:
	/* COSTRUTTORE */
	Gioco(int, int, int);

	/* LEGGI CAMPI PRIVATE DI CLASS GIOCO */
	int _altezza() { return altezza; };
	int _larghezza() { return larghezza; };
	int _mine() { return mine; };
	int _numero_bandiere() { return numero_bandiere; };
	char _status() { return status; };
	Campo<bool> _campo_gioco() { return campo_gioco; };
	Campo<std::string> _campo_giocatore() { return campo_giocatore; };

	/* FUNZIONI DI GENERAZIONE DEL CAMPO */
	void randomizza_campo();								// randomizza il campo con le mine 
	void randomizza_campo(int, int);

	/* FUNZIONI DI RESET */
	void reset_gioco() { campo_gioco.reset(); };			// pulisce il campo da gioco dalle mine
	void reset_giocatore() { campo_giocatore.reset(); };	// pulisce il campo dell giocatore allo stato originale
	void reset_status() { status = '-'; };					// resetta lo status
	void reset_numero_bandiere() { numero_bandiere = 0; };	// resetta il numero di bandiere segnate
	void reset();											// pulisce il campo da gioco e del giocatore, resetta lo status a '-'

	/* FUNZIONI DI GIOCO */
	void aggiorna(int, int, int);							// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine
	void scava_celle(int, int, Campo<bool>&);				// funzione che 'scava' le celle
	void gioca(int, int, char);								// compie le azioni di gioco

	/* FUNZIONI DI STATUS */
	Campo<std::string> rivela();							// rivela il campo da gioco al giocatore nelle opzioni
	void sconfitta(int, int);								// visualizzando il campo con le mine, imposta lo status a S(confitta)
	void vittoria();										// controlla se le mine sono state tutte segnate con la bandierina. Se è così, imposta lo status a V(ittoria)
};

Gioco::Gioco(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_larghezza < 1) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	numero_bandiere = 0;
	status = '-';
	campo_gioco = Campo<bool>(altezza, larghezza);
	campo_giocatore = Campo<std::string>(altezza, larghezza);
}

bool comando_lecito(char comando)
{
	return comando == 'B' || comando == 'T' || comando == 'S';
}

void Gioco::scava_celle(int i, int j, Campo<bool>& processate)
{
	std::vector<std::pair<int, int> > coda;
	coda.push_back(std::pair<int, int>(i, j));
	while (coda.size() != 0)
	{
		std::pair<int, int> cella = coda[0];
		coda.erase(coda.begin());

		if (campo_giocatore.nel_campo(cella.first, cella.second) && campo_giocatore[cella.first][cella.second] != u8"⚑" && !campo_gioco[cella.first][cella.second] && !processate[cella.first][cella.second])
		{
			if (conta_mine(campo_gioco, cella.first, cella.second) == 0) campo_giocatore[cella.first][cella.second] = "-";          //TO DO: Rendere funzione
			else campo_giocatore[cella.first][cella.second] = std::to_string(conta_mine(campo_gioco, cella.first, cella.second));

			processate[cella.first][cella.second] = true;
			if (conta_mine(campo_gioco, cella.first, cella.second) == 0) // controlla che la casella non sia un fiore; in caso contrario non aggiungerà i nodi alla coda
			{
				coda.push_back(std::pair<int, int>(cella.first - 1, cella.second - 1));
				coda.push_back(std::pair<int, int>(cella.first - 1, cella.second));
				coda.push_back(std::pair<int, int>(cella.first - 1, cella.second + 1));
				coda.push_back(std::pair<int, int>(cella.first, cella.second - 1));
				coda.push_back(std::pair<int, int>(cella.first, cella.second + 1));
				coda.push_back(std::pair<int, int>(cella.first + 1, cella.second - 1));
				coda.push_back(std::pair<int, int>(cella.first + 1, cella.second));
				coda.push_back(std::pair<int, int>(cella.first + 1, cella.second + 1));
			}
		}
	}
	return;
}

void Gioco::gioca(int i, int j, char comando)
{
	comando = std::toupper(comando);
	if (!comando_lecito(comando)) throw std::invalid_argument("comando illecito");
	if (comando == 'B')
	{
		if (campo_giocatore[i][j] == u8"⎕")
		{
			campo_giocatore[i][j] = u8"⚑";
			numero_bandiere++;
			return;
		}
		else return;
	}
	else if (comando == 'T')
	{
		if (campo_giocatore[i][j] == u8"⚑")
		{
			campo_giocatore[i][j] = u8"⎕";
			numero_bandiere--;
			return;
		}
		else return;
	}
	else
	{
		if (campo_giocatore[i][j] == u8"⚑")
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
			if (conta_mine(campo_gioco, i, j) == 0) campo_giocatore[i][j] = "-";
			else campo_giocatore[i][j] = std::to_string(conta_mine(campo_gioco, i, j));
			
			Campo<bool> processate(altezza, larghezza);

			scava_celle(i, j, processate);
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

void Gioco::aggiorna(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 1 || input_larghezza < 1) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	campo_gioco.resize(altezza, larghezza, 0);
	campo_giocatore.resize(altezza, larghezza, u8"⎕");
}

Campo<std::string> Gioco::rivela()
{
	Campo<std::string> res(altezza, larghezza);
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if (conta_mine(campo_gioco, i, j) == -1) res[i][j] = u8"◉";
			else if (conta_mine(campo_gioco, i, j) == 0) res[i][j] = "-";
			else res[i][j] = std::to_string(conta_mine(campo_gioco, i, j));
		}
	}
	return res;
}

void Gioco::sconfitta(int x, int y)
{
	campo_giocatore[x][y] = u8"\x1B[48;2;159;0;1m\x1B[38;2;255;255;255m◉\x1B[48;2;192;192;192m";
	status = 'S';
	for (int i = 0; i < altezza; i++)
	{
		for (int j = 0; j < larghezza; j++)
		{
			if ((i != x || j != y) && conta_mine(campo_gioco, i, j) == -1) campo_giocatore[i][j] = u8"◉";
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