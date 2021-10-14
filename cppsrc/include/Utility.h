#include <string>
#include <fstream>
#include "Result.h"
#include "ParserInclude.h"
#include "Constants.h"

Result ReadTextFile(const std::string& path, std::string& result) {
	std::ifstream file(path);
	if (file.is_open()) {
		std::string buf;
		buf.reserve(50);
		result.reserve(50);
		while (std::getline(file, result)) {}
		file.close();
		return Result::Success;
	}
	else {
		return Result::Error;
	}
}

Configure ParseJsonToConfig(const std::string& json) {
	rapidjson::Document d;
	d.Parse(json.c_str());

	assert(d.HasMember("n"));
	assert(d["n"].IsInt());
	assert(d.HasMember("m"));
	assert(d["m"].IsInt());
	assert(d.HasMember("port"));
	assert(d["port"].IsInt());
	assert(d.HasMember("ip"));
	assert(d["ip"].IsString());

	return Configure(d["n"].GetInt(), d["m"].GetInt(), d["port"].GetInt(), d["ip"].GetString());
}
