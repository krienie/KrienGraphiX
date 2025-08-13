
#pragma once

#ifndef DEFINE_RESOURCE_CAST
#define DEFINE_RESOURCE_CAST(To, From)		\
inline To* dxCast(From* resource){ return static_cast<To*>(resource); } \
inline const To* dxCast(const From* resource){ return static_cast<const To*>(resource); }
#endif
