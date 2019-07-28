/*
 * matrix_lib.h
 *
 *  Created on: 24 июн. 2019 г.
 *      Author: sveta
 */
#pragma once
#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <iterator>
#include <list>
#include <memory>

template<typename T, T defaultValue>
struct ColumnWrapper;
template<typename T, T defaultValue>
struct Matrix;
template<typename T, T defaultValue>
struct matr_iterator;
template<typename T, T defaultValue>
class ValueWrapper;

// Для запоминания строки и столбца.
struct position {
	int row;
	int column;
};
template<typename T, T defaultValue>
using rowType = std::map<position, T>;
// Обертка для значения матрицы, чтобы задать оперции индексирования, присваивания
//хранит значение, ссылку(адрес) на список всех значений матрицы, позицию в матрице.
template<typename T, T defaultValue>
class ValueWrapper {

	void RemoveFromContainer() {
		std::shared_ptr<rowType<T, defaultValue>> sharedRows = rows.lock();
		if (sharedRows == nullptr)
			return;
		position p { row, column };
		if (sharedRows->find(p) != sharedRows->end()) {
			sharedRows->erase(p);
		}
	}

	friend Matrix<T, defaultValue> ;
	friend matr_iterator<T, defaultValue> ;
	ValueWrapper() = delete;

	ValueWrapper(std::shared_ptr<rowType<T, defaultValue>> rowsRef) :
			rows(rowsRef) {
		row = -1;
		column = -1;
	}
	T GetValue() const {
		position p { row, column };
		std::shared_ptr<rowType<T, defaultValue>> sharedRows(rows);
		auto it = sharedRows->find(p);
		if (it == sharedRows->end()) {
			return defaultValue;
		} else {
			return it->second;
		}

	}

public:
	// приведение к типу tuple.
	operator std::tuple<int, int, T>() {
		return std::make_tuple(this->row, this->column, GetValue());
	}

// присваивание, если присваиваем значение по умолчание, то удаляем из контейнера,
//иначе добавляем или меняем уже имеющееся.
	ValueWrapper<T, defaultValue> & operator=(const T &other) {
		if (!(row >= 0 && column >= 0)) {
			throw "Incorrect operation";

		}
		if (other != defaultValue) {
			position p { row, column };
			std::shared_ptr<rowType<T, defaultValue>> sharedRows(rows);
			auto it = sharedRows->find(p);
			if (it == sharedRows->end()) {
				sharedRows->insert(std::pair<position, T>(p, other));
			} else {
				it->second = other;
			}
		} else {
			RemoveFromContainer();
		}
		return *this;
	}

// Приведение  к типу T (тип значения матрицы)
	operator T() const {
		auto value = GetValue();
		return value;
	}
// запоминаем индекс для получения в дальнейшем значений
	ValueWrapper<T, defaultValue> &operator[](int i) {
		if (i < 0) {
			throw "Incorrect Index";

		}

		if (this->row == -1) {
			this->row = i;
			return *this;
		}

		if (this->column == -1) {
			this->column = i;
		}

		return *this;
	}

private:
	std::weak_ptr<rowType<T, defaultValue>> rows;
	int row;
	int column;
};
template<typename T, T defaultValue>
bool operator <(const ValueWrapper<T, defaultValue> & lhs,
		const ValueWrapper<T, defaultValue> & rhs) {

	return (T) lhs < (T) rhs;
}

bool operator <(const position lhs, const position rhs) {
	if (lhs.row == rhs.row) {
		return lhs.column < rhs.column;
	} else {
		return lhs.row < rhs.row;
	}
}

// итератор для матрицы.
template<typename T, T defaultValue>
struct matr_iterator {
public:
	using value_type = T;
	using reference = T &;
	using iterator_category = std::bidirectional_iterator_tag;
	matr_iterator() = delete;
	matr_iterator(const std::weak_ptr<rowType<T, defaultValue>> &rw,
			typename std::map<position, T>::iterator pos) :
			rowsPtr(rw), lastPosition(pos) {

	}
	matr_iterator& operator++() {
		std::shared_ptr<rowType<T, defaultValue>> sharedRows(rowsPtr);
		lastPosition++;
		return *this;
	}
	bool operator==(matr_iterator j) const {
		std::shared_ptr<rowType<T, defaultValue>> sharedRows = rowsPtr.lock();
		std::shared_ptr<rowType<T, defaultValue>> otherRows = j.rowsPtr.lock();
		if (sharedRows == nullptr) {
			if (otherRows == nullptr)
				return true;
			else
				return false;
		}
		if (otherRows == nullptr)
			return false;

		return (sharedRows == otherRows && lastPosition == j.lastPosition);
	}
	bool operator!=(matr_iterator j) const {
		return !(*this == j);
	}

	std::tuple<int, int, T> GetValue() const {
		std::shared_ptr<rowType<T, defaultValue>> sharedRows(rowsPtr);
		std::tuple<int, int, T> currentValue;
		if (lastPosition == sharedRows->end()) {
			currentValue = std::tuple<int, int, T>(-1, -1, defaultValue);
		} else {
			currentValue = std::tuple<int, int, T>(lastPosition->first.row,
					lastPosition->first.column, lastPosition->second);
		}
		return currentValue;
	}

	std::tuple<int, int, T> operator*() const {
		return GetValue();
	}

private:
	std::weak_ptr<rowType<T, defaultValue>> rowsPtr;
	typename std::map<position, T>::iterator lastPosition;
};

// разреженная матрица.
template<typename T, T defaultValue>
class Matrix {

// схранит значения матрицы, ключем является позиция.
	std::shared_ptr<rowType<T, defaultValue>> rowsPtr;
	// обертка для значений.
	ValueWrapper<T, defaultValue> tempValue;
	position fakePosition;

public:

	Matrix() :
			rowsPtr(std::make_shared<rowType<T, defaultValue>>()), tempValue(
					rowsPtr) {
		fakePosition = position { -1, -1 };
	}

	std::size_t size() const {
		if (rowsPtr == nullptr) {
			return 0;
		}
		return rowsPtr->size();
	}

	ValueWrapper<T, defaultValue> & operator[](int i) {
		if (i < 0) {
			throw "Incorrect Index";

		}
		tempValue.column = -1;
		tempValue.row = i;
		return tempValue;
	}

	//когда матрица константная
	const ValueWrapper<T, defaultValue> operator[](int i) const {
		if (i < 0) {
			throw "Incorrect Index";

		}
		ValueWrapper<T, defaultValue> val(rowsPtr);
		val.column = -1;
		val.row = i;
		return val;
	}

	matr_iterator<T, defaultValue> begin() const {
		return matr_iterator<T, defaultValue>(rowsPtr, rowsPtr->begin());
	}

	matr_iterator<T, defaultValue> end() const {
		return matr_iterator<T, defaultValue>(rowsPtr, rowsPtr->end());
	}

	Matrix(const Matrix<T, defaultValue>& other) :
			rowsPtr(other.rowsPtr), tempValue(other.tempValue) {
	}
};
