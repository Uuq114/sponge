#include "stream_reassembler.hh"
#include <vector>
//#include <iostream>
//using std::cout;
//using std::endl;

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _first_unassembled_index(0), _n_assembled_byte(0), _eof_index(SIZE_MAX), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
//    cout<<"===="<<endl;
    // eof
    if(eof) { // 当前的data是最后的值
        _eof_index = index + data.length();
        if(_first_unassembled_index >= _eof_index) _output.end_input(); // 有传进来空串+eof的case
    }
    // 收到重复的数据，或者是空数据，或者超过了eof_index的，直接丢弃
    string processed_data = data;
    if(index + data.length() <= _first_unassembled_index) {
        return;
    }
    if(index + data.length() >= _eof_index) {
        processed_data = data.substr(0, _eof_index - index);
    }
    // 尝试将新来的substring和暂存的substring合并
    merge_substring(index, processed_data);
    // 在暂存的substring中寻找
    auto it = _unassembled.lower_bound(UnassembledSubstring(_first_unassembled_index, ""));
//    if(it == _unassembled.end()) { // 没有可以合并的
//        cout<<"没有可以合并的"<<endl;
//        return;
//    }
//    cout<<"after merge, begin push, first unassembled index:"<<_first_unassembled_index<<endl;
    if(it != _unassembled.end() && it->index == _first_unassembled_index) { // 暂存的数据中，有可以直接push的data
//        cout<<"可以直接push数据"<<endl;
        auto write_len = _output.write(it->data);
//        cout<<"write_len:"<<write_len<<endl;
        size_t new_index = it->index + write_len;
        string new_data = it->data.substr(write_len);
        //　删除已经读取的
        _unassembled.erase(it);
        // 插入新的
        if(!new_data.empty()) {
            _unassembled.insert(UnassembledSubstring(new_index, new_data));
//            cout<<"没读取完，set insert:"<<new_data<<endl;
        }
        _first_unassembled_index += write_len;
    } else { // 暂存的数据中，可能有和已经push的数据有重叠的数据
//        cout<<"可能有重叠数据"<<endl;
//        cout<<"first unassembled index:"<<_first_unassembled_index<<endl;
        --it;
        if(it != _unassembled.end() && it->index <= _first_unassembled_index) {
//            cout<<"有重叠数据$$"<<endl;
//            cout<<"it->index"<<it->index<<endl;
            string tail = it->data.substr(_first_unassembled_index - it->index);
            auto write_len = _output.write(tail);
            size_t new_index = it->index + write_len;
            string new_data = it->data.substr(_first_unassembled_index - it->index + write_len);
            // 删除it对应的元素
            _unassembled.erase(it);
            // 插入新的
            if(!new_data.empty()) {
                _unassembled.insert(UnassembledSubstring(new_index, new_data));
//                cout<<"没读取完，set insert:"<<new_data<<endl;
            }
            _first_unassembled_index += write_len;
        }
    }
    // eof
//    cout<<"eof, first unassembled index:"<<_first_unassembled_index<<endl;
//    cout<<"eof, eof_index:"<<_eof_index<<endl;
    if(_first_unassembled_index >= _eof_index) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t ret = 0;
    for(auto it = _unassembled.begin(); it != _unassembled.end(); ++it) {
        ret += it->data.length();
    }
    return ret;
}

bool StreamReassembler::empty() const {
    return _unassembled.size() == 0;
}

void StreamReassembler::merge_substring(size_t index, const std::string& data) {
//    cout<<"merge string: index="<<index<<endl;
    // 可以合并的it
    auto it = _unassembled.begin();
    vector<decltype(it)> remove_it;
    size_t l2 = index, r2 = index + data.length();
    size_t merged_index = index;
    string merged_data = data;
    for(; it != _unassembled.end(); ++it) {
        size_t l1 = it->index, r1 = it->index + it->data.length();
        if(l1 <= l2) {
            if(r1 >= l2) {
                merged_index = min(merged_index, l1);
                merged_data = it->data.substr(0, l2 - l1) + merged_data;
                if(r1 >= r2) merged_data += it->data.substr(r2 - l1);   // set中的元素覆盖了新来的data
                remove_it.push_back(it);
            }
        } else {
            if(r2 >= l1) {
                merged_data += it->data.substr(min(r2 - l1, it->data.length()));
                remove_it.push_back(it);
            }
        }
    }
    // 清除被合并的it
    for(auto& i : remove_it) {
        _unassembled.erase(i);
    }
    // 插入新合并的
    _unassembled.insert(UnassembledSubstring(merged_index, merged_data));
//    cout<<"set size:"<<_unassembled.size()<<endl;
//    for(auto i : _unassembled) {
//        cout<<i.index<<","<<i.data<<";";
//    }
//    cout<<endl;
//    cout<<"merged_index:"<<merged_index<<endl;
//    cout<<"merged_data:"<<merged_data<<endl;
//    cout<<"merged_data len:"<<merged_data.length()<<endl;
}

