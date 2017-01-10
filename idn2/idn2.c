#include <stddef.h> /* NULL */
#include <stdint.h> /* uint8_t */

#include <idn2.h>

#include <lua.h>
#include <lauxlib.h>

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
int lua_absindex (lua_State *L, int i) {
	if (i < 0 && i > LUA_REGISTRYINDEX)
		i += lua_gettop(L) + 1;
	return i;
}
#endif

static int luaidn2_push_error(lua_State *L, int rc) {
	lua_pushnil(L);
	lua_pushstring(L, idn2_strerror(rc));
	return 2;
}

int idn2_flags_list[] = { IDN2_NFC_INPUT, IDN2_ALABEL_ROUNDTRIP, IDN2_TRANSITIONAL, IDN2_NONTRANSITIONAL };
const char *idn2_flags_names[] = { "nfc_input", "alabel_roundtrip", "transitional", "nontransitional", NULL };
#define checkidn2_flag(L, idx) \
	(idn2_flags_list[luaL_checkoption((L), (idx), NULL, idn2_flags_names)])
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

static int luaidn2_lookup(lua_State *L) {
	int res;
	const uint8_t *src = (const uint8_t*)luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 3, 0);
	uint8_t **lookupname = lua_newuserdata(L, sizeof(uint8_t *));
	*lookupname = NULL;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
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
	uint8_t **insertname = lua_newuserdata(L, sizeof(uint8_t *));
	*insertname = NULL;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
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

static int luaidn2_lookup_ul(lua_State *L) {
	int res;
	const char *src = luaL_checkstring(L, 1);
	int flags = optidn2_flags(L, 3, 0);
	char **lookupname = lua_newuserdata(L, sizeof(char *));
	*lookupname = NULL;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
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
	char **insertname = lua_newuserdata(L, sizeof(char *));
	*insertname = NULL;
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setmetatable(L, -2);
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
	lua_createtable(L, 0, 5);

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
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, luaidn2_lookup, 1);
	lua_setfield(L, -3, "lookup");
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, luaidn2_register, 1);
	lua_setfield(L, -3, "register");
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, luaidn2_lookup_ul, 1);
	lua_setfield(L, -3, "lookup_ul");
	lua_pushcclosure(L, luaidn2_register_ul, 1);
	lua_setfield(L, -2, "register_ul");

	return 1;
}
