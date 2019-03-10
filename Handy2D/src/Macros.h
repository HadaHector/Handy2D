#pragma once

#define POINTER_TYPEDEFS(name) typedef std::shared_ptr<C##name> P##name; \
typedef std::weak_ptr<C##name> W##name;

#define PNEW(name) std::make_shared<name>()