#ifndef __VETTORE_H__
#define __VETTORE_H__

#ifdef _WIN32
// Se compilato su un computer con Windows includiamo la libreria windows.h, necessaria per la compatibilità UTF-8.
// È consigliato usare Dejavù Sans Mono dato che supporta molti caratteri unicode.
// Per motivi di incompatibilità di macro della libreria, useremo la notazione con parentesi per (std::min) and (std::max).
#include <windows.h>
#endif

#include <iterator>		// classe iterator
#include <iostream>		// std::cout / std::cin
#include <vector>		// classe vector

template <typename T>
class Vector {
private:
	std::vector<T> data;
public:
	Vector();
	T operator[](int) const;
	T& operator[](int);
	Vector<T> add(const Vector<T>&) const;
	Vector<T> sub(const Vector<T>&) const;
	Vector<T> mul(T) const;
	T mul(const Vector<T>&) const;
};

#endif // __VETTORE_H__