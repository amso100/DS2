/*
 * KVPair.h
 *
 *  Created on: Jun 5, 2017
 *      Author: amit
 */

#ifndef KVPAIR_H_
#define KVPAIR_H_

/*
 * A class for a pair of key and data.
 */

template<typename Key, typename Value> class Pair {
private:
	Key key;
	Value value;
public:
	Pair(Key key, Value value) : key(key), value(value) { }
	~Pair() { }
	Key GetKey() { return this->key; }
	Value GetValue() { return this->value; }
	void SetKey(Key key) { this->key = key; }
	void SetValue(Value val) { this->value = val; }
};

#endif /* KVPAIR_H_ */
