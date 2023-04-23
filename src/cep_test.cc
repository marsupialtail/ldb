#include "vector"
#include "queue"
#include "cassert"
#include "iostream"
#include "algorithm"
#include "cstdint"
#include "exceptions.h"

#define DIFF 5

void find_tuples(std::vector<std::vector<uint64_t>> lists, std::vector<std::vector<uint64_t>> & results) {
    uint64_t limit = lists.size();
    std::queue<std::pair<std::vector<uint64_t>, std::pair<uint64_t, uint64_t>>> q;
    q.push(std::make_pair(std::vector<uint64_t>{lists[0][0]}, std::make_pair(0, lists[0][0])));
    while (!q.empty()) {
        std::vector<uint64_t> curr_tup = q.front().first;
        std::pair<uint64_t, uint64_t> curr_index = q.front().second;
        q.pop();
        uint64_t curr_list = curr_index.first;
        uint64_t curr_value = curr_index.second;
        std::vector<std::pair<uint64_t, uint64_t>> to_add;
        for (uint64_t i = 0; i < lists[curr_list + 1].size(); i++) {
            uint64_t value = lists[curr_list + 1][i];
            if (value >= curr_value && value <= curr_value + DIFF) {
                to_add.push_back(std::make_pair(lists[curr_list + 1][i], value));
            }
        }
        if (curr_list == limit - 2) {
            for (auto tup : to_add) {
                curr_tup.push_back(tup.first);
                results.push_back(curr_tup);
                curr_tup.pop_back();
            }
        } else {
            for (auto tup : to_add) {
                curr_tup.push_back(tup.first);
                q.push(std::make_pair(curr_tup, std::make_pair(curr_list + 1, tup.second)));
                curr_tup.pop_back();
            }
        }
    }
}

void simple(int k, const uint64_t * ts, const std::vector<std::vector<uint64_t>> conditions, std::vector<std::vector<uint64_t>> & ret) {
   
    
    std::vector<uint64_t> fingers(k);
    // ts[conditions[fingers[i]]] must be bigger than or equal to ts[conditions[fingers[i - 1]]]]
    // fingers[i] gets bigger than conditions[i].size() when it reaches the end, quit loop

    uint64_t start_time = ts[conditions[0][0]];
    for(int i = 1; i < k; i++) {
        // walk finger[i] forward enough such that ts[conditions[i][fingers[i]]] >= start_time
        while (fingers[i] < conditions[i].size() && ts[conditions[i][fingers[i]]] < start_time) {
            fingers[i]++;
        }
        if (fingers[i] == conditions[i].size()) {
            // we are done
            return;
        }
        start_time = ts[conditions[i][fingers[i]]];
    }

    std::vector<std::vector<uint64_t>> local_ret(k, std::vector<uint64_t>());

    while (fingers[0] < conditions[0].size()) {

        // check if we are done
        bool done = true;
        for (int i = 1; i < k; i++) {
            if (fingers[i] == conditions[i].size()) {
                return;
            }
        }

        for (int i = 0; i < k; i++) {
            local_ret[i].clear();
        }
        local_ret[0].push_back(conditions[0][fingers[0]]);

        // compute the window now
        uint64_t start_time = ts[conditions[0][fingers[0]]];
        uint64_t end_time = start_time + DIFF;
        for (int i = 1; i < k; i++) {
            // increment fingers[i] until ts[conditions[i][fingers[i]]] >= start_time
            while (ts[conditions[i][fingers[i]]] < start_time && fingers[i] < conditions[i].size()) {
                fingers[i] ++;
            }
            start_time = ts[conditions[i][fingers[i]]];
            uint64_t local_finger = fingers[i];
            while (ts[conditions[i][local_finger]] <= end_time && local_finger < conditions[i].size()) {
                local_ret[i].push_back(conditions[i][local_finger]);
                local_finger ++;
            }
            end_time = ts[conditions[i][local_finger - 1]] + DIFF;
        }

        std::vector<std::vector<uint64_t>> expanded_results;
        find_tuples(local_ret, expanded_results);
        //print out local_ret

        for (auto vec : local_ret) {
            for (auto i : vec) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }

        for (auto vec : expanded_results) {
            for (int i = 0; i < k; i++) {
                ret[i].push_back(vec[i]);
            }
        }
        fingers[0] ++;

        //extend the local_ret into ret
    }    
}

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
   productImplement(conditions, catesian_product, 0, tmp);
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


int main()
{
    // generate a test case for the function simple
    // test case 1: 3 conditions, 3 events, 1 tuple
    int k = 3;
    uint64_t ts[] = {0, 1, 2, 3, 4, 5, 6};
    std::vector<std::vector<uint64_t>> conditions;
    conditions.push_back(std::vector<uint64_t>{0, 1});
    conditions.push_back(std::vector<uint64_t>{1, 4});
    conditions.push_back(std::vector<uint64_t>{2, 5, 6});
    std::vector<std::vector<uint64_t>> ret(k);
    for (int i = 0; i < k; i++) {
        ret[i] = std::vector<uint64_t>();
        ASSERT_MSG(conditions[i].size() > 0, "condition is empty, should not happen")
    }
    simple(k, ts, conditions, ret);
    for (auto vec : ret) {
        for (auto i : vec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------" << std::endl;
    std::vector<std::vector<uint64_t>> ret2;
    fsm(3, ts, conditions, ret2);
    print_vec_vec(ret2);
}