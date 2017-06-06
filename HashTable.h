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

class Exception {};
class TableEmpty               : Exception {};
class AlreadyInTableException  : Exception {};
class KeyNotInTableException   : Exception {};
class InvalidArg               : Exception {};
class AllocationError          : Exception {};

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
	int item_count;
	//The size of the table at a given moment.
	int current_table_size;

	class HashEntry {
	private:
		//Current HashEntry's key.
		int  entry_key;
		//Current HashEntry's data.
		Data entry_data;
		//The "next" field will help us whether we will be using regular hashing
		//with linked lists, or with double hashing by pointing to the next entry
		//afteer a jump.
		HashEntry* next;
	public:
		//Creates a new HashEntry with specified key, data, and next.
		HashEntry(int key, Data data, HashEntry* next) : entry_key(key), entry_data(data), next(next) { }
		//Destroys the current HashEntry;
		~HashEntry() { }
		//Returns the key of the current entry.
		int GetKey() { return this->entry_key; }
		//Sets the current key to the given value,
		void SetKey(int set) { this->entry_key = set; }
		//Returns the data of the current entry.
		Data GetData() { return this->entry_data; }
		//Sets the current data to the given value,
		void SetData(Data set) { this->entry_data = set; }
		//Returns the next HashEntry to the current one.
		HashEntry* GetNext() { return this->next; }
		//Sets the current next HashEntry to the given value,
		void SetNext(HashEntry* next) { this->next = next; }
	};
private:
	//The actual array of hash entries kept in the table.
	HashEntry** table_entries;

	int max_list_length;

	int HashFunc(int key) {
		return (key % this->current_table_size);
	}

	int ListSize(HashEntry* head) {
		HashEntry* iter = head;
		int cnt = 0;
		while(iter != nullptr) {
			cnt++;
			iter = iter->GetNext();
		}
		return cnt;
	}
	void ResetHashArray(HashEntry** arr, int size) {
		if(arr == nullptr || size == 0)
			return;
		for(int i = 0; i < size; i++)
			arr[i] = nullptr;
	}

	//Function to resize the hash-table. This function is called on every add,
	//But does not necessarily need to do anything if there weren't enough
	//collisions in the table.
	void ResizeHash() {
		if(this->max_list_length <= 10)
			return; //If expand was called, but not needed.
		int size = 0;
		this->max_list_length = 0;
		Pair<int, Data>** flushed_arr = this->FlushTable(&size);
		try {
			int next_size = this->current_table_size * this->RESIZE_FACTOR;
			HashEntry** resized = new HashEntry*[next_size]; //Allocate a new array
			ResetHashArray(resized, next_size);
			HashEntry** prev = this->table_entries;                          //Keep the previous array for deletion.
			this->table_entries = resized;                                  //Update the current table's array.
			this->current_table_size = next_size;
			this->item_count = 0;
			for(int i = 0; i < size; i++) {
				if(flushed_arr[i] != nullptr)
					this->Insert(flushed_arr[i]->GetKey(), flushed_arr[i]->GetValue());
			}
			delete[] prev;
			for(int i = 0; i < size; i++) {
				if(flushed_arr[i] != nullptr)
					delete flushed_arr[i];
			}
			delete[] flushed_arr;
		} catch(AllocationError& e) {
			delete flushed_arr;
			throw;
		} catch(TableEmpty& e) {
			throw; //Should never be called here, very bad error otherwise.
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
	void AddToHashList(int list_id, int key_add, Data data_add) {
		if(this->SearchHashList(this->table_entries[list_id], key_add) == true)
			throw AlreadyInTableException();
		if(this->table_entries[list_id] == nullptr) {
			this->table_entries[list_id] = new HashEntry(key_add, data_add, nullptr);
			return;
		}
		HashEntry* head = this->table_entries[list_id];
		HashEntry* add = new HashEntry(key_add, data_add, head);
		this->table_entries[list_id] = add;
		int len = ListSize(this->table_entries[list_id]);
		if(len > this->max_list_length)
			this->max_list_length = len;
	}

	//Returns TRUE if the key was found in the hash list, and FALSE otherwise.
	bool SearchHashList(HashEntry* start, int find) {
		if(start == nullptr)
			return false;
		HashEntry* iter = start;
		bool found = false;
		while(iter != nullptr) {
			if(find == iter->GetKey()) {
				found = true;
				break;
			}
			iter = iter->GetNext();
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
			return;
		}
		while(   prev                      != nullptr
			  && prev->GetNext()           != nullptr
			  && prev->GetNext()->GetKey() != key)    {
			prev = prev->GetNext();
		}
		//If not found in list.
		if(prev == nullptr || prev->GetNext() == nullptr)
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
	HashTable() : max_list_length(0) {
		this->current_table_size = this->START_SIZE;
		this->item_count = 0;
		this->table_entries = new HashEntry*[START_SIZE];
		for(int i = 0; i < START_SIZE; i++) {
			this->table_entries[i] = nullptr;
		}
	}
	~HashTable() {
		for(int i = 0; i < this->current_table_size; i++) {
			this->DeleteHashList(this->table_entries[i]);
		}
		delete[] this->table_entries;
	}
	int Size() {
		return this->item_count;
	}
	void Insert(int key, Data data) {
		try {
			int hash_id = this->HashFunc(key);
			this->AddToHashList(hash_id, key, data);
			this->item_count++;
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		} catch(AlreadyInTableException& e) {
			throw;
		}
		try {
			this->ResizeHash();
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		}
	}

	bool Find(int key) {
		HashEntry* list = this->table_entries[this->HashFunc(key)];
		return this->SearchHashList(list, key);
	}

	void Delete(int key) {
		if(key < 0)
			throw KeyNotInTableException();
		try {
			this->DeleteFromList(this->HashFunc(key), key);
			this->item_count--;
		} catch(KeyNotInTableException& e) {
			throw;
		}
		//No need to resize Hash on delete.
	}

	//Flushs all keys aand data from the table into an array of pairs,
	//in some unknown order. The table will be empty after the flush,
	//but will still exist.
	//NOTE: Table does not revert back to original size.
	Pair<int, Data>** FlushTable(int* size_out) {
		if(size_out == nullptr)
			throw InvalidArg();
		Pair<int, Data>** arr = nullptr;
		try {
			arr = new Pair<int, Data>*[this->item_count];
		} catch(std::bad_alloc& e) {
			throw AllocationError();
		}
		HashEntry* iter = nullptr;
		int out_iter = 0;
		for(int i = 0; i < this->current_table_size; i++) { //Delete table data
			iter = this->table_entries[i];
			while(iter != nullptr) {
				arr[out_iter] = new Pair<int, Data>(iter->GetKey(), iter->GetData());
				out_iter++;
				iter = iter->GetNext();
			}
			//Delete the list after iterating over it.
			this->DeleteHashList(this->table_entries[i]);
			this->table_entries[i] = nullptr;
		}
		this->item_count = 0;
		this->max_list_length = 0;
		*size_out = out_iter;
		return arr;
	}
};

#endif /* HASHTABLE_H_ */
