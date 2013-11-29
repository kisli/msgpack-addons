//
// Add-on for serialization of boost::shared_ptr<> in MessagePack/C++
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

#ifndef MSGPACKADDONS_BOOST_SHARED_PTR_INCLUDED
#define MSGPACKADDONS_BOOST_SHARED_PTR_INCLUDED


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <msgpack.hpp>


/** Use case:

    #include <msgpack.hpp>
    #include <msgpack_boost_shared_ptr.hpp>

    struct A {
       int a;
       boost::shared_ptr <B> b;  // <-- this will be serialized and unserialized correctly
       MSGPACK_DEFINE(a, b);
    };

    struct B {
       int c;
       MSGPACK_DEFINE(c);
    };

*/


namespace msgpack {


template <typename T>
inline boost::shared_ptr <T> &operator>>(object o, boost::shared_ptr <T> &v) {
	if (o.type != type::ARRAY) {
		throw type_error();
	}
	if (o.via.array.size > 0) {
		v = boost::make_shared <T>();
		o.via.array.ptr->convert(v.get());
	} else {
		v.reset();  // NULL pointer
	}
	return v;
}

template <typename Stream, typename T>
inline packer <Stream> &operator<<(packer<Stream> &o, const boost::shared_ptr <T> &v) {
	o.pack_array(1);
	o.pack(*v.get());
	return o;
}

template <typename T>
inline void operator<<(object::with_zone &o, const boost::shared_ptr <T> &v) {
	o.type = type::ARRAY;
	if (!v) {
		o.via.array.ptr = NULL;
		o.via.array.size = 0;
	} else {
		object *p = static_cast <object *>(o.zone->malloc(sizeof(object)));
		o.via.array.ptr = p;
		o.via.array.size = v.size();
		*p = object(*(v.get()), o.zone);
	}
}


}  // namespace msgpack


#endif // MSGPACKADDONS_BOOST_SHARED_PTR_INCLUDED
