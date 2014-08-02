/*
	Copyright (c) 2003, Jeff Lee
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"
#include "Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

// Content-Type header field specifies the media type of a body part. it coule be:
// text/image/audio/video/application (discrete type) or message/multipart (composite type).
// the default Content-Type is: text/plain; charset=us-ascii (RFC 2046)

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   const char* MimeHeader::type_table_[] =
   {
	   "text", "image", "audio", "video", "application", "multipart", "message", NULL
   };

   const MimeHeader::MediaTypeCvt MimeHeader::type_cvt_table_[] =
   {
	   // media-type, sub-type, file extension
	   { MEDIA_APPLICATION, "xml", "xml" },
	   { MEDIA_APPLICATION, "msword", "doc" },
	   { MEDIA_APPLICATION, "rtf", "rtf" },
	   { MEDIA_APPLICATION, "vnd.ms-excel", "xls" },
	   { MEDIA_APPLICATION, "vnd.ms-powerpoint", "ppt" },
	   { MEDIA_APPLICATION, "pdf", "pdf" },
	   { MEDIA_APPLICATION, "zip", "zip" },

	   { MEDIA_IMAGE, "jpeg", "jpeg" },
	   { MEDIA_IMAGE, "jpeg", "jpg" },
	   { MEDIA_IMAGE, "gif", "gif" },
	   { MEDIA_IMAGE, "tiff", "tif" },
	   { MEDIA_IMAGE, "tiff", "tiff" },

	   { MEDIA_AUDIO, "basic", "wav" },
	   { MEDIA_AUDIO, "basic", "mp3" },

	   { MEDIA_VIDEO, "mpeg", "mpg" },
	   { MEDIA_VIDEO, "mpeg", "mpeg" },

	   { MEDIA_UNKNOWN, "", "" }		// add new subtypes before this line
   };
}
