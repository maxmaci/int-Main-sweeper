#include <iostream>
#include <vector>
#include <map>
#include <array>

class Cella {
private:
	bool mina;
	char stato; // n (normale), s (scavato), b (bandiera) 
public:
	Cella(bool, char);												// costruttore
	void Cambia_stato();
	friend std::ostream& operator<<(std::ostream&, const Cella&);	// stampa cella su terminale
};

Cella::Cella(bool m = false, char s = 'n') {
	if (s != 'n' && s != 's' && s != 'b') throw std::domain_error("stato illegittimo");
	mina = m;
	stato = s;
}

std::ostream& operator<<(std::ostream& os, const Cella& c) {
	os << "(" << c.mina << ", " << c.stato << ")";
	return os;
}

class VettoreCella {
private:
	int lunghezza;				// lunghezza > 0
	std::vector<Cella> data;
public:
	VettoreCella(int);														// costruttore
	int dimensione() const { return lunghezza; };
	Cella operator[](int) const;											// leggi elemento di data
	Cella& operator[](int);													// scrivi elemento di data
	friend std::ostream& operator<<(std::ostream&, const VettoreCella&);	// stampa vettore cella su terminale
};

VettoreCella::VettoreCella(int l = 1) {
	if (l < 1) throw std::domain_error("dimensioni vettore invalide");
	lunghezza = l;
	data.resize(l, Cella());
}

Cella VettoreCella::operator[](int i) const {
	return data.at(i);
}

Cella& VettoreCella::operator[](int i) {
	return data.at(i);
}

std::ostream& operator<<(std::ostream& os, const VettoreCella& c) {
	for (int i = 0; i < c.dimensione(); i++)
		os << c[i];
	return os;
}

class Campo {
private:
	int righe;								// righe > 0
	int colonne;							// colonne > 0
	std::vector<VettoreCella> campo;
public:
	Campo(int, int);					// costruttore
	int _righe() const { return righe; };
	int _colonne() const { return colonne; };
	VettoreCella operator[](int) const;	// leggi riga Campo (come vettore di Cella)
	VettoreCella& operator[](int);		// scrivi riga Campo (come vettore di Cella)
	friend std::ostream& operator<<(std::ostream&, const Campo&);
};

Campo::Campo(int r, int c) {
	if (r < 1 || c < 1) throw std::domain_error("dimensioni campo invalide");
	righe = r;
	colonne = c;
	campo.resize(r, VettoreCella(c));
}

VettoreCella Campo::operator[](int i) const {
	return campo.at(i);
}

VettoreCella& Campo::operator[](int i) {
	return campo.at(i);
}

std::ostream& operator<<(std::ostream& os, const Campo& c) {
	for (int i = 0; i < c._righe(); i++) {
		for (int j = 0; j < c._colonne(); j++) {
			os << c[i][j];
		}
		os << std::endl;
	}
	return os;
}

class Gioco {
private:
	int altezza;	// altezza > 0
	int larghezza;	// larghezza > 0
	int mine;		// 0 < mine < altezza * larghezza
public:
	void giocare();
};

void Gioco::giocare() {
	// ???
}

int main() {
	Cella d;
	Campo c(5, 5);
	std::cout << c;
}