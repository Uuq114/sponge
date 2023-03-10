#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity):
    _capacity(capacity), _size(0), _n_written(0), _n_read(0), _input_ended(false) {
}

size_t ByteStream::write(const string &data) {
    size_t len = min(data.size(), _capacity - _size);
    _stream_buffer.append(BufferList(move(data.substr(0, len))));
    _size += len;
    _n_written += len;
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t peek_len = min(len, _size);
    return _stream_buffer.concatenate().substr(0, peek_len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t pop_len = min(len, _size);
    _stream_buffer.remove_prefix(pop_len);
    _size -= pop_len;
    _n_read += pop_len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    size_t read_len = min(len, _size);
    string ret = peek_output(read_len);
    _stream_buffer.remove_prefix(read_len);
    _n_read += read_len;
    _size -= read_len;
    return ret;
}

void ByteStream::end_input() { _input_ended = true; }

bool ByteStream::input_ended() const { return _input_ended; }

size_t ByteStream::buffer_size() const { return _size; }

bool ByteStream::buffer_empty() const { return _size == 0; }

bool ByteStream::eof() const { return input_ended() && _size == 0; }

size_t ByteStream::bytes_written() const { return _n_written; }

size_t ByteStream::bytes_read() const { return _n_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _size; }
