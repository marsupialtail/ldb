#include "vector"
#include "queue"
#include "cassert"
#include "iostream"
#include "algorithm"
#include "cstdint"
#include "exceptions.h"
#include <stdio.h>
#include <chrono>

void print_vector(const std::vector<uint64_t>& vec) {
    for (const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

void print_vec_vec(const std::vector<std::vector<uint64_t>>& vec) {
    for (const auto& element : vec) {
        print_vector(element);
    }
}

std::vector<std::vector<uint64_t>> index_to_ts(std::vector<std::vector<uint64_t>> index, uint64_t* ts) {
    std::vector<std::vector<uint64_t>> ret;
    for (auto vec : index) {
        std::vector<uint64_t> tmp;
        for (auto i : vec) {
            tmp.push_back(ts[i]);
        }
        ret.push_back(tmp);
    }
    return ret;
}

bool fsmaccept(std::vector<uint64_t> ts_vec) {
    // given a sequence, use finite state machine to check if it is accepted
    // the FSM only needs to maintain the previous timestamp element it has seen, and a counter (i)
    for (int i = 0; i < ts_vec.size() - 1; i++) {
        // time stamp should not decrease
        if (ts_vec[i+1] < ts_vec[i]) {
            return false;
        }
        // time stamp must be within the time window
        if (ts_vec[i+1] > ts_vec[i]+DIFF) {
            return false;
        }
    }
    return true;
}

// Currently not used, replaced by find_tuples
void productImplement(std::vector<std::vector<uint64_t>> dimvalue,
                    std::vector<std::vector<uint64_t>> &res,
                    int layer,
                    std::vector<uint64_t> tmp) {
	if (layer < dimvalue.size() - 1){
		for (int i = 0; i < dimvalue[layer].size(); i++){
			std::vector<uint64_t> sb;
			sb.clear();
			for (int i = 0; i < tmp.size(); i++){
				sb.push_back(tmp[i]);
			}
			sb.push_back(dimvalue[layer][i]);
			productImplement(dimvalue, res, layer+1,sb);
		}
	}
	else if (layer == dimvalue.size() - 1){
		for (int j = 0; j < dimvalue[layer].size();j++){
            tmp.push_back(dimvalue[layer][j]);
			res.push_back(tmp);
			tmp.pop_back();
		}
	}
}

// this is FSM equivalent of the simple function
void fsm(int k, uint64_t* ts, std::vector<std::vector<uint64_t>> conditions,
         std::vector<std::vector<uint64_t>>& ret) {
   // compute catesian product for condition arrays as index arrays' catesian product
   std::vector<std::vector<uint64_t>> catesian_product;
   std::vector<uint64_t> tmp;
   find_tuples(conditions, catesian_product);
   // productImplement(conditions, catesian_product, 0, tmp);
   // print_vec_vec(catesian_product);
   // printf("xxxxxxxx\n");
   // use the index product to generate ts product, used for FSM input to be filtered
   std::vector<std::vector<uint64_t>> ts_product = index_to_ts(catesian_product, ts);
   for (int i = 0; i < ts_product.size(); i++) {
       if (fsmaccept(ts_product[i])) {
           ret.push_back(catesian_product[i]);
       }
   }
}

void benchmark1(int k, uint64_t* ts, std::vector<std::vector<uint64_t>> conditions,
                std::vector<std::vector<uint64_t>>& ret)
{
    auto begin = std::chrono::high_resolution_clock::now();
    
    simple(k, ts, conditions, ret);
    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
}

void benchmark2(int k, uint64_t* ts, std::vector<std::vector<uint64_t>> conditions,
                std::vector<std::vector<uint64_t>>& ret)
{
    auto begin = std::chrono::high_resolution_clock::now();
    
    fsm(k, ts, conditions, ret);
    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
}


void check_result(std::vector<std::vector<uint64_t>> x, std::vector<std::vector<uint64_t>> y) {
    for (int i = 0; i < x.size(); i++) {
        for (int j = 0; j < x[i].size(); j++) {
            if (x[i][j] != y[j][i]) {
                printf("FAILED\n");
                return;
            }
        }
    }
    printf("PASSED\n");
}
