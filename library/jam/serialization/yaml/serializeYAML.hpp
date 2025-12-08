#pragma once
#ifndef YAML_CPP_STATIC_DEFINE
	#define YAML_CPP_STATIC_DEFINE  //if not included yaml will look for a .dll
#endif
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <filesystem>
#include <raylib.h>
#include "Jam/core/uuid.hpp"
#include "Jam/core/iVec2.hpp"

#include <yaml-cpp/yaml.h>
namespace YAML
{
	template<>
	struct convert<Vector2>
	{
		static Node encode(const Vector2& rhs) { Node node; node.push_back(rhs.x); node.push_back(rhs.y); return node; }
		static bool decode(const Node& node, Vector2& rhs) {
			if (!node.IsSequence() || node.size() < 2) return false;
			rhs.x = node[0].as<float>(); rhs.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Vector3>
	{
		static Node encode(const Vector3& rhs) { Node node; node.push_back(rhs.x); node.push_back(rhs.y); node.push_back(rhs.z); return node; }
		static bool decode(const Node& node, Vector3& rhs) {
			if (!node.IsSequence() || node.size() < 3) return false;
			rhs.x = node[0].as<float>(); rhs.y = node[1].as<float>(); rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Vector4>
	{
		static Node encode(const Vector4& rhs) { Node node; node.push_back(rhs.x); node.push_back(rhs.y); node.push_back(rhs.z); node.push_back(rhs.w); return node; }
		static bool decode(const Node& node, Vector4& rhs) {
			if (!node.IsSequence() || node.size() < 4) return false;
			rhs.x = node[0].as<float>(); rhs.y = node[1].as<float>(); rhs.z = node[2].as<float>(); rhs.w = node[3].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Rectangle>
	{
		static Node encode(const Rectangle& rhs) { Node node; node.push_back(rhs.x); node.push_back(rhs.y); node.push_back(rhs.width); node.push_back(rhs.height); return node; }
		static bool decode(const Node& node, Rectangle& rhs) {
			if (!node.IsSequence() || node.size() < 4) return false;
			rhs.x = node[0].as<float>(); rhs.y = node[1].as<float>(); rhs.width = node[2].as<float>(); rhs.height = node[3].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Color>
	{
		static Node encode(const Color& rhs) {
			Node node;
			std::stringstream hexValue;
			hexValue.imbue(std::locale("C"));
			hexValue << std::uppercase << std::hex << ColorToInt(rhs);

			node.push_back(hexValue.str());

			return node;
		}
		static bool decode(const Node& node, Color& rhs) {
			if (!node.IsScalar())
				return false;
			unsigned int hexValue;
			std::string s = node.as<std::string>();
			std::istringstream converter(s);
			converter >> std::hex >> hexValue;
			rhs = GetColor(hexValue);

			return true;
		}
	};


	template<>
	struct convert<NPatchInfo>
	{
		static Node encode(const NPatchInfo& rhs)
		{
			Node node;
			node.push_back(rhs.source.x);
			node.push_back(rhs.source.y);
			node.push_back(rhs.source.width);
			node.push_back(rhs.source.height);
			node.push_back(rhs.top);
			node.push_back(rhs.left);
			node.push_back(rhs.bottom);
			node.push_back(rhs.right);
			node.push_back(rhs.layout);


			return node;
		}
		static bool decode(const Node& node, NPatchInfo& rhs) {
			if (!node.IsSequence() || node.size() < 9)
				return false;
			int i = 0;
			rhs.source.x = node[i++].as<float>();
			rhs.source.y = node[i++].as<float>();
			rhs.source.width = node[i++].as<float>();
			rhs.source.height = node[i++].as<float>();
			rhs.top = node[i++].as<int>();
			rhs.left = node[i++].as<int>();
			rhs.bottom = node[i++].as<int>();
			rhs.right = node[i++].as<int>();
			rhs.layout = node[i++].as<int>();
			return true;
		}
	};

	template<>
	struct convert<jam::iVec2>
	{
		static Node encode(const jam::iVec2& rhs) { Node node; node.push_back(rhs.x); node.push_back(rhs.y); return node; }
		static bool decode(const Node& node, jam::iVec2& rhs) {
			if (!node.IsSequence() || node.size() < 2) return false;
			rhs.x = node[0].as<float>(); rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<jam::UUID>
	{
		static Node encode(const jam::UUID& rhs) {
			Node node;
			std::stringstream hexValue;
			hexValue.imbue(std::locale("C"));
			hexValue << std::uppercase << std::hex << rhs.toUint64();

			node.push_back(hexValue.str());

			return node;
		}
		static bool decode(const Node& node, jam::UUID& rhs) {
			if (!node.IsScalar())
				return false;
			uint64_t hexValue;
			std::string s = node.as<std::string>();
			std::istringstream converter(s);
			converter >> std::hex >> hexValue;
			rhs = (hexValue);

			return true;
		}
	};

}


inline YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
{
	out << YAML::Flow << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}
inline YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
{
	out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
{
	out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}
inline YAML::Emitter& operator<<(YAML::Emitter& out, const Rectangle& v)
{
	out << YAML::Flow << YAML::BeginSeq << v.x << v.y << v.width << v.height << YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const Color& v)
{
	std::stringstream hexValue;
	hexValue.imbue(std::locale("C"));
	hexValue << std::uppercase << std::hex << ColorToInt(v);
	out << hexValue.str();
	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const NPatchInfo& v)
{
	out << YAML::Flow << YAML::BeginSeq
		<< v.source.x << v.source.y << v.source.width << v.source.height
		<< v.top << v.left << v.bottom << v.right << v.layout
		<< YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const jam::iVec2& v)
{
	out << YAML::Flow << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& out, const jam::UUID& v)
{
	std::stringstream hexValue;
	hexValue.imbue(std::locale("C"));
	hexValue << std::uppercase << std::hex << v;
	out << hexValue.str();
	return out;
}


namespace jam
{
	inline YAML::Node LoadYamlFile(std::string fileLocation)
	{
		std::filesystem::path fp = fileLocation;

		if (!std::filesystem::exists(fp))
			return YAML::Node();

		std::ifstream stream(fp);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node root = YAML::Load(strStream.str());

		return root;
	}

	inline void SaveYamlFile(std::string fileLocation, YAML::Emitter& out)
	{
		std::ofstream fileout(fileLocation);

		fileout << out.c_str();
	}

	template <typename T>
	inline bool readValue(YAML::Node node, const char* name, T* value)
	{
		if (!node[name])
			return false;

		*value = node[name].as<T>();
		return true;
	}

	template <typename T>
	inline bool readValueEx(YAML::Node node, T* value)
	{
		if (!node)
			return false;

		*value = node.as<T>();
		return true;
	}
}

