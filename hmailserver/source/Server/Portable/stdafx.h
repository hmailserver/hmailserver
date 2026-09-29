// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// The precompiled header for the portable build.
//
// Sources under Common include "stdafx.h" and find hMailServer/stdafx.h,
// because the server project puts its own directory on the include path.
// The portable build puts this directory there instead, so the same sources
// see only the declarations that do not need the Windows SDK.
//
// A source file belongs in the portable build only if it compiles against
// this header. Anything reaching for Windows, ATL, ADO or COM does not, and
// stays in the server project alone.

#pragma once

#include <cassert>

#include "../Common/PortableCore.h"
