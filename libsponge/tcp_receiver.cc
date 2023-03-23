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
    const TCPHeader header = seg.header();

    if(!_syn_recved && !header.syn) {     // 连接还未建立，就收到了数据包，直接丢弃
        return;
    }
    if(header.syn) {
        _syn_recved = true;
        _isn = header.seqno;
    }

    std::string payload = seg.payload().copy();
    WrappingInt32 seqno = header.syn ? header.seqno + 1 : header.seqno; // 第一个有效字符的序号
    uint64_t checkpoint = stream_out().bytes_written();
    uint64_t index = unwrap(seqno, _isn, checkpoint) - 1; // -1是因为unwrap得到的是absolute seqno,syn是0,数据从1开始
    _reassembler.push_substring(payload, index, _syn_recved && header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_syn_recved) {
        return nullopt;
    } else {
        size_t ack = stream_out().bytes_written() + 1;  // +1加的是syn占用的序号
        if(stream_out().input_ended()) {
            return {wrap(ack + 1, _isn)};   // 已经结束了，需要加上fin的一个序号
        } else {
            return {wrap(ack, _isn)};
        }
    }
}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }
