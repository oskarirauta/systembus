#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "common.hpp"
#include "kcmdline.hpp"

kcmdline::kcmdline(std::string path) {

	this -> _path = path;
	if ( !path.empty()) {
		std::fstream f(path, std::fstream::in);
		if ( !f.good() || !f.is_open())
			this -> _io_error = true;
		else {
			this -> _cmdline = "";
			char ch, prev = 0, quote;
			bool quoted = false;
			while ( f >> std::noskipws >> ch ) {
				if ( f.eof() || !f.good() || ch == std::char_traits<char>::eof()) break;
				if ( ch == '\r' || ch == '\n' || ( !quoted && ch == '\t'))
					ch = ' ';
				if ( !quoted && ch == ' ' && prev == ' ' )
					continue;
				if ( !quoted && ( ch == '\'' || ch == '"' )) {
					quoted = true;
					quote = ch;
				} else if ( quoted && ch == quote )
					quoted = false;
				this -> _cmdline += ch;
			}
			if ( !this -> _cmdline.empty()) {
				this -> _cmdline = common::trim_ws(this -> _cmdline);
				this -> parse();
			}
		}
		if ( f.is_open())
			f.close();
	} else this -> _io_error = true;
}

std::string kcmdline::path(void) {
	return this -> _path;
}

std::string kcmdline::cmdline(void) {
	return this -> _cmdline;
}

std::map<std::string, std::string> kcmdline::pairs(void) {
	return this -> _pairs;
}

bool kcmdline::io_error(void) {
	return this -> _io_error;
}

void kcmdline::parse(void) {

	bool is_name = true;
	bool escaping = false;
	char quote = 0;
	std::string key, val;

	for ( char& ch : this -> _cmdline ) {

		if ( !is_name && (
			( quote != 0 && !escaping && ch == quote ) ||
			( quote == 0 && ch == ' ' ))) {
			quote = 0;
			escaping = false;
			is_name = true;
			this -> _pairs[key] = val;
			key = "";
			val = "";
		} else if ( is_name && ch == ' ' ) {
			quote = 0;
			escaping = false;
			is_name = true;
			this -> _pairs[key] = "";
			key = "";
			val = "";
		} else if ( is_name && ch == '=' ) {
			quote = 0;
			escaping = false;
			is_name = false;
		} else if ( !is_name && val.empty() && ( ch == '\'' || ch == '"' )) {
			quote = ch;
		} else if ( !is_name && !escaping && ch == '\\' ) {
			escaping = true;
		} else {
			if ( !is_name && escaping && ch != '\'' && ch != '"' && ch != '\\' ) {
				escaping = false;
				val += '\\';
			}
			if ( is_name ) key += ch;
			else val += ch;
			escaping = false;
		}
	}

	if ( !key.empty())
		this -> _pairs[key] = val;
}
