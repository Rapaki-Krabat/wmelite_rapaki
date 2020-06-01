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

	/**
	* No objects allowed, just pointers.
	*/
	wl_non_owning<T>(wl_non_owning<T>&& ref) = delete;

	/**
	* Return the object from the owner.
	*/
	const T getObject(void) const { return obj; }

};

template <typename T> class wl_owning
{
protected:
	T obj;

public:
	/**
	* Default constructor clears object.
	*/
	wl_owning<T>(void) { obj = nullptr; };

	/**
	* Constructor that initializes object. Convert NULL argument to nullptr.
	*/
	wl_owning<T>(T newObj) {
		if (newObj != NULL) 
		{ 
			obj = newObj; 
		}
		else 
		{ 
			obj = nullptr; 
		}
	};

	/**
	* Do not allow assignment to other objects.
	*/
	wl_owning<T>& operator=(const wl_owning<T>& ref) = delete;

	/**
	* Do not allow assignment to other objects.
	*/
	wl_owning<T>& operator=(wl_owning<T>&& ref) = delete;

	/**
	* No copy constructor.
	*/
	wl_owning<T>(const wl_owning<T>& ref) = delete;

	/**
	* No move constructor.
	*/
	wl_owning<T>(wl_owning<T>&& ref) = delete;

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
	* Check for a valid containing object.
	*/
	bool isValid(void) { return (obj != nullptr);  }
	
	/**
	* Invalidate the containing object.
	*/
	void invalidate(void) { if (obj != nullptr) { delete obj; obj = nullptr; } }

	/**
	* Replace the contained object with a new one, deleting the previous one if existing.
	*/
	void replace(T newObj) { 
		if (obj != nullptr) delete obj; 
		if (newObj != NULL)
		{
			obj = newObj;
		}
		else
		{
			obj = nullptr;
		}
	}

	/**
	* Overload class member access operator for easy reference of stuff inside the contained object.
	*/
	const T operator->() const { return obj; };

	/**
	* Need an exception for accessing the address of the contained object for load&save purposes.
	*/
	T* getPersistentObjectAddress(void) { return &obj; };

	/**
	* Delete the object if non-NULL.
	*/
	virtual ~wl_owning<T>(void) { invalidate(); }
};

#endif // WMELITE_POINTERS_H
