package = "idn2"
version = "scm-0"
source = {
	url = "git+https://github.com/daurnimator/lua-idn2.git";
}
description = {
	summary = "Bindings to libidn2, an implementation of IDNA2008/TR46 internationalized domain names";
	homepage = "https://github.com/daurnimator/lua-idn2";
	license = "MIT";
}
dependencies = {}
external_dependencies = {
	IDN2 = {
		header = "idn2.h";
		library = "idn2";
	};
}
build = {
	type = "builtin";
	modules = {
		idn2 = {
			sources = "idn2/idn2.c";
			libraries = {
				"idn2";
			};
			defines = {
			};
			incdirs = {
				"$(IDN2_INCDIR)";
			};
			libdirs = {
				"$(IDN2_LIBDIR)";
			};
		};
	};
}
