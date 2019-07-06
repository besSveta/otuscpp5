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
using rowType = std::map<position, ValueWrapper<T, defaultValue>>;
// Обертка для значения матрицы, чтобы задать оперции индексирования, присваивания
//хранит значение, ссылку(адрес) на список всех значений матрицы, позицию в матрице.
template<typename T, T defaultValue>
class ValueWrapper {

	void RemoveFromContainer() {
		position p { row, column };

		if ((*rows).find(p) != (*rows).end()) {
			(*rows).erase(p);
		}
	}

	friend Matrix<T, defaultValue> ;
	friend matr_iterator<T, defaultValue> ;
	ValueWrapper() = delete;
	ValueWrapper(const std::shared_ptr<rowType<T, defaultValue>> &rowsRef) :
			rows(rowsRef) {
		value = defaultValue;
		row = -1;
		column = -1;
	}

	void SetValue(T&& other) {
		if (this->value != other) {
			if (other != defaultValue) {
				value = other;
				position p { row, column };

				auto it = (*rows).find(p);
				if (it == (*rows).end()) {
					(*rows).insert(
							std::pair<position, ValueWrapper<T, defaultValue>>(
									p, *this));
				} else {
					(*it).second.value = other;
				}
			} else {
				RemoveFromContainer();
			}
		}

	}
public:
	// приведение к типу tuple.
	operator std::tuple<int, int, T>() {

		return std::make_tuple(this->row, this->column, this->value);
	}

// присваивание, если присваиваем значение по умолчание, то удаляем из контейнера,
//иначе добавляем или меняем уже имеющееся.
	ValueWrapper<T, defaultValue> & operator=(const T &other) {
		// похоже на костыль, но иначе получается копирование кода.. не знаю как тут обыграть.
		auto a = other;
		SetValue(std::forward<T>(a));
		return *this;
	}

	ValueWrapper<T, defaultValue> & operator=(T&& other) noexcept {

		SetValue(std::forward<T>(other));
		return *this;
	}

// Приведение  к типу T (тип значения матрицы)
	operator T() const {
		return value;
	}
// получение по индексу, проверяем сначала строки, потом столбцы, если есть такие позиции,
	//возвращаем значение из матрицы, если  нет, то возвращаем новый ValueWrapper.
	ValueWrapper<T, defaultValue> &operator[](int i) {
		if (i < 0) {
			throw "Incorrect Index";

		}

		if (row >= 0 && column >= 0) {
			position p { row, column };
			if ((*rows).find(p) != (*rows).end()) {
				return *this;
			}
		}

		if (this->row == -1) {
			this->row = i;

		}

		if (this->column == -1) {
			this->column = i;
			position p { row, column };
			if ((*rows).begin() != (*rows).end()) {
				auto it = (*rows).find(p);
				if ((*rows).find(p) != (*rows).end()) {
					return (*it).second;
				}
			}

		}

		if (this->value != defaultValue) {
			this->value = defaultValue;
		}
		//throw std::exception("incorrect operation!");
		return *this;
	}
	// Если матрица константная
	const ValueWrapper<T, defaultValue> operator[](int i) const {
		if (i < 0) {
			throw "Incorrect Index";

		}
		if (row >= 0 && column >= 0) {
			position p { row, column };
			if ((*rows).find(p) != (*rows).end()) {
				return *this;
			}
		}

		return ValueWrapper(this->rows);
	}

private:
	T value;
	std::shared_ptr<rowType<T, defaultValue>> rows;
	int row;
	int column;
};
template<typename T, T defaultValue>
bool operator <(const ValueWrapper<T, defaultValue> & lhs,
		const ValueWrapper<T, defaultValue> & rhs) {
	return lhs.value < rhs.value;
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
	matr_iterator(const std::shared_ptr<rowType<T, defaultValue>> &rw,
			position pos) :
			rowsPtr(rw), lastPosition(pos) {

	}
	matr_iterator& operator++() {
		auto it = (*rowsPtr).find(lastPosition);
		if (it != (*rowsPtr).end()) {
			it = std::next(it);
			if (it != (*rowsPtr).end()) {
				lastPosition = it->first;
			} else
				lastPosition = {-1,-1};
			}
		return *this;
	}
	bool operator==(matr_iterator j) const {
		auto t = (rowsPtr == j.rowsPtr
				&& lastPosition.column == j.lastPosition.column
				&& lastPosition.row == j.lastPosition.row);
		return t;
	}
	bool operator!=(matr_iterator j) const {
		return !(*this == j);
	}

	std::tuple<int, int, T> GetValue() const {
		auto rows = *rowsPtr;
		auto it = rows.find(lastPosition);
		std::tuple<int, int, T> currentValue;
		if (it == rows.end()) {
			currentValue = std::tuple<int, int, T>(-1, -1, defaultValue);
		} else {
			currentValue = std::tuple<int, int, T>(it->second.row,
					it->second.column, it->second.value);
		}
		return currentValue;
	}

	std::tuple<int, int, T> operator*() const {
		return GetValue();
	}

private:
	std::shared_ptr<rowType<T, defaultValue>> rowsPtr;
	position lastPosition;
};

// разреженная матрица.
template<typename T, T defaultValue>
class Matrix {

// схранит значения матрицы, ключем является прзиция.
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
		return (*rowsPtr).size();
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
		if ((*rowsPtr).begin() != (*rowsPtr).end()) {
			auto val = *(*rowsPtr).begin();
			return matr_iterator<T, defaultValue>(rowsPtr, val.first);
		}
		return matr_iterator<T, defaultValue>(rowsPtr, fakePosition);
	}

	matr_iterator<T, defaultValue> end() const {

		if ((*rowsPtr).begin() != (*rowsPtr).end()) {
			return matr_iterator<T, defaultValue>(rowsPtr,
					(*(*rowsPtr).rbegin()).first);
		}

		return matr_iterator<T, defaultValue>(rowsPtr, fakePosition);
	}

	~Matrix() {
		(*rowsPtr).clear();
		tempValue.rows = nullptr;
	}

};
