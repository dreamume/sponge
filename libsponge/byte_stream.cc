#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : data_(), capacity_(capacity) {
}

size_t ByteStream::write(const string &data) {
    if (end_input_) return 0;

    int len{0};
    while (len < data.size() && data_.size() < capacity_)
        data_.push_back(data[len++]);

    total_written_ += len;

    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t size = min(data_.size(), len);
    return string(data_.begin(), data_.begin() + size);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t size = min(data_.size(), len);
    total_read_ += size;
    data_.erase(data_.begin(), data_.begin() + size);
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);

    return res;
}

void ByteStream::end_input() { end_input_ = true; }

bool ByteStream::input_ended() const { return end_input_; }

size_t ByteStream::buffer_size() const { return data_.size(); }

bool ByteStream::buffer_empty() const { return data_.empty(); }

bool ByteStream::eof() const { return end_input_ && data_.empty(); }

size_t ByteStream::bytes_written() const { return total_written_; }

size_t ByteStream::bytes_read() const { return total_read_; }

size_t ByteStream::remaining_capacity() const { return capacity_ - data_.size(); }
