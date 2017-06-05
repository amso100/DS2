/*
 * HashTable.h
 *
 *  Created on: Jun 5, 2017
 *      Author: amit
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

/*
 * Class for a hash-table implemented with regular hash.
 */

#include <new>
#include "KVPair.h"

template<typename Data> class HashTable {
private:
	//Hashtable will be initialied with this size.
	//The number is a large prime number that
	const int START_SIZE = 128;
//	//In case we will be using double  hashing.
//	const int JUMP_SIZE  = 7;
	//The factor by which we will multiply/divide the table size when resizing.
	const int RESIZE_FACTOR = 2;
	//The amount of cells in the hash currently being actually used.
	//We will use this to determine when we need to resize the table.
	int used_cells;
	//The size of the table at a given moment.
	int current_table_size;

	class HashEntry {
	private:
		//Current HashEntry's key.
		int  entry_key;
		//Current HashEntry's data.
		Data* entry_data;
		//The "next" field will help us whether we will be using regular hashing
		//with linked lists, or with double hashing by pointing to the next entry
		//afteer a jump.
		HashEntry* next;
	public:
		//Creates a new HashEntry with default values.
		HashEntry() : entry_key(nullptr), entry_data(nullptr), next(nullptr) { }
		//Creates a new HashEntry with specified key, data, and next.
		HashEntry(int key, Data* data, HashEntry* next) : entry_key(key), entry_data(data), next(next) { }
		//Destroys the current HashEntry;
		~HashEntry() { }
		//Returns the key of the current entry.
		int GetKey() { return this->entry_key; }
		//Sets the current key to the given value,
		void SetKey(int set) { if(set != nullptr) this->entry_key = set; }
		//Returns the data of the current entry.
		Data* GetData() { return this->entry_data; }
		//Sets the current data to the given value,
		void SetData(Data* set) { this->entry_data = set; }
		//Returns the next HashEntry to the current one.
		HashEntry* GetNext() { return this->next; }
		//Sets the current next HashEntry to the given value,
		void SetNext(HashEntry* next) { this->next = next; }
	};
private:
	//The actual array of hash entries kept in the table.
	HashEntry* table_entries;

	int HashFunc(int key) {
		return (key % this->current_table_size);
	}



	//Function to resize the hash-table. This function is called on every add/remove,
	//but does not necessarily do anything if there is no resize needed.
	void ResizeHash(bool expand) {
		if((this->used_cells >= this->current_table_size/2 && !expand)
		|| (this->used_cells <= this->current_table_size/2 && expand))
			return; //If expand/decrease was called, but not needed.
		if(expand) {

		}

		else {

		}
	}

	//Deletes a list of hash entries, given its start.
	void DeleteHashList(HashEntry* start) {
		if(start == nullptr)
			return;
		HashEntry* next = start->GetNext();
		HashEntry* prev = start;
		while(next != nullptr) {
			delete prev;
			prev = next;
			next = next->GetNext();
		}
		//Delete the last node.
		delete prev;
	}

	//Adds a given key and data to the list of HashEntry's kept in the
	//specified location in the table.
	void AddToHashList(int list_id, int key_add, Data* data_add) {
		if(this->SearchHashList(this->table_entries[list_id], key_add) == true)
			throw AlreadyInTableException();
		if(this->table_entries[list_id] == nullptr) {
			this->table_entries[list_id] = new HashEntry(key_add, data_add);
			return;
		}
		HashEntry* head = this->table_entries[list_id];
		HashEntry* add = new HashEntry(key_add, data_add, head);
		this->table_entries[list_id] = add;
	}

	//Returns TRUE if the key was found in the hash list, and FALSE otherwise.
	bool SearchHashList(HashEntry* start, int find) {
		HashEntry* iter = start;
		bool found = false;
		while(iter != nullptr) {
			if(find == iter->GetKey()) {
				found = true;
				break;
			}
		}
		return found;
	}

	//Deletes a given key from the list in the specified hash location.
	void DeleteFromList(int list_id, int key) {
		HashEntry* prev = this->table_entries[list_id];
		if(prev == nullptr)
			throw KeyNotInTableException();
		if(prev->GetKey() == key) {
			this->table_entries[list_id] = prev->GetNext();
			delete prev;
			this->table_entries[list_id] = nullptr;
			return;
		}
		while(prev != nullptr && prev->GetNext()->GetKey() != key) {
			prev = prev->GetNext();
		}
		//If not found in list.
		if(prev == nullptr)
			throw KeyNotInTableException();
		//If we did find it in the list...
		//Save it for deletion.
		HashEntry* to_delete = prev->GetNext();
		//Set prev's next ot be the next node after it, instead of the deleted one.
		prev->SetNext(to_delete->GetNext());
		//Delete the node.
		delete to_delete;
	}
public:
	HashTable() {
		this->current_table_size = this->START_SIZE;
		this->used_cells = 0;
		this->table_entries = new HashEntry[START_SIZE];
	}
	~HashTable() {
		//Finish code.
	}
	int Size() {
		return this->used_cells;
	}
	void Insert(int key, Data data) {
		try {
			int hash_id = this->HashFunc(key);
			this->AddToHashList(hash_id, key, data);
			this->used_cells++;
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		} catch(AlreadyInTableException& e) {
			throw;
		}
		try {
			this->ResizeHash(true);
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		}
	}

	bool Find(int key) {
		HashEntry* list = this->table_entries[this->HashFunc(key)];
		return this->SearchHashList(list, key);
	}

	void Delete(int key) {
		try {
			this->DeleteFromList(this->HashFunc(key), key);
			this->used_cells--;
		} catch(KeyNotInTableException& e) {
			throw;
		}
		try {
			this->ResizeHash(true);
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		}
	}

	//Flushs all keys aand data from the table into an array of pairs,
	//in some unknown order. The table will be empty after the flush,
	//but will still exist.
	Pair<int, Data>* FlushTable(int* size_out) {
		if(size_out == nullptr)
			throw InvalidArg();
		Pair<int, Data>* arr = nullptr;
		try {
			arr = new Pair<int, Data>[this->used_cells];
		} catch(std::bad_alloc) {
			throw AllocationError();
		}

	}
};

class Exception {};
class AlreadyInTableException : Exception {};
class KeyNotInTableException : Exception {};
class InvalidArg : Exception {};
class AllocationError : Exception {};

#endif /* HASHTABLE_H_ */
