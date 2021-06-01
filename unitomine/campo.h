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
	int altezza;						// 1 < altezza < 50
	int larghezza;						// 1 < larghezza < 50
	int mine;							// 0 < mine < altezza * larghezza

	int numero_bandiere;				// 0 <= numero_bandiere < altezza * larghezza

	char status;						// '-': nè persa, nè vinta; 'S': sconfitta; 'V': vittoria.

	Matrice<bool> campo_nascosto;
	Matrice<int> campo_visibile;

/* METODI DI GIOCO */
	void scava_celle(int, int, Matrice<bool>&);				// metodo che 'scava' le celle (utilizzando l'algoritmo di Fill usato ad es. in Microsoft Paint)
	void aggiorna_cella(int, int);

/* METODI DI LETTURA DI GIOCO */
	int conta_mine_vicine(int, int) const;

/* SCONFITTA / VITTORIA */
	void sconfitta(int, int);								// visualizzando il campo con le mine, imposta lo status a S(confitta)
	void vittoria();										// controlla se le mine sono state tutte segnate con la bandierina. Se è così, imposta lo status a V(ittoria)

public:
/* COSTRUTTORE */
	// Costruisce un Campo vuoto di dimensioni altezza x larghezza con un numero di mine dato in input. Il costruttore NON piazza mine di suo.
	Campo(int = 9, int = 9, int = 10);
	// Costruisce un Campo partendo dal campo delle mine, che viene dato in input come matrice booleana.
	Campo(Matrice<bool>);

/* LEGGI CAMPI PRIVATI */
	// Restituisce l'altezza del campo di gioco.
	int _altezza() const { return altezza; };
	// Restituisce la larghezza del campo di gioco.
	int _larghezza() const { return larghezza; };
	// Restituisce il numero di mine presenti nel campo di gioco (nascosto).
	int _mine() const { return mine; };
	// Restituisce il numero di bandiere piazzate nel campo di gioco (visibile) 
	int _numero_bandiere() const { return numero_bandiere; };
	// Restituisce lo status della partita.
	char _status() const { return status; };
	// Restituisce il campo visibile al giocatore.
	Matrice<int> _campo_visibile() const { return campo_visibile; };

/* FUNZIONI DI GENERAZIONE DEL CAMPO */
	
	// Pone  nel campo nascosto in modo casuale un numero di mine (cioè di celle uguali a 'true') pari al valore del campo privato 'mine' di Campo. Se è possibile, evita di piazzare mine nelle celle intorno alle coordinate date in input.
	void randomizza_campo(int, int);

/* METODI DI RESET */
	
	// Elimina le mine dal campo nascosto.
	void reset_campo_nascosto() { campo_nascosto.sostituisci_tutti(); };
	// Elimina qualunque modifica (bandiere, celle scavate, ecc...) fatta sul campo visibile, sostituendo tutte le celle con celle non scavate in modo da riportarlo allo stato originale.
	void reset_campo_visibile() { campo_visibile.sostituisci_tutti(-3); };
	// Resetta lo status del campo a '-'.
	void reset_status() { status = '-'; };
	// Resetta il numero delle bandiere piazzata a 0.
	void reset_numero_bandiere() { numero_bandiere = 0; };
	// Resetta il campo nascosto, il campo visibile, lo status e il numero delle bandiere piazzate contemporaneamente.
	void reset();											

/* METODI DI GIOCO */

	//void resize(int, int, int); // TO DO: eliminare	// aggiorna il campo da gioco e del giocatore con nuovi valori di altezza, larghezza e mine

	// Compie sulla cella (i, j) data in input l'azione di gioco sulla base del comando dato in input.
	void gioca(int, int, char);
	// Modifica il campo visibile "scavando" tutto il campo e mostrando tutto al giocatore (sia numeri, sia mine).
	void rivela();

/* FUNZIONI DI LETTURA STATUS */
	// Conta il numero di celle non scavate presenti nelle 8 (se nell'interno della matrice), nelle 5 (se sul bordo) o nelle 3 posizioni (se nell'angolo) attorno alla cella (i, j).
	int conta_non_scavati_vicini(int, int) const;
	// Conta il numero di bandiere presenti nelle 8 (se nell'interno della matrice), nelle 5 (se sul bordo) o nelle 3 posizioni (se nell'angolo) attorno alla cella (i, j).
	int conta_bandiere_vicine(int, int) const;
	// Conta quanti numeri sono presenti nelle 8 (se nell'interno della matrice), nelle 5 (se sul bordo) o nelle 3 posizioni (se nell'angolo) attorno alla cella (i, j).
	int conta_numeri_vicini(int i, int j) const; // TO DO: l'unico uso che ne facciamo è di verificare se ce ne sono, non quanti. Potrebbe sostituirlo con un conta se numeri vicini

	// TO DO: sostituire non scavati e bandiere vicine col metodo conta vicini di matrice, spostandolo qui - non ha senso contare gli elementi vicini ad una matrice o no?


	/* FUNZIONI DI STAMPA */
	// Stampa in modo carino il campo, con attorno le coordinate per facilitare l'utente.
	friend std::ostream& operator<<(std::ostream&, const Campo&);
};

// Genera il campo di gioco, verifica che le dimensioni fornite dall'utente siano accettabili (compreso strettamente tra un quadro 2x2 e uno 50x50)
// Verifica anche che il numero di mine sia conforme con la convenzione di Ginevra (deve essere compreso tra 1 e area-del-campo - 1)
Campo::Campo(int input_altezza, int input_larghezza, int input_mine)
{
	if (input_altezza < 2 || input_altezza > 50 ||  input_larghezza < 2 || input_larghezza > 50) throw std::domain_error("dimensioni del campo invalide");
	if (input_mine < 1 || input_mine >= input_altezza * input_larghezza) throw std::domain_error("numero delle mine illegale");
	
	campo_nascosto = Matrice<bool>(input_altezza, input_larghezza);		// Matrice booleana, contiene solo informazioni sulla presenza di mine
	campo_visibile = Matrice<int>(input_altezza, input_larghezza, -3);	// Matrice a valori interi, contiene tutte le altre infomazioni utili al gioco (e al giocatore)

	altezza = input_altezza;
	larghezza = input_larghezza;
	mine = input_mine;
	numero_bandiere = 0;
	status = '-';
}

// Genera il campo di gioco a partire dallo schema fornito, verificando che sia un formato valido (rettangolare, con condizioni sulle dimensioni come prima)
Campo::Campo(Matrice<bool> campo_input)
{
	if (campo_input._righe() < 2 || campo_input._righe() > 50 || campo_input._colonne() < 2 || campo_input._colonne() > 50) throw std::domain_error("dimensioni del campo invalide");
	if (campo_input.conta_tutti_elementi(true) < 1 || campo_input.conta_tutti_elementi(true) >= campo_input._righe() * campo_input._colonne()) throw std::domain_error("numero delle mine illegale");

	campo_nascosto = campo_input;
	campo_visibile = Matrice<int>(campo_input._righe(), campo_input._colonne(), -3);

	altezza = campo_input._righe();
	larghezza = campo_input._colonne();
	mine = campo_input.conta_tutti_elementi(true);
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

// È la funzione che scava in sequenza quando si "clicca" su una cella senza numeri, in maniera che continui a scavare fin quando non trova i bordi 
// del campo oppure delle celle numerate; sfrutta la caratterizzazione delle celle vuote (identificate come "-1"), utilizzando poi oggetti di tipo
// queue per continuare a scavare "a cascata" in tutte le direzioni in base alla cella di partenza, applicando lo stesso processo anche alle celle
// apperna scavate e aggiornando la coda per ridurre lo sforzo computazionale (si rischia di far "rimbalzare" i check sulle celle da una parte 
// all'altra del campo all'infinito)
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
			if (conta_mine_vicine(cella.first, cella.second) == 0) // controlla che la casella non sia una mina; in caso contrario non aggiungerà i nodi alla coda
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

// Applica i comandi dati in input o da Il Risolutore™ al campo, verificando che siano leciti
void Campo::gioca(int i, int j, char comando)
{
	comando = std::toupper(comando);
	if (!comando_lecito(comando)) throw std::invalid_argument("comando illecito");
	
	if (comando == 'B' && campo_visibile[i][j] == -3) // Aggiorna i contatori di bandiere segnalandone ("B") o togliendone ("T")
	{
		campo_visibile[i][j] = -2;
		numero_bandiere++;
	}
	else if (comando == 'T' && campo_visibile[i][j] == -2)
	{
		campo_visibile[i][j] = -3;
		numero_bandiere--;
	}
	else if (campo_visibile[i][j] != -2) // Segnala la sconfitta se si scava su di una mina, altrimenti esegue l'operazione di scavo e aggiorna il campo
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
	vittoria();
}

// Una volta scelte le dimensioni del campo e giocata la prima mossa genera il campo di mine, applicando un metodo basato sul numero di mine 
// scelto e selezionando in maniera casuale le coordinate dove piazzare le mine (tolto il blocco 3x3 circostante alla cella della prima mossa)
// fino al raggiungimento del numero finale delle mine
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
	reset_campo_nascosto();
	reset_campo_visibile();
	reset_status();
	reset_numero_bandiere();
}

// Svela le mine del campo se si sceglie questa opzione dal menù
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

// In caso di sconfitta vengono mostrate le mine rimanenti e viene evidenziata quella fatta esplodere
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
	if (campo_visibile.conta_tutti_elementi(-2) + campo_visibile.conta_tutti_elementi(-3) == mine)
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