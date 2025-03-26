#pragma once

#include "DynamicArray.h"
#include "LinkedList.h"
#include "ICollection.h"

namespace PATypes {
    template<class T>
    class Sequence : public ICollection<T> {
    public:
        virtual T getFirst() = 0;
        virtual T getLast() = 0;
        virtual T get(int index) = 0;
        virtual int getLength() = 0;
        virtual Sequence<T> *getSubsequence(int startIndex, int endIndex) = 0;
        virtual Sequence<T> *append(T item) = 0;
        virtual Sequence<T> *insertAt(T item, int index) = 0;
        virtual Sequence<T> *concat(Sequence<T> *list) = 0;
        virtual Sequence<T> *map(T (*f)(T)) = 0;
        virtual T operator[](int index) = 0;
        Sequence<T>& operator+(Sequence<T> &sequence);
    };

    template<class T>
    Sequence<T> &Sequence<T>::operator+(Sequence<T> &sequence) {
        Sequence<T>& aaa = *concat(&sequence);
        return *(this->concat(&sequence));
    }

    template<class T>
    class ArraySequence : public Sequence<T> {
    public:
        ArraySequence(T *items, int count) : array(items, count) {}
        ArraySequence() : array(0) {};
        ArraySequence(const ArraySequence &arraySequence) : array(arraySequence.array) {};
        ArraySequence(Sequence<T> &sequence);
        virtual T getFirst();
        virtual T getLast();
        virtual T get(int index);
        virtual T get(size_t index);
        virtual size_t getCount();
        virtual int getLength();
        virtual Sequence<T> *getSubsequence(int startIndex, int endIndex) = 0;
        virtual Sequence<T> *append(T item);
        virtual Sequence<T> *insertAt(T item, int index);
        virtual Sequence<T> *concat(Sequence<T> *list);
        virtual Sequence<T> *map(T (*f)(T));
        T operator[](int index);
        ArraySequence<T>& operator=(ArraySequence<T> other);

    private:
        DynamicArray<T> array;

    protected:
        virtual ArraySequence<T> *Instance() = 0;
    };

    template<class T>
    T ArraySequence<T>::getFirst() {
        if (array.getSize() < 1)
            throw std::out_of_range("при попытке получения первого элемента ArraySequence пустой");
        return array[0];
    }

    template<class T>
    T ArraySequence<T>::getLast() {
        if (array.getSize() < 1)
            throw std::out_of_range("при попытке получения последнего элемента ArraySequence пустой");
        return array[array.getSize() - 1];
    }

    template<class T>
    T ArraySequence<T>::get(int index) {
        if (index < 0 || index >= array.getSize())
            throw std::out_of_range("при попытке получения элемента по индексу ArraySequence индекс за границами");
        return array[index];
    }

    template<class T>
    int ArraySequence<T>::getLength() {
        return array.getSize();
    }

    template<class T>
    Sequence<T> *ArraySequence<T>::append(T item) {
        ArraySequence<T> *current = Instance();
        current->array.resize(array.getSize() + 1);
        current->array.set(array.getSize() - 1, item);
        return current;
    }

    template<class T>
    Sequence<T> *ArraySequence<T>::insertAt(T item, int index) {
        if (index < 0 || index >= array.getSize())
            throw std::out_of_range("при попытке вставки элемента по индексу ArraySequence индекс за границами");
        ArraySequence<T> *current = Instance();
        current->array.resize(array.getSize() + 1);
        for (int i = array.getSize() - 1; i > index; --i) {
            current->array[i] = current->array[i - 1];
        }
        current->array[index] = item;
        return current;
    }

    template<class T>
    Sequence<T> *ArraySequence<T>::concat(Sequence<T> *list) {
        ArraySequence<T> *current = Instance();
        int previousLength = current->getLength();
        DynamicArray<int>* array = new DynamicArray<int>(previousLength + list->getLength(), this->array);
        current->array = *array;
        for (int i = 0; i < list->getLength(); ++i)
            current->array.set(i + previousLength, list->get(i));
        return current;
    }

    template<class T>
    T ArraySequence<T>::operator[](int index) {
        return this->array[index];
    }

    template<class T>
    T ArraySequence<T>::get(size_t index) {
        return get((int)index);
    }

    template<class T>
    size_t ArraySequence<T>::getCount() {
        return (size_t) array.getSize();
    }

    template<class T>
    Sequence<T> *ArraySequence<T>::map(T (*f)(T)) {
        ArraySequence<T> *current = Instance();
        for(int i = 0; i < current->getLength(); ++i)
            current->array.set(f(current->array.get(i)), i);
        return current;
    }
    template<class T>
    ArraySequence<T>::ArraySequence(Sequence<T> &sequence) : array(sequence.getLength()) {
        for (int i = 0; i < sequence.getLength(); ++i) {
            array.set(i, sequence.get(i));
        }
    }

    template<class T>
    ArraySequence<T> &ArraySequence<T>::operator=(ArraySequence<T> other) {
        std::swap(array, other.array);
        return *this;
    }

    template<class T>
    class ImmutableArraySequence : public ArraySequence<T> {
    public:
        ImmutableArraySequence(T *items, int count) : ArraySequence<T>(items, count){};
        ImmutableArraySequence() : ArraySequence<T>(){};
        ImmutableArraySequence(Sequence<T> &sequence) : ArraySequence<T>(sequence) {};
        virtual PATypes::Sequence<T> *getSubsequence(int startIndex, int endIndex);

    protected:
        virtual ArraySequence<T> *Instance();
    };

    template<class T>
    Sequence<T> *ImmutableArraySequence<T>::getSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || startIndex >= this->getLength() || endIndex < 0 || endIndex >= this->getLength())
            throw std::out_of_range("Индекс за границами при попытке получения подпоследовательности ImmutableArraySequence");
        ImmutableArraySequence<T> *current = new ImmutableArraySequence<T>(*this);
        for (int i = startIndex; i <= endIndex; ++i) {
            current->append(this->get(i));
        }
        return current;
    }

    template<class T>
    ArraySequence<T> *ImmutableArraySequence<T>::Instance() {
        return new ImmutableArraySequence(*this);
    }

    template<class T>
    class MutableArraySequence : public ArraySequence<T> {
    public:
        MutableArraySequence(T *items, int count) : ArraySequence<T>(items, count){};
        MutableArraySequence() : ArraySequence<T>(){};
        MutableArraySequence(Sequence<T> &sequence) : ArraySequence<T>(sequence) {};
        virtual PATypes::Sequence<T> *getSubsequence(int startIndex, int endIndex);

    protected:
        virtual ArraySequence<T> *Instance();
    };

    template<class T>
    Sequence<T> *MutableArraySequence<T>::getSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || startIndex >= this->getLength() || endIndex < 0 || endIndex >= this->getLength())
            throw std::out_of_range("Индекс за границами при попытке получения подпоследовательности MutableArraySequence");
        MutableArraySequence<T> *current = new MutableArraySequence<T>(*this);
        for (int i = startIndex; i <= endIndex; ++i) {
            current->append(this->get(i));
        }
        return current;
    }

    template<class T>
    ArraySequence<T> *MutableArraySequence<T>::Instance() {
        return this;
    }

    template<class T>
    class ListSequence : public Sequence<T> {
    public:
        ListSequence(T *items, int count) : list(items, count) {};
        ListSequence() : list() {};
        ListSequence(ListSequence<T>& listSequence) : list(listSequence.list) {};
        ListSequence(Sequence<T>& sequence);
        T getFirst();
        T getLast();
        T get(int index);
        int getLength();
        virtual T get(size_t index);
        virtual size_t getCount();
        virtual Sequence<T> *getSubsequence(int startIndex, int endIndex) = 0;
        virtual Sequence<T> *append(T item);
        virtual Sequence<T> *insertAt(T item, int index);
        virtual Sequence<T> *concat(Sequence<T> *list);
        virtual Sequence<T> *map(T (*f)(T));
        T operator[](int index);
    private:
        LinkedList<T> list;
    protected:
        virtual ListSequence<T> *Instance() = 0;
    };

    template<class T>
    T ListSequence<T>::getFirst() {
        return list.getFirst();
    }
    template<class T>
    T ListSequence<T>::getLast() {
        return list.getLast();
    }
    template<class T>
    T ListSequence<T>::get(int index) {
        return list.get(index);
    }
    template<class T>
    Sequence<T> *ListSequence<T>::append(T item) {
        ListSequence<T> *current = Instance();
        current->list.append(item);
        return current;
    }

    template<class T>
    int ListSequence<T>::getLength() {
        return list.getLength();
    }

    template<class T>
    Sequence<T> *ListSequence<T>::insertAt(T item, int index) {
        if (index < 0 || index >= list.getLength())
            throw std::out_of_range("при попытке вставить в ListSequence индекс за границами");
        ListSequence<T> *current = Instance();
        try {
            return current->insertAt(index, item);
        } catch (std::out_of_range&) {
            throw std::out_of_range("при попытке вставить в ListSequence индекс за границами");
        }
        return current;
    }

    template<class T>
    Sequence<T> *ListSequence<T>::concat(Sequence<T> *list) {
        ListSequence<T> *current = Instance();
        for (int i = 0; i < list->getLength(); ++i) {
            current->append(list->get(i));
        }
        return current;
    }

    template<class T>
    T ListSequence<T>::operator[](int index) {
        return get(index);
    }

    template<class T>
    T ListSequence<T>::get(size_t index) {
        return get((int) index);
    }

    template<class T>
    size_t ListSequence<T>::getCount() {
        return (size_t) getLength();
    }

    template<class T>
    Sequence<T> *ListSequence<T>::map(T (*f)(T)) {
        ListSequence<T> *current = Instance();
        current->list.map(f);
        return current;
    }
    template<class T>
    ListSequence<T>::ListSequence(Sequence<T> &sequence) : list() {
        for (int i = 0; i < sequence.getLength(); ++i) {
            list.append(sequence.get(i));
        }
    }

    template<class T>
    class ImmutableListSequence : public ListSequence<T> {
    public:
        ImmutableListSequence(T *items, int count) : ListSequence<T>(items, count){};
        ImmutableListSequence() : ListSequence<T>(){};
        virtual PATypes::Sequence<T> *getSubsequence(int startIndex, int endIndex);

    protected:
        virtual ListSequence<T> *Instance();
    };

    template<class T>
    ListSequence<T> *ImmutableListSequence<T>::Instance() {
        return new ImmutableListSequence(*this);
    }

    template<class T>
    Sequence<T> *ImmutableListSequence<T>::getSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || startIndex >= this->getLength() || endIndex < 0 || endIndex >= this->getLength())
            throw std::out_of_range("Индекс за границами при попытке получения подпоследовательности ImmutableListSequence");
        ImmutableListSequence<T> *current = new ImmutableListSequence<T>(*this);
        for (int i = startIndex; i <= endIndex; ++i) {
            current->append(this->get(i));
        }
        return current;
    }

    template<class T>
    class MutableListSequence : public ListSequence<T> {
    public:
        MutableListSequence(T *items, int count) : ListSequence<T>(items, count){};
        MutableListSequence() : ListSequence<T>(){};
        virtual PATypes::Sequence<T> *getSubsequence(int startIndex, int endIndex);

    protected:
        virtual ListSequence<T> *Instance();
    };

    template<class T>
    ListSequence<T> *MutableListSequence<T>::Instance() {
        return this;
    }

    template<class T>
    Sequence<T> *MutableListSequence<T>::getSubsequence(int startIndex, int endIndex) {
        if (startIndex < 0 || startIndex >= this->getLength() || endIndex < 0 || endIndex >= this->getLength())
            throw std::out_of_range("Индекс за границами при попытке получения подпоследовательности MutableListSequence");
        MutableListSequence<T> *current = new MutableListSequence<T>(*this);
        for (int i = startIndex; i <= endIndex; ++i) {
            current->append(this->get(i));
        }
        return current;
    }

};// namespace PATypes