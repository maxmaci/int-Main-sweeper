// AUTORI:
// Massimo Bertolotti - massimo.bertolott86@edu.unito.it - Matricola n° 919899
//
// Guido Buffa		  - guido.buffa@edu.unito.it		 - Matricola n° 919899
// 
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

// Cerca il primo elemento di un vettore (eventualmente a partire da un indice dato) uguale ad uno dato in input e ne restituisce l'indice.
// Se non trova l'elemento o l'indice di partenza non è compreso fra 0 e la lunghezza del vettore - 1, restituisce un errore.
// INPUT: 
// •  (const std::vector<T>&) vettore: il vettore in cui cercare l'elemento
// •  (T) elemento: elemento da cercare nel vettore
// •  (int) indice_partenza: indice da cui far partire la ricerca
// OUTPUT:
// •  (int) : primo indice (dopo 'indice_partenza') di 'vettore' in cui si ha 'elemento'
template <typename T>
int trova_indice_elemento(const std::vector<T>& vettore, T elemento, int indice_partenza = 0)
{
	if (indice_partenza < 0 || indice_partenza >= vettore.size()) throw std::range_error("indice di partenza non lecito");
	typename std::vector<T>::const_iterator it;
	int res = static_cast<int>(std::find(vettore.cbegin() + indice_partenza, vettore.cend(), elemento) - vettore.cbegin());
	if (res == vettore.size()) throw std::domain_error("elemento non trovato");
	else return res;
}

// Cerca un elemento dato in input in un vettore e restituisce 'true' se lo trova.
// INPUT: 
// •  (const std::vector<T>&) vettore: il vettore in cui cercare l'elemento
// •  (T) elemento: elemento da cercare nel vettore
// OUTPUT:
// •  (bool) : 'true' se trova l'elemento nel vettore, 'false' altrimenti
template <typename T>
bool trova_elemento(const std::vector<T>& vettore, T elemento)
{
	typename std::vector<T>::const_iterator it;
	it = std::find(vettore.cbegin(), vettore.cend(), elemento);
	return it != vettore.end();
}

// Somma gli elementi di un vettore dato in input.
// INPUT: 
// •  (const std::vector<T>&) vettore: il vettore i cui elementi vanno sommati
// OUTPUT:
// •  (int) : la somma di tutti gli elementi
template <typename T>
int somma_elementi(const std::vector<T>& vettore)
{
	int res = 0;
	for (int i = 0; i < vettore.size(); i++) res += vettore[i];
	return res;
}

#endif // __VETTORE_H__