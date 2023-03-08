#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();
    if (!_syn_flag) {
        if (!header.syn) return;
        _syn_flag = header.syn;
        _isn = header.seqno;
    }
    uint64_t checkpoint = header.syn ? 0 : stream_out().bytes_written() - 1;
    uint64_t abs_seqno = unwrap(header.seqno, _isn, checkpoint);
    uint64_t stream_idx = header.syn ? 0 : abs_seqno - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_idx, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if (!_syn_flag) return std::nullopt;
    uint64_t abs_ackno = stream_out().bytes_written() + 1;
    if (stream_out().input_ended()) abs_ackno += 1;

    return wrap(abs_ackno, _isn);
}

size_t TCPReceiver::window_size() const { 
    return _capacity - stream_out().buffer_size(); 
}
