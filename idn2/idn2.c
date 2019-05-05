#include <stddef.h> /* NULL */
#include <stdint.h> /* uint8_t */

#include <idn2.h>

#include <lua.h>
#include <lauxlib.h>

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
#define lua_absindex luaidn2_absindex
static int lua_absindex (lua_State *L, int i) {
	if (i < 0 && i > LUA_REGISTRYINDEX)
		i += lua_gettop(L) + 1;
	return i;
}

#define luaL_setfuncs luaidn2L_setfuncs
static void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
	luaL_checkstack(L, nup+1, "too many upvalues");
	for (; l->name != NULL; l++) {  /* fill the table with given functions */
		int i;
		lua_pushstring(L, l->name);
		for (i = 0; i < nup; i++)  /* copy upvalues to the top */
			lua_pushvalue(L, -(nup + 1));
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_settable(L, -(nup + 3)); /* table must be below the upvalues, the name and the closure */
	}
	lua_pop(L, nup);  /* remove upvalues */
}
#endif

static int luaidn2_push_error(lua_State *L, int rc) {
	lua_pushnil(L);
	lua_pushstring(L, idn2_strerror(rc));
	return 2;
}

static void* new_boxed_pointer(lua_State *L) {
	void **ptr = lua_newuserdata(L, sizeof(void *));
	*ptr = NULL;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
	return ptr;
}

static const int idn2_flags_list[] = {
	IDN2_NFC_INPUT,
	IDN2_ALABEL_ROUNDTRIP,
#ifdef IDN2_NO_TR46
	IDN2_NO_TR46,
#endif
	IDN2_TRANSITIONAL,
	IDN2_NONTRANSITIONAL
#ifdef IDN2_ALLOW_UNASSIGNED
	IDN2_ALLOW_UNASSIGNED,
#endif
#ifdef IDN2_USE_STD3_ASCII_RULES
	IDN2_USE_STD3_ASCII_RULES,
#endif
};
static const char *idn2_flags_names[] = {
	"nfc_input",
	"alabel_roundtrip",
#ifdef IDN2_NO_TR46
	"no_tr46",
#endif
	"transitional",
	"nontransitional",
#ifdef IDN2_ALLOW_UNASSIGNED
	"allow_unassigned",
#endif
#ifdef IDN2_USE_STD3_ASCII_RULES
	"use_std3_ascii_rules",
#endif
	NULL
};
static int checkidn2_flags(lua_State *L, int idx) {
	size_t i;
	int res = 0;
	idx = lua_absindex(L, idx);
	luaL_checktype(L, idx, LUA_TTABLE);
	for (i = 0; i < (sizeof(idn2_flags_list)/sizeof(int)); i++) {
		lua_getfield(L, idx, idn2_flags_names[i]);
		if (lua_toboolean(L, -1)) {
			res |= idn2_flags_list[i];
		}
		lua_pop(L, 1);
	}
	return res;
}
#define optidn2_flags(L, n, d) luaL_opt((L), checkidn2_flags, (n), (d))

#if IDN2_VERSION_NUMBER >= 0x02000000
static int luaidn2_to_ascii(lua_State *L) {
	int res;
	const char *input = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **output = new_boxed_pointer(L);
	res = idn2_to_ascii_8z(input, output, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *output);
	}
	idn2_free(*output);
	*output = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_to_unicode(lua_State *L) {
	int res;
	const char *input = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **output = new_boxed_pointer(L);
	res = idn2_to_unicode_8z8z(input, output, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *output);
	}
	idn2_free(*output);
	*output = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}
#endif

static int luaidn2_lookup(lua_State *L) {
	int res;
	const uint8_t *src = (const uint8_t*)luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	uint8_t **lookupname = new_boxed_pointer(L);
	res = idn2_lookup_u8(src, lookupname, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, (char*)*lookupname);
	}
	idn2_free(*lookupname);
	*lookupname = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_register(lua_State *L) {
	int res;
	const uint8_t *ulabel = (const uint8_t*)luaL_optstring(L, 1, NULL);
	const uint8_t *alabel = (const uint8_t*)luaL_optstring(L, 2, NULL);
	int flags = optidn2_flags(L, 3, 0);
	uint8_t **insertname = new_boxed_pointer(L);
	res = idn2_register_u8(ulabel, alabel, insertname, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, (char*)*insertname);
	}
	idn2_free(*insertname);
	*insertname = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

#if IDN2_VERSION_NUMBER >= 0x02000000
static int luaidn2_to_ascii_lz(lua_State *L) {
	int res;
	const char *input = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **output = new_boxed_pointer(L);
	res = idn2_to_ascii_lz(input, output, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *output);
	}
	idn2_free(*output);
	*output = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_to_unicode_8zlz(lua_State *L) {
	int res;
	const char *input = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **output = new_boxed_pointer(L);
	res = idn2_to_unicode_8zlz(input, output, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *output);
	}
	idn2_free(*output);
	*output = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_to_unicode_lzlz(lua_State *L) {
	int res;
	const char *input = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **output = new_boxed_pointer(L);
	res = idn2_to_unicode_lzlz(input, output, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *output);
	}
	idn2_free(*output);
	*output = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}
#endif

static int luaidn2_lookup_ul(lua_State *L) {
	int res;
	const char *src = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 2, 0);
	char **lookupname = new_boxed_pointer(L);
	res = idn2_lookup_ul(src, lookupname, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *lookupname);
	}
	idn2_free(*lookupname);
	*lookupname = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_register_ul(lua_State *L) {
	int res;
	const char *ulabel = luaL_optstring(L, 1, NULL);
	const char *alabel = luaL_optstring(L, 2, NULL);
	int flags = optidn2_flags(L, 3, 0);
	char **insertname = new_boxed_pointer(L);
	res = idn2_register_ul(ulabel, alabel, insertname, flags);
	if (res == IDN2_OK) {
		lua_pushstring(L, *insertname);
	}
	idn2_free(*insertname);
	*insertname = NULL;
	if (res == IDN2_OK) {
		return 1;
	} else {
		return luaidn2_push_error(L, res);
	}
}

static int luaidn2_check_version(lua_State *L) {
	const char *req_version = luaL_optstring(L, 1, NULL);
	const char *res = idn2_check_version(req_version);
	lua_pushstring(L, res);
	return 1;
}

static int boxed_pointer__gc(lua_State *L) {
	void **ud = lua_touserdata(L, 1);
	idn2_free(*ud);
	*ud = NULL;
	return 0;
}

int luaopen_idn2(lua_State *L) {
	static const luaL_Reg lib_with_upval[] = {
#if IDN2_VERSION_NUMBER >= 0x02000000
		{"to_ascii", luaidn2_to_ascii},
		{"to_unicode", luaidn2_to_unicode},
#endif
		{"lookup", luaidn2_lookup},
		{"register", luaidn2_register},
#if IDN2_VERSION_NUMBER >= 0x02000000
		{"to_ascii_lz", luaidn2_to_ascii_lz},
		{"to_unicode_8zlz", luaidn2_to_unicode_8zlz},
		{"to_unicode_lzlz", luaidn2_to_unicode_lzlz},
#endif
		{"lookup_ul", luaidn2_lookup_ul},
		{"register_ul", luaidn2_register_ul},
		{NULL, NULL},
	};

#if LUA_VERSION_NUM >= 502
	luaL_checkversion(L);
#endif
	lua_createtable(L, 0, (sizeof(lib_with_upval)/sizeof(lib_with_upval[0]) - 1) + 5);

	lua_pushliteral(L, IDN2_VERSION);
	lua_setfield(L, -2, "VERSION");
	lua_pushinteger(L, IDN2_VERSION_NUMBER);
	lua_setfield(L, -2, "VERSION_NUMBER");
	lua_pushinteger(L, IDN2_LABEL_MAX_LENGTH);
	lua_setfield(L, -2, "LABEL_MAX_LENGTH");
	lua_pushinteger(L, IDN2_DOMAIN_MAX_LENGTH);
	lua_setfield(L, -2, "DOMAIN_MAX_LENGTH");

	lua_pushcfunction(L, luaidn2_check_version);
	lua_setfield(L, -2, "check_version");

	/* special metatable for boxed pointers that need to be freed */
	lua_createtable(L, 0, 1);
	lua_pushcfunction(L, boxed_pointer__gc);
	lua_setfield(L, -2, "__gc");
	luaL_setfuncs(L, lib_with_upval, 1);

	return 1;
}
