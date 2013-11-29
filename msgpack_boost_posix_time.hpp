//
// Add-on for serialization of boost::posix_time in MessagePack/C++
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

#ifndef MSGPACKADDONS_BOOST_POSIX_TIME_INCLUDED
#define MSGPACKADDONS_BOOST_POSIX_TIME_INCLUDED


#include <ctime>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <msgpack.hpp>


/** Use case:

    #include <msgpack.hpp>
    #include <msgpack_boost_posix_time.hpp>

    struct A {
       int a;
       boost::posix_time::ptime b;  // <-- this will be serialized and unserialized
                                    //     as a 64-bit unsigned integer
       MSGPACK_DEFINE(a, b);
    };

*/


namespace msgpack {

namespace detail {
	struct boost_posix_time_converter {
		static inline std::time_t to_time_t(const boost::posix_time::ptime t) {
			const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
			const boost::posix_time::time_duration::sec_type x = (t - epoch).total_seconds();
			return std::time_t(x);
		}
	};
}  // namespace detail


inline boost::posix_time::ptime &operator>>(object o, boost::posix_time::ptime &v) {
	if (o.type == type::POSITIVE_INTEGER) {
		const std::time_t t = static_cast <std::time_t>(o.via.u64);
		v = boost::posix_time::from_time_t(t);
		return v;
	}
	throw type_error();
}

template <typename Stream>
inline packer <Stream> &operator<<(packer<Stream>& o, const boost::posix_time::ptime &v) {
	o.pack_unsigned_long_long(detail::boost_posix_time_converter::to_time_t(v));
	return o;
}

inline void operator<<(object &o, boost::posix_time::ptime v) {
	o.type = type::POSITIVE_INTEGER;
	o.via.u64 = detail::boost_posix_time_converter::to_time_t(v);
}

inline void operator<<(object::with_zone &o, boost::posix_time::ptime v) {
	static_cast <object&>(o) << v;
}


}  // namespace msgpack


#endif // MSGPACKADDONS_BOOST_POSIX_TIME_INCLUDED
