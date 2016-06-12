#pragma once
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <map>
namespace drift
{
	struct variant;
	using variant_ptr = std::shared_ptr<variant>;
	using variant_unq = std::unique_ptr<variant>;
	typedef std::string string;
	typedef std::vector<variant_ptr> list;
	typedef std::function<variant_ptr(const list&)> function;
	typedef std::map<std::string, std::shared_ptr<variant>> table;
	typedef variant_ptr* reference;
	struct variant
	{
		struct null_kind {};
		union
		{
			unsigned char value_char;
			bool value_bool;
			long long value_int;
			double value_double;
			string value_string;
			list value_list;
			table value_table;
			function value_function;
			reference value_reference;
			null_kind value_null;
		};
		enum kind
		{
			kind_char,
			kind_bool,
			kind_int,
			kind_double,
			kind_string,
			kind_list,
			kind_table,
			kind_function,
			kind_reference,
			kind_null
		};
		kind variant_kind;
		variant() : value_null(null_kind()), variant_kind(kind_null) { }
		variant(unsigned char v) : value_char(v), variant_kind(kind_char) { }
		variant(bool v) : value_bool(v), variant_kind(kind_bool) { }
		variant(long long v) : value_int(v), variant_kind(kind_int) { }
		variant(double v) : value_double(v), variant_kind(kind_double) { }
		variant(const std::string& v) : value_string(v), variant_kind(kind_string) { }
		variant(const decltype(value_list)& v) : value_list(v), variant_kind(kind_list) { }
		variant(const decltype(value_table)& v) : value_table(v), variant_kind(kind_table) { }
		variant(function v) : value_function(v), variant_kind(kind_function) { }
		variant(reference v): value_reference(v), variant_kind(kind_reference) { }
		variant(null_kind v) : value_null(v), variant_kind(kind_null) { }
		~variant();

		static string kind_str(kind);
	};
	template <typename... Args>
	auto make_variant(Args&&... args) ->
	decltype(std::make_shared<variant>(std::forward<Args>(args)...))
	{
		return std::make_shared<variant>(std::forward<Args>(args)...);
	}

	variant_ptr operator+(variant_ptr, variant_ptr);
	variant_ptr operator*(variant_ptr, variant_ptr);
	variant_ptr operator-(variant_ptr, variant_ptr);
	variant_ptr operator/(variant_ptr, variant_ptr);
}