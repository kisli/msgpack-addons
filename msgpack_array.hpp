//
// Add-on for serialization of arrays in MessagePack/C++
//
// Copyright (C) 2013 Kisli    http://www.kisli.com
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#ifndef MSGPACKADDONS_ARRAY_INCLUDED
#define MSGPACKADDONS_ARRAY_INCLUDED


#include <msgpack.hpp>


/** Use case:

    #include <msgpack.hpp>
    #include <msgpack_array.hpp>

    struct A {
       int a;
       unsigned char b[16];  // <-- this will be serialized and unserialized
                             //     as an array
       MSGPACK_DEFINE(a, b);
    };

*/


namespace msgpack {


template <typename T, int N>
inline T* operator>>(object o, T (&v)[N]) {
	if (o.type != type::ARRAY || o.via.array.size != N) {
		throw type_error();
	}
	if (o.via.array.size > 0) {
		object* p = o.via.array.ptr;
		object* const pend = o.via.array.ptr + o.via.array.size;
		T* it = &v[0];
		do {
			p->convert(it);
			++p;
			++it;
		} while (p < pend);
	} else {
		for (int i = 0 ; i < N ; ++i) {
			v[i] = T();
		}
	}
	return v;
}

template <typename Stream, typename T, int N>
inline packer <Stream> &operator<<(packer<Stream> &o, const T (&v)[N]) {
	o.pack_array(N);
	for (int i = 0 ; i < N ; ++i) {
		o.pack(v[i]);
	}
	return o;
}

template <typename T, int N>
inline void operator<<(object::with_zone &o, const T (&v)[N]) {
	o.type = type::ARRAY;
	if (N == 0) {
		o.via.array.ptr = NULL;
		o.via.array.size = 0;
	} else {
		object *p = static_cast <object *>(o.zone->malloc(sizeof(object) * N));
		object *const pend = p + N;
		o.via.array.ptr = p;
		o.via.array.size = N;
		const T *it = &v[0];
		do {
			*p = object(*it, o.zone);
			++p;
			++it;
		} while (p < pend);
	}
}


} // namespace msgpack


#endif // MSGPACKADDONS_ARRAY_INCLUDED
