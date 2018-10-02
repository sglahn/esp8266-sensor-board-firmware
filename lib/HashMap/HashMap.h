/* 
||
|| @author         Alexander Brevig <abrevig@wiring.org.co>
|| @url            http://wiring.org.co/
|| @url            http://alexanderbrevig.com/
|| @contribution   https://github.com/alessandro1105
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
||
|| @description
|| | Implementation of a HashMap data structure.
|| | This version is heavily based upon a suggestion from Alessandro1105
|| | Thank you for your contribution!
|| |
|| | Wiring Cross-platform Library
|| #
||
|| @license Please see cores/Common/License.txt.
||
*/

#ifndef HASHMAP_H
#define HASHMAP_H

template<typename hash,typename map>
class HashType {
public:
  HashType() { reset(); }
  HashType(hash code, map value): hashCode(code), mappedValue(value) { }

  void reset() {
    hashCode = 0; mappedValue = 0;
  }

  hash getHash() const {
    return hashCode;
  }

  void setHash(hash code){
    hashCode = code;
  }

  map getValue() const {
    return mappedValue;
  }

  void setValue(map value){
    mappedValue = value;
  }

  HashType& operator()(hash code, map value){
    setHash(code);
    setValue(value);
  }
private:
  hash hashCode;
  map mappedValue;
};


template<typename hash, typename map>
class HashNode {
public:
  HashNode(hash code, map value) {
    hashType = new HashType<hash, map>(code, value);
    previus = 0;
    next = 0;
  }

  ~HashNode() {
    delete hashType;
  }

  HashType<hash, map> * getHashType() const {
    return hashType;
  }

  HashNode<hash, map> * getPrevius() const {
    return previus;
  }

  HashNode<hash, map> * getNext() const {
    return next;
  }

  void setPrevius(HashNode<hash, map> * previus) {
    this->previus = previus;
  }

  void setNext(HashNode<hash, map> * next) {
    this->next = next;
  }

private:
  HashType<hash, map> * hashType;
  HashNode * previus;
  HashNode * next;
};


template<typename hash,typename map>
class HashMap {
public:
  HashMap(){  
    start = 0;
    finish = 0;
    position = 0; 
    size = 0;
  }

  ~HashMap(){
    if (moveToFirst()) {
      HashNode<hash, map> * next = position;
      while (position) {
        next = position->getNext();
        delete position;
        position = next;
      }
    }
  }

  void put(hash key, map value) { 
    if (start == 0) {
      start = finish = new HashNode<hash, map>(key, value);
    } else {
      if (hashPairForKey(key)!=0) {
        remove(hashPairForKey(key));
      }
      HashNode<hash, map> * temp = new HashNode<hash, map>(key, value);
      finish->setNext(temp);
      temp->setPrevius(finish);
      finish = temp;
    }
    size++;
  }

  bool containsKey(hash key) const {
    return hashPairForKey(key) != 0;
  }

  map valueFor(hash key) {
    HashNode<hash, map> * pointer = hashPairForKey(key);
    if (pointer != 0) {
      return pointer->getHashType()->getValue();
    }
  }

  map valueAt(int i) {
    moveToFirst();
    int iter = 0;
    do {
      if (iter == i) {
        return position->getHashType()->getValue();
      }
      iter++;
    } while (moveToNext());
  }

  map value() {
    if (position != 0) {
      return position->getHashType()->getValue();
    }
  }

  hash keyFor(map value) {
    moveToFirst();
    do {
      if (position->getHashType()->getValue() == value) {
        return position->getHashType()->getHash();
      }
    } while (moveToNext());
  }

  hash keyAt(int i) {
    moveToFirst();
    int iter = 0;
    do {
      if (iter == i) {
        return position->getHashType()->getHash();
      }
      iter++;
    } while (moveToNext());
  }

  hash key() {
    if (position != 0) {
      return position->getHashType()->getHash();
    }
  }

  void remove(hash key) {
    HashNode<hash, map> * pointer = hashPairForKey(key);
    if (pointer != 0) {
      remove(pointer);
    }
  }

  void remove() {
    if (position != 0) {
      if (size == 1) {          
        remove(position);
        position = 0;
      } else {
        if (position == start) {
          remove(position);
          position = start;
        } else {
          remove(position);
          position = position->getPrevius();
        }
      }
    }
  }

  unsigned int count() const {return size; }

  bool moveToFirst() {
    if (start != 0) {
      position = start;
      return true;
    } else {
      return false;
    }
  }

  bool moveToLast() {
    if (finish != 0) {
      position = finish;
      return true;
    } else {
      return false;
    }
  }

  bool moveToNext() {
    if (position->getNext() != 0) {
      position = position->getNext();
      return true;
    } else {
      return false;
    }
  }

  bool moveToPrev() {
    if (position->getPrevius() != 0) {
      position = position->getPrevius();
      return true;
    } else {
      return false;
    }
  }

private:

  HashNode<hash, map> * hashPairForKey(hash key) {
    for(HashNode<hash, map> * pointer = start; pointer != 0; pointer = pointer->getNext()) {
      HashType<hash, map> * hashType = pointer->getHashType();
      if (key == hashType->getHash()) {
        return pointer;
      }
    }
    return 0;
  }

  void remove(HashNode<hash, map> * pointer) {
    if (pointer == 0) return;
    if (size == 1) {
      start = finish = 0;
    } else {
      if (pointer == start) {
        start = start->getNext();
        start->setPrevius(0);
      } else if (pointer == finish) {
        finish = finish->getPrevius();
        finish->setNext(0);
      } else {
        pointer->getPrevius()->setNext(pointer->getNext());
        pointer->getNext()->setPrevius(pointer->getPrevius());
      }
    }

    size--;
    delete pointer;
  }

  HashNode<hash, map> * start;
  HashNode<hash, map> * finish;
  HashNode<hash, map> * position;
  int size;
};
#endif
// HASHMAP_H