#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const {
    return _sender.stream_in().remaining_capacity();
}

size_t TCPConnection::bytes_in_flight() const {
    return _sender.bytes_in_flight();
}

size_t TCPConnection::unassembled_bytes() const {
    return _receiver.unassembled_bytes();
}

size_t TCPConnection::time_since_last_segment_received() const {
    return _time_since_last_segment_received;
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    if(!_active) {
        return;
    }
    TCPHeader header = seg.header();
    if(header.rst) {
        _active = false;
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        return;
    }
    // 转交ackno,window_size给sender
    if(header.ack) {
        _sender.ack_received(header.ackno, header.win);
    }
    if(_receiver.ackno().has_value() && (seg.length_in_sequence_space() == 0)
        && header.seqno == _receiver.ackno().value() - 1) {  // invalid seqno => keepalive segment
        _sender.send_empty_segment();
    } else {    // 转交seg给recver
        _receiver.segment_received(seg);
    }
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    size_t ret = _sender.stream_in().write(data);
    _sender.fill_window();
    return ret;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _time_since_last_segment_received = ms_since_last_tick;
    if(_sender.consecutive_retransmissions() > _cfg.MAX_RETX_ATTEMPTS) {
        // send RST in deconstructor
    }
    // TIME_WAIT phase
//    if(_linger_after_streams_finish && _time_since_last_segment_received)
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
}

void TCPConnection::connect() {
    _active = true;
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
            TCPSegment seg;
            seg.header().rst = true;
            seg.header().seqno = _sender.next_seqno();
            _sender.segments_out().push(seg);
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
