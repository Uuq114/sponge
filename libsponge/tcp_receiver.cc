#include "tcp_receiver.hh"
#include <iostream>
using std::cout;
using std::endl;

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
//    bool old_syn_recved = _syn_recved;  // 上一个连接的状态，在新连接建立时需要清除
//    bool old_fin_recved = _fin_recved;
    const TCPHeader header = seg.header();
    Buffer payload = seg.payload();

    if(header.syn) {
        _syn_recved = true;
        _isn = header.seqno;
    }
    // window
    uint64_t win_start = unwrap(*ackno(), _isn, _checkpoint);
    uint64_t win_size = window_size() ? window_size() : 1;
    uint64_t win_end = win_start + win_size - 1;
    // seq
    uint64_t seq_start = unwrap(header.seqno, _isn, _checkpoint);
    uint64_t seq_size = seg.length_in_sequence_space();
    seq_size = (seq_size) ? seq_size : 1;
//    uint64_t seq_end = seq_start + seq_size - 1;
    // payload
    uint64_t payload_size = payload.str().size();
    payload_size = (payload_size) ? payload_size : 1;
//    uint64_t payload_end = seq_start + payload_size - 1;
    // is in window
    bool inbound = seq_start >= win_start && seq_start <= win_end;
    if(inbound) {
        _reassembler.push_substring(payload.copy(), seq_start - 1, header.fin);    // reassembler忽视syn，减掉1

    }
    // fin
    if(header.fin && !_fin_recved) {
        _fin_recved = true;
        _syn_recved = false;    // 收到fin，准备开启一条新连接
        if(_reassembler.empty()) _reassembler.stream_out().end_input();
    }
    // ack no
    bool fin_finished = _fin_recved && _syn_recved && (_reassembler.first_unassembled_byte() == 0);    // 只有fin包，没有数据
    cout<<"fin_finished:"<<fin_finished<<endl;
    cout<<"first unassemled byte:"<<_reassembler.first_unassembled_byte()<<endl;
    _ackno = wrap(_reassembler.first_unassembled_byte() + 1 + fin_finished, _isn);
    // recv fin twice

}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_syn_recved) {
        return nullopt;
    } else {
        return {_ackno};
    }
}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }
