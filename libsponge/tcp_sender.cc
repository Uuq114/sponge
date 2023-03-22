#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>
#include <iostream>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _retransmission_timeout{retx_timeout} {}

uint64_t TCPSender::bytes_in_flight() const { return _next_seqno - _ackno; }

void TCPSender::fill_window() {
    if(_win_size == 0 || _is_fin) {
        return;
    }
    TCPSegment seg;
    if(!_is_syn) {   // send syn to handshake
        _is_syn = true;
        seg.header().syn = true;
        seg.header().seqno = next_seqno();
        ++_next_seqno;
        --_win_size;
        segments_out().push(seg);
        segments_to_be_acked().push(seg);
        cout<<"syn sent"<<endl;
    } else if(stream_in().eof()) {  // send fin
        _is_fin = true;
        seg.header().fin = true;
        seg.header().seqno = next_seqno();
        ++_next_seqno;
        --_win_size;
        segments_out().push(seg);
        segments_to_be_acked().push(seg);
    } else {    // normal payload
        while(_win_size > 0 && !stream_in().buffer_empty()) {
            seg.header().seqno = next_seqno();
            uint64_t send_len = min(TCPConfig::MAX_PAYLOAD_SIZE, min(_win_size, stream_in().buffer_size()));
            seg.payload() = stream_in().read(send_len);
            if(seg.length_in_sequence_space() < _win_size && stream_in().eof()) {   // 数据发完了，附带fin
                seg.header().fin = true;
                _is_fin = true;
            }
            _next_seqno += seg.length_in_sequence_space();
            _win_size -= seg.length_in_sequence_space();
            segments_out().push(seg);
            segments_to_be_acked().push(seg);
        }
    }
    // open timer
    if(!_is_timer_running) {
        _is_timer_running = true;
        _total_time = 0;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t absolute_ackno = unwrap(ackno, _isn, _next_seqno);
    _win_size = window_size;
    if(absolute_ackno > _next_seqno || absolute_ackno <= _ackno) {
        return;
    }

    _retransmission_timeout = _initial_retransmission_timeout;  // 收到ack，RTO恢复初始值
    _consecutive_retransmission = 0;
    while(!segments_to_be_acked().empty()) {
        TCPSegment seg = segments_to_be_acked().front();
        uint64_t absolute_seg_end_no = unwrap(seg.header().seqno, _isn, _next_seqno) + seg.length_in_sequence_space();
        if(absolute_seg_end_no <= absolute_ackno) {
            _ackno = absolute_seg_end_no;
            segments_to_be_acked().pop();
        } else {
            break;
        }
    }
    if(!segments_to_be_acked().empty()) {   // 还有on air的数据，复位timer
        _total_time = 0;
        _is_timer_running = true;
    }
    if(bytes_in_flight() == 0) {    // 没有正在传输的数据，关闭timer
        _is_timer_running = false;
    }
    if(bytes_in_flight() > window_size) {
        _win_size = 0;
        _win_zero_flag = true;
        return;
    }
    if(_win_size == 0) {
        _win_zero_flag = true;
        _win_size = 1;
    } else {
        _win_zero_flag = false;
    }
    fill_window();
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
//    cout<<"tick(): since_last_tick:"<<ms_since_last_tick<<endl;
    _total_time += ms_since_last_tick;
//    cout<<"tick(): total_time:"<<_total_time<<endl;
//    cout<<"tick(): rx_to:"<<_retransmission_timeout<<endl;

    if(_total_time >= _retransmission_timeout && !segments_to_be_acked().empty()) {
        segments_out().push(segments_to_be_acked().front());
        if(!_win_zero_flag) {
            ++_consecutive_retransmission;
            _retransmission_timeout *= 2;
        }
        _total_time = 0;
    }
    if(segments_to_be_acked().empty()) {
        _is_timer_running = false;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmission; }

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = next_seqno();
    segments_out().push(seg);
}
