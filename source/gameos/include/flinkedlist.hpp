//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include "gameos.hpp"
#include "MemoryManager.hpp"

class ListItem
{
public:
	virtual ~ListItem() { }
};

template <class T>
class FLink
{
public:
	FLink<T>* Next;
	T* linkData;
	ListItem* castedData;

public:
	FLink()
	{
		linkData = nullptr;
		Next = nullptr;
	}
	FLink(T* ptr)
	{
		castedData = static_cast<ListItem*>(ptr);
		linkData = ptr;
		Next = nullptr;
	}
	virtual ~FLink()
	{
		Next = nullptr;
	}

	FLink<T>* GetNext()
	{
		return (FLink<T>*)Next;
	}
};

template <class T>
class FLinkedList
{
protected:
	FLink<T>* m_Tail;
	FLink<T>* m_Iterator;
	int32_t m_Size;

public:
	friend class FLink;

	FLink<T>* m_Head;

	FLinkedList()
	{
		m_Head = nullptr;
		m_Tail = nullptr;
		m_Size = 0;
	};
	FLinkedList(T* ptr)
	{
		FLink<T>* newlink = (FLink<T>*)malloc(sizeof(FLink<T>));
		newlink->castedData = static_cast<ListItem*>(ptr);
		newlink->linkData = ptr;
		newlink->Next = nullptr;
		m_Head = newlink;
		m_Head->Next = nullptr;
		m_Size = 1;
	};
	~FLinkedList()
	{
		while (m_Head != nullptr)
			Remove(m_Head->linkData);
	};

	void Add(T* ptr)
	{
		FLink<T>* newlink = (FLink<T>*)malloc(sizeof(FLink<T>));
		newlink->castedData = static_cast<ListItem*>(ptr);
		newlink->linkData = ptr;
		newlink->Next = nullptr;
		if (m_Head == nullptr)
		{
			m_Head = newlink;
			m_Tail = newlink;
			newlink->Next = nullptr;
		}
		else
		{
			m_Tail->Next = newlink;
			m_Tail = newlink;
			newlink->Next = nullptr;
		}
		m_Size += 1;
	};

	void Add(T* ptr, PVOIDlink_memory)
	{
		gosASSERT(link_memory != nullptr);
		FLink<T>* link = (FLink<T>*)link_memory;
		if (m_Head == nullptr)
		{
			m_Head = link;
			m_Tail = link;
			link->Next = nullptr;
		}
		else
		{
			m_Tail->Next = link;
			m_Tail = link;
		}
		m_Size++;
	};

	void InsertAfterCurrent(T* ptr)
	{
		if (!m_Head)
		{
			Add(ptr);
		}
		else
		{
			FLink<T>* newlink = (FLink<T>*)malloc(sizeof(FLink<T>));
			newlink->castedData = static_cast<ListItem*>(ptr);
			newlink->linkData = ptr;
			newlink->Next = nullptr;
			FLink<T>* tmp;
			gosASSERT(m_Iterator);
			tmp = m_Iterator->Next;
			m_Iterator->Next = new_link;
			new_link->Next = tmp;
		}
	};

	void Remove(T* ptr, BOOLEAN delete_data = false)
	{
		FLink<T>* tmp = m_Head;
		if ((T*)tmp->linkData == ptr)
		{
			m_Size -= 1;
			m_Head = tmp->Next;
			if (m_Tail == tmp)
			{
				m_Tail = m_Head;
				m_Iterator = m_Head;
			}
			else if (m_Iterator == tmp)
			{
				m_Iterator = tmp->Next;
			}
			if (delete_data)
			{
				delete tmp->castedData;
			}
			free(tmp);
		}
		else
		{
			FLink<T>* target;
			while (tmp->Next != nullptr)
			{
				if (tmp->Next->linkData == ptr)
				{
					m_Size -= 1;
					target = tmp->Next;
					// Rewind tail or iterator if they point
					// to the link being deleted.
					if (m_Tail == target)
					{
						m_Tail = tmp;
						m_Iterator = m_Head;
					}
					else if (m_Iterator == target)
					{
						m_Iterator = target->Next;
					}
					tmp->Next = target->Next;
					if (delete_data)
					{
						delete target->castedData;
					}
					free(target);
					return;
				}
				else
				{
					tmp = tmp->Next;
				}
			}
		}
	};

	T* Head()
	{
		m_Iterator = m_Head;
		if (m_Head == nullptr)
			return nullptr;
		return m_Head->linkData;
	}

	T* GetTail()
	{
		if (m_Tail == nullptr)
			return nullptr;
		return m_Tail->linkData;
	}

	int32_t Size()
	{
		return m_Size;
	}
	T* Read()
	{
		return m_Iterator->linkData;
	}
	T* PeekNext()
	{
		if (m_Iterator->Next == nullptr)
			return nullptr;
		else
			return m_Iterator->Next->linkData;
	}
	T* ReadAndNext()
	{
		FLink<T>* tmp = m_Iterator;
		if (tmp == nullptr)
			return nullptr;
		m_Iterator = m_Iterator->Next;
		return tmp->linkData;
	}
	T* Next()
	{
		if (m_Iterator == nullptr || m_Iterator == m_Tail)
			return nullptr;
		m_Iterator = m_Iterator->Next;
		return m_Iterator->linkData;
	}
	T* Get(int32_t index)
	{
		FLink<T>* tmp = m_Head;
		if (tmp == nullptr)
		{
			return nullptr;
		}
		while (index && tmp->Next)
		{
			tmp = tmp->Next;
			index--;
		}
		if (index != 0)
		{
			return nullptr;
		}
		else
		{
			return tmp->linkData;
		}
	}
	void RemoveCurrentItem()
	{
		Remove(m_Iterator->linkData);
	}

	inline void Clear(BOOLEAN delete_data = false)
	{
		while (m_Size > 0)
			Remove(m_Head->linkData, delete_data);
	}
};

template <class T>
class FListIterator
{
protected:
	FLinkedList<T>* myList;
	FLink<T>* current;

public:
	FListIterator<T>(FLinkedList<T>* list)
	{
		myList = list;
		current = myList->m_Head;
	}

	FListIterator<T>()
	{
		myList = nullptr;
		current = nullptr;
	}

	void Init(FLinkedList<T>* list)
	{
		myList = list;
		current = myList->m_Head;
	}

public:
	T* Next()
	{
		gosASSERT(current != nullptr);
		current = current->GetNext(void);
		if (current == nullptr)
			return nullptr;
		else
			return current->linkData;
	}

	void Rewind()
	{
		current = myList->m_Head;
	}

	T* Read()
	{
		if (current)
			return current->linkData;
		else
			return nullptr;
	}
};
