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

template<typename T, T defaultValue>
struct ColumnWrapper;
template<typename T, T defaultValue>
struct Matrix;

struct position
{
	int row;
	int column;
};
template<typename T, T defaultValue>
struct ValueWrapper {
	using rowType = std::map<position, ValueWrapper<T, defaultValue>>;
public:

	void RemoveFromContainer() {
		position p{ row, column };
		if (rows.find(p) != rows.end()) {
			rows.erase(p);
		}
	}

	friend Matrix<T, defaultValue>;
	ValueWrapper() = delete;
	ValueWrapper(rowType &rowsRef) : rows{ rowsRef } {
		value = defaultValue;
		row = -1;
		column = -1;
	}
	ValueWrapper(T val, const rowType &rowsRef, int rw, int col) :
		value(val), rows(rowsRef), row(rw), column(col) {

	}
	ValueWrapper(const rowType &rowsRef, int rw, int col) :
		value(defaultValue), rows(rowsRef), row(rw), column(col) {

	}

	void SetValue(const T &val) {
		value = val;
	}

	T value;
	rowType &rows;
	int row;
	int column;
	//ValueWrapper<T, defaultValue> & operator=(const ValueWrapper<T, defaultValue>& other)= delete;
	//ValueWrapper<T, defaultValue> & operator=(ValueWrapper<T, defaultValue>&& other)=delete;

	friend Matrix<T, defaultValue>;
	operator std::tuple<int, int, T>() { // copy assignment

		return std::make_tuple(this->row, this->column, this->value);
	}



	ValueWrapper<T, defaultValue> & operator=(const T& other) { // copy assignment

		if (this->value != other) {
			if (other!= defaultValue) {
				value = other;
				position p{ row, column };
				auto it = rows.find(p);
				if (it == rows.end()) {
					rows.insert(std::pair<position, ValueWrapper<T, defaultValue>>(p, *this));
					return *this;
				}
				else {
					(*it).second.SetValue(other);
					return  (*it).second;
				}
			}
			else {
				RemoveFromContainer();
			}
		}
		return *this;
	}

	ValueWrapper<T, defaultValue> & operator=(T&& other) noexcept { // move assignment

		if (this->value != other) {
			if (other != defaultValue) {
				value = std::move(other);
				position p{ row, column };
				auto it = rows.find(p);
				if (it == rows.end()) {
					rows.insert(std::pair<position, ValueWrapper<T, defaultValue>>(p, *this));
					return *this;
				}
				else {
					(*it).second.SetValue(other);
					return  *it;
				}
			}
			else {
				RemoveFromContainer();

			}
		}
		return *this;
	}


	operator T() const {
		return value;
	}

	ValueWrapper<T, defaultValue> &operator[](int i) {
		if (row >= 0 && column >= 0) {
					position p{ row, column };
					if (rows.find(p) != rows.end()) {
						return *this;
					}
				}

				if (this->row == -1) {
					this->row = i;

				}

				if (this->column == -1) {
					this->column = i;
					position p{ row, column };
					auto it = rows.find(p);
					if (rows.find(p) != rows.end()) {
						return (*it).second;
					}
				}

				if (this->value != defaultValue) {
					this->value = defaultValue;
				}
				//throw std::exception("incorrect operation!");
				return *this;
	}
	const ValueWrapper<T, defaultValue> &operator[](int i) const {
		if (row >= 0 && column >= 0) {
					position p{ row, column };
					if (rows.find(p) != rows.end()) {
						return *this;
					}
				}

				if (this->row == -1) {
					this->row = i;

				}

				if (this->column == -1) {
					this->column = i;
					position p{ row, column };
					auto it = rows.find(p);
					if (rows.find(p) != rows.end()) {
						return (*it).second;
					}
				}

				if (this->value != defaultValue) {
					this->value = defaultValue;
				}
				//throw std::exception("incorrect operation!");
				return *this;
	}
};
template<typename T, T defaultValue>
bool operator <  (const ValueWrapper<T, defaultValue> & lhs, const ValueWrapper<T, defaultValue> & rhs)
{
	return lhs.value < rhs.value;
}


bool operator <  (const position lhs, const  position rhs)
{
	if (lhs.row == rhs.row) {
		return lhs.column < rhs.column;
	}
	else {
		return lhs.row < rhs.row;
	}
}
template<typename T, T defaultValue>
struct map_iterator : public std::iterator<std::bidirectional_iterator_tag, std::tuple<int, int, T>>
{
public:
	using rowType = std::map<position, ValueWrapper<T, defaultValue>>;
	map_iterator() = delete;
	map_iterator(rowType &rw, position pos) : rows(rw), lastPosition(pos)
	{
		currentValue = GetValue();

	}
	map_iterator& operator++() {
		auto it = rows.find(lastPosition);
		if (it != rows.end()) {
			it=std::next(it);
			if (it != rows.end()) {
				lastPosition = it->first;
			}
			else lastPosition = { -1,-1 };
		}
		return *this;
	}
	bool operator==(map_iterator j) const
	{
		auto t = (&rows == &j.rows && lastPosition.column == j.lastPosition.column && lastPosition.row == j.lastPosition.row);
		return t;
	}
	bool operator!=(map_iterator j) const { return !(*this == j); }

	decltype(auto) GetValue() {
		auto it = rows.find(lastPosition);
		if (it == rows.end()) {
			currentValue = std::tuple<int, int, T>(-1, -1, defaultValue);
		}
		else {
			currentValue = std::tuple<int, int, T>(it->second.row, it->second.column, it->second.value);
		}
		return currentValue;
	}

	typename map_iterator::reference operator*()
	{
		currentValue = GetValue();
		return currentValue;
	}
	typename map_iterator::pointer operator->() {
		currentValue = GetValue();
		return &currentValue;
	}

private:
	rowType &rows;
	position lastPosition;
	std::tuple<int, int, T> currentValue;
};


template<typename T, T defaultValue>
class Matrix {
	using rowType = std::map<position, ValueWrapper<T, defaultValue>>;
	rowType rows;
	ValueWrapper<T, defaultValue> tempValue;
public:
	Matrix() :
		rows{ }, tempValue(rows) {
	}

	std::size_t size() {
		return rows.size();
	}

	ValueWrapper<T, defaultValue> & operator[](int i) {
		tempValue.rows = rows;
		tempValue.column = -1;
		tempValue.row = i;
		return tempValue;
	}

	const ValueWrapper<T, defaultValue> &operator[](int i) const {
		tempValue.rows = rows;
		tempValue.column = -1;
		tempValue.row = i;
		return tempValue;
	}



	decltype(auto) begin() noexcept {
		if (rows.begin() != rows.end()) {
			auto val = *rows.begin();
		 return map_iterator<T, defaultValue>(rows,val.first);
		}
		return map_iterator<T, defaultValue>(rows, { -1, -1 });
	}

	decltype(auto) end() noexcept {
		return  map_iterator<T, defaultValue>(rows, { -1, -1 });
	}

};



