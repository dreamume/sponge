#include "stream_reassembler.hh"

#include <vector>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}
StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _buffer(), _unassembled_size(0), _is_eof(false), _eof_idx(0) {}

void StreamReassembler::buffer_erase(const set<Segment>::iterator &it) {
    _unassembled_size -= it->length();
    _buffer.erase(it);
}

void StreamReassembler::buffer_insert(const Segment &seg) {
    _unassembled_size += seg.length();
    _buffer.insert(seg);
}

void StreamReassembler::handle_substring(Segment& seg) {
    if (seg._idx >= _1st_unacceptable_idx()) return;

    if (seg._idx < _1st_unacceptable_idx() && 
	seg._idx + seg.length() - 1 >= _1st_unacceptable_idx())
	seg._data = seg._data.substr(0, _1st_unacceptable_idx() - seg._idx);

    if (seg._idx + seg.length() - 1 < _1st_unassembled_idx()) return;

    if (seg._idx < _1st_unassembled_idx() && 
	seg._idx + seg.length() - 1 >= _1st_unassembled_idx()) {
	seg._data = seg._data.substr(_1st_unassembled_idx() - seg._idx);
	seg._idx = _1st_unassembled_idx();
    }

    if (_buffer.empty()) _buffer.insert(seg);
    else handle_overlap(seg);
}

void StreamReassembler::handle_overlap(Segment& seg) {
    for (auto it = _buffer.begin(); it != _buffer.end();) {
	size_t seg_tail = seg._idx + seg.length() - 1;
	size_t cache_tail = it->_idx + it->length() - 1;

	if ((seg._idx >= it->_idx && seg._idx <= cache_tail) ||
	    (it->_idx >= seg._idx && it->_idx <= seg_tail)) {
	    merge_seg(seg, *it);
	    buffer_erase(it++);
	} else {
	    ++it;
	}
    }

    buffer_insert(seg);
}

void StreamReassembler::merge_seg(Segment& seg, const Segment &cache) {
    size_t seg_tail = seg._idx + seg.length() - 1;
    size_t cache_tail = cache._idx + cache.length() - 1;

    if (seg._idx < cache._idx && seg_tail <= cache_tail) {
	seg._data = seg._data.substr(0, cache._idx - seg._idx) + cache._data;
    } else if (seg._idx >= cache._idx && seg_tail > cache_tail) {
	seg._data = 
	    cache._data + seg._data.substr(cache._idx + cache.length() - seg._idx);
	seg._idx = cache._idx;
    } else if (seg._idx >= cache._idx && seg_tail <= cache_tail) {
	seg._data = cache._data;
	seg._idx = cache._idx;
    }
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (!data.empty()) {
	Segment seg{index, data};
	handle_substring(seg);
    }

    while (!_buffer.empty() && _buffer.begin()->_idx == _1st_unassembled_idx()) {
	const auto& it = _buffer.begin();
	_output.write(it->_data);
	buffer_erase(it);
    }

    if (eof) {
	_is_eof = eof;
	_eof_idx = index + data.length();
    }
    if (_is_eof && _1st_unassembled_idx() == _eof_idx)
	_output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_size; }

bool StreamReassembler::empty() const { return _buffer.empty(); }
