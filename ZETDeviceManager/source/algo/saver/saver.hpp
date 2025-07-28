#pragma once
#ifndef saver_hpp__
#define saver_hpp__

#include "../../types.hpp"
#include "../factory/factory.hpp"

#include <pugixml/src/pugixml.hpp>

namespace zetlab {

class saver
{
public:
    saver( const factory_ptr& factory_ptr_ );

public:
    void save(
		const tstring& file_name );
	void backup(
		object* const object_,
		const tstring& file_name,
		tstring sConfigName);
	void restore(
		object* const object_,
		const tstring& file_name,
		const tstring& file_name_bak,
		tstring sConfigName);
	void get(
		const tstring& fileName,
		object* const object_,
		::std::vector<::std::pair<tstring, CZetTime>>& cfg);
	bool deleteConfig(
		const tstring& file_name,
		tstring sConfigName);
private:
	bool isDeviceChanged(
		object* const object_,
		pugi::xml_node& parent_ );

	bool isChannelChanged(
		object* const object_,
		pugi::xml_node& parent_ );

    void saveDevice(
		object* const object_,
        pugi::xml_node& parent_,
		tstring sConfigName = _T(""));
	
	void saveChannel( 
		object* const object_,
        pugi::xml_node& parent_ );

	void copyDevice(
		pugi::xml_node& src_,
		pugi::xml_node& dst_);

	void copyChannel(
		pugi::xml_node& src_,
		pugi::xml_node& dst_);

private:
    factory_ptr factory_;
};

} // namespace zetlab

#endif // saver_hpp__
