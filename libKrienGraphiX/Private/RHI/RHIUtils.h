
#pragma once

#ifndef DEFINE_RESOURCE_CAST
#define DEFINE_RESOURCE_CAST(To, From)		\
inline To* rcCast(From* resource){ return static_cast<To*>(resource); } \
inline const To* rcCast(const From* resource){ return static_cast<const To*>(resource); }
#endif
