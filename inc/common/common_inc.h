#ifndef  _COMMON_INC_H_
#define  _COMMON_INC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <set>

//tbase
#include "pal/ttypes.h"
#include "tapp/tapp.h"
#include "tdr/tdr.h"

//lua
#if defined(__cplusplus) 
	extern "C"
	{
	#include "lualib.h"
	#include "lauxlib.h"
	#include "toluapp.h"
	}
#else
	#include "lualib.h"
	#include "lauxlib.h"
	#include "toluapp.h"
#endif

#include "file/ifile.h"

#include "lua/lua_script.h"
#include "lua/script_mgr.h"

#include "math/utf8.h"
#include "math/hash.h"
#include "math/crc32.h"
#include "math/crc32c_fast.h"
#include "math/math_ex.h"

#include "process_error/process_error.h"
#include "file/ifile.h"
#include "string/string_ex.h"

#include "shm/shm_pool.h"

#include "config/global_config.h"

#include "app/server_app.h"
#include "app/server_msg_handler.h"

#include "module/server_module.h"

#include "define/server_def.h"
#include "define/error_def.h"

#include "tbus/tbus_wrapper.h"

#endif   /* ----- #ifndef _COMMON_INC_H_  ----- */

