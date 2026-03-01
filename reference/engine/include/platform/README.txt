The registry.hpp file is one of the most performance critical pieces
of code I've written for my game engine. It's a stripped down entity
component system registry which allows for components to be dynamically
attached and removed from entities at runtime. Using a data-orientated
layout with sparse mappings between each component type and their
associated entities, the entity registry is capable of filtering and
find entities with common components.

The big performance win for this is the compile-time pseudo-type reflection
system. Although C++ doesn't have type reflection, using clever compiler
macros and constexpr evaluations make it possible to deduce and store arrays
of ambiguous types.