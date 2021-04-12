#include <iostream>
#include <vector>
#include <map>
#include <array>

/*
class Matrix {
private:
	int rowno;
	int colno;
	std::vector<Vector> mat;
	void init(int, int);
public:
	Matrix(int, int);
	Matrix(Vector);
	int row_number() const { return rowno; };
	int col_number() const { return colno; };
	Vector size() const;
	Vector col(int) const;
	Vector row(int) const;
	//int el(int, int) const;
	//int& el(int, int);
	Vector operator[](int) const;	// read row
	Vector& operator[](int);		// write row
	Matrix transpose() const;
	Matrix mul(int) const;
	Matrix add(const Matrix&) const;
	Matrix neg() const;
	Matrix sub(const Matrix&) const;
	Matrix mul(const Matrix&) const;
	Matrix pow(int) const;
	int digits() const;
};
*/

class Cella {
private:
	bool mina;
	char stato; // n (normale), s (scavato), b (bandiera) 
public:
	Cella(bool, char);												// costruttore
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
	Cella operator[](int) const;											// leggi elemento di data
	Cella& operator[](int);													// scrivi elemento di data
	friend std::ostream& operator<<(std::ostream&, const VettoreCella&);	// stampa vettore cella su terminale
};

VettoreCella::VettoreCella(int l = 1) {
	if (l < 1) throw std::domain_error("dimensioni vettore invalide");
	data.resize(l, Cella());
}

Cella VettoreCella::operator[](int i) const {
	return data.at(i);
}

Cella& VettoreCella::operator[](int i) {
	return data.at(i);
}

class Campo {
private:
	int righe;								// righe > 0
	int colonne;							// colonne > 0
	std::vector<VettoreCella> campo;
public:
	Campo(int, int);					// costruttore
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

class Gioco {
private:
	int altezza;	// altezza > 0
	int larghezza;	// larghezza > 0
	int mine;		// 0 < mine < altezza * larghezza
public:
	void giocare();
};

void Gioco::giocare () {
	// ???
}

int main() {
	Cella d;
	Campo c(5, 5);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			std::cout << c[i][j];
		}
	std::cout << std::endl;
	}
}