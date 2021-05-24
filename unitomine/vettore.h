#ifndef __VETTORE_H__
#define __VETTORE_H__

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max).
#include <windows.h>
#endif

#include <iterator>		// classe iterator
#include <iostream>		// std::cout / std::cin
#include <vector>		// classe vector

template <typename T>
class Vector {				//TO DO: sistemare la questione dei nomi della classe
private:
	std::vector<T> data;
public:
	Vector();
	T operator[](int) const;
	T& operator[](int);

};

template <typename T>


int trova_indice_elemento(const std::vector<T>& vettore, T elemento, int indice_partenza = 0)
{
	if (indice_partenza > vettore.size()) throw std::range_error("indice di partenza non lecito");
	typename std::vector<T>::const_iterator it;
	int res = static_cast<int>(std::find(vettore.cbegin() + indice_partenza, vettore.cend(), elemento) - vettore.cbegin());
	if (res == vettore.size()) throw std::domain_error("elemento non trovato");
	else return res;
}

template <typename T>
bool trova_elemento(const std::vector<T>& vettore, T elemento)
{
	typename std::vector<T>::const_iterator it;
	it = std::find(vettore.cbegin(), vettore.cend(), elemento);
	return it != vettore.end();
}

template <typename T>
int somma_elementi(const std::vector<T>& vettore)
{
	int res = 0;
	for (int i = 0; i < vettore.size(); i++) res += vettore[i];
	return res;
}

#endif // __VETTORE_H__