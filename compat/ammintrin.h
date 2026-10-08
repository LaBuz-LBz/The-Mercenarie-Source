#pragma once
// The standalone VC++ 2010 SP1 compiler package can omit this optional AMD
// SSE4a header. KenshiLib does not use those intrinsics, so an empty shim is
// sufficient and keeps the build on the required v100 ABI.
