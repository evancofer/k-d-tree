/*
 * KDTree.h
 *
 * Created on: Nov 26, 2015
 *
 * Copyright 2015, Evan Cofer
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See <http://www.gnu.org/licenses/>.
 */


#ifndef KDTREE_H_
#define KDTREE_H_

#include <utility>
#include <vector>
#include <cmath>

/*
The type T should have operator[] and it should be valid to index them
with values in the range of 0 to D-1.
*/
template <typename T,int D>

class KDTree {
	unsigned int PART_SIZE = 7;
	struct Plane {
		Plane():left(nullptr),right(nullptr),dim(-1), split(0), data() {}
		Plane *left; //Points less than or equal to split
		Plane *right; //Points greater than split
		int dim;
		double split;
		bool leaf = false;
		std::vector<T> data;//So I assume this will be where we split?
	};

	Plane* root = nullptr;

public:

	KDTree(std::vector<T> points){// Not a reference. You can mess with this copy.
		root = splitter(points, nullptr);
	}

	~KDTree(){
		deleteAll(root);
	}

	std::vector<T> search(const T &center,double radius) const {
		std::vector<T> tofill{};
		return recurs_find(tofill, root, center, radius);
	}

	void prettyPrint(){
		printTreeRecur(root);
	}

private:

	std::vector<T>& recurs_find(std::vector<T> &vec, Plane* p, const T &center, double radius) const {
		if(p->leaf == false){
			if (p->split >= center[p->dim] + radius) {
				return recurs_find(vec, p->left, center, radius);
			} else if (p->split < center[p->dim] - radius) {
				return recurs_find(vec, p->right, center, radius);
			} else {
				return recurs_find( recurs_find(vec, p->left, center, radius), p->right, center, radius);
			}
		} else {
			for(size_t i = 0; i < (p->data).size(); ++i){
				if (dist((p->data)[i], center, radius)){
					vec.push_back(p->data[i]);
				}
			}
			return vec;
		}
	}

	bool dist(const T point, const T center, double radius ) const {
		double d = 0;
		for(size_t i = 0; i < point.size(); ++i){
			d +=(point[i] - center[i])*(point[i] - center[i]);
		}
		if(std::sqrt(d) <= radius) return true;
		else return false;
	}

	std::pair<int, int> minmax(const std::vector<T>& vec, int dim) const {
		int min = 0;
		int max = 0;
		for(size_t i = 0; i< vec.size(); ++i){
			if(vec[i][dim] > vec[max][dim]){
				max = i;
			} else if(vec[i][dim] < vec[min][dim]){
				min = i;
			}
		}
		return std::pair<int, int>(min,max);
	}

	void indexPartition(std::vector<T> &points,int dimension, int index, int start, int end){
		if(end - start >1){
			int pivot = -1;
			if(end - start < 3) {pivot = start;} else {
				int mid = start + (end - start)/2;
				double ps = points[start][dimension];
				double pm = points[mid][dimension];
				double pe = points[end -1][dimension];
				if((ps<=pm && ps>=pe) || (ps>=pm && ps<=pe)) {
					pivot = start;
				} else if ((ps<=pm && pm<=pe)||(ps>=pm && pm>=pe)){
					pivot = mid;
				}	else {
					pivot = end- 1;
				}
			}
			T ptmp = points[pivot];
			points[pivot] = points[start];
			points[start] = ptmp;
			int lo = start + 1;
			int hi = end -1;
			while(lo <= hi) {
				if(points[lo][dimension] <= ptmp[dimension]){
					lo += 1;
				} else {
					T tmp2 = points[lo];
					points[lo] = points[hi];
					points[hi] = tmp2;
					hi -=1;
				}
			}
			points[start] = points[hi];
			points[hi] = ptmp;
			if(hi<index){
				indexPartition(points, dimension, index, hi+1, end);
			} else {
				indexPartition(points, dimension, index, start, hi);
			}
		}

	}

	Plane* splitter(std::vector<T> &points, Plane* p){
		Plane *ret = new Plane();
		if(points.size()<= PART_SIZE){
			 ret->leaf = true;
			 ret->data = points;//TODO Change to array
		} else{
			std::pair<int,int> spread (0,0);
			int dimension = 0;

			for(int i = 0; i < D; ++i){
				std::pair<int, int> temp = minmax(points,i);
				if (points[temp.second][i] - points[temp.first][i] >= points[spread.second][dimension] - points[spread.first][dimension]){
					dimension = i;
					spread = temp;
				}
			}
				ret->dim = dimension;
				int mid = (points.size()/2);
				indexPartition(points, dimension, mid, 0, points.size());
				ret->split = points[mid][dimension];
				std::vector<T> left {};
				std::vector<T> right {};
				for(int i = 0; i<mid+1;++i){
					left.push_back(points[i]);
				}
				for(size_t i = mid+1; i<points.size();++i){
					right.push_back(points[i]);
				}
				ret->left = splitter(left, ret);
				ret->right = splitter(right, ret);
		}
		return ret;
	}


	void deleteAll(Plane *n){
		if(n!=nullptr) {
			deleteAll(n->left);
			deleteAll(n->right);
			delete(n);
		}
	}

	void printTreeRecur(Plane *n) {
		if(n!=nullptr) {
			if(n->leaf){
				std::cout << "(";
			  for(int i = 0; i< n->data.size();++i){printpoint(n->data[i]);}
				std::cout << ")";
			} else {
			std::cout << "(";
			printTreeRecur(n->left);
			printTreeRecur(n->right);
			std::cout << ")";
		}
		}
	}

	void printpoint(T point){
		std::cout<<"<";
		for(int i = 0; i< D; ++i){
			std::cout<<point[i]<<" ";
		}
		std::cout<<">";
	}

};



#endif /* KDTREE_H_ */
