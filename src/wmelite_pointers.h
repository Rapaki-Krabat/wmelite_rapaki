#ifndef WMELITE_POINTERS_H
#define WMELITE_POINTERS_H

#include "BPersistMgr.h"

template <typename T> class wl_owning;

template <typename T> class wl_non_owning : public wl_owning<T>
{
public:
	/**
	* No objects allowed, just pointers.
	*/
	wl_non_owning<T>(void) = delete;

	/**
	* No objects allowed, just pointers.
	*/
	wl_non_owning<T>(const wl_non_owning<T>& ref) = delete;
};

template <typename T> class wl_owning
{
public:
	T obj;

	/**
	* Constructor clears object.
	*/
	wl_owning<T>(void) { obj = NULL; };

	/**
	* Do not allow assignment to other objects.
	*/
	wl_owning<T>& operator=(const wl_owning<T>& ref) = delete;

	/**
	* No copy constructor.
	*/
	wl_owning<T>(const wl_owning<T>& ref) = delete;

	/**
	* Do not allow comparison.
	*/
	bool operator==(const wl_owning<T>& other) = delete;
	bool operator!=(const wl_owning<T>& other) = delete;

	/**
	* Return a non-owning pointer;
	*/
	wl_non_owning<T>* toNonOwning() { return static_cast<wl_non_owning<T>*> (this); };

	/**
	* Delete the object if non-NULL.
	*/
	virtual ~wl_owning<T>(void) { if (obj != NULL) delete obj; }
};

#endif // WMELITE_POINTERS_H
