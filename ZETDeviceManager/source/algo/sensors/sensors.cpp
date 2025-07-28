#include "stdafx.h"

#include "sensors.hpp"

#include <pugixml/auto_link.h>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <resource/Phrases.h>

namespace zetlab {

sensors::sensors()
{
}

void sensors::load( const tstring& file_name )
{
	pugi::xml_document xml;

	// Load file
	if (!xml.load_file( file_name.c_str(), pugi::parse_default, pugi::encoding_utf8 ))
		return;

	pugi::xml_node root = xml.first_child();

	sensors_.clear();


	for (auto it = root.begin(), end = root.end(); it != end; ++it)
	{
		sensor_ptr sensor_ptr_(new sensor());

		try
		{
			sensor_ptr_->name      = tstring(it->attribute( _T("name") ).value());
			CString s = CString(it->attribute(_T("unit")).value());
			sensor_ptr_->unit = tstring(TranslateString(s).GetString());
			//sensor_ptr_->unit      = tstring(it->attribute( _T("unit") ).value());
			sensor_ptr_->sense     = boost::lexical_cast< double >( it->child( _T("Sense") )		.first_child().value() );
			sensor_ptr_->amplify   = boost::lexical_cast< double >( it->child( _T("Amplify") )		.first_child().value() );
			sensor_ptr_->icp       = boost::lexical_cast< long   >( it->child( _T("Preamplifier") )	.first_child().value() );
			sensor_ptr_->reference = boost::lexical_cast< double >( it->child( _T("Reference") )	.first_child().value() );
			sensor_ptr_->shift     = boost::lexical_cast< double >( it->child( _T("Shift") )		.first_child().value() );
			sensor_ptr_->afch      = tstring(it->child( _T("AFCH") )								.first_child().value());
		}
		catch (boost::bad_lexical_cast&)
		{
			continue;
		}

		sensors_.push_back( sensor_ptr_ );


		if (!it->child(_T("HPF")).first_child().empty())
		{
			try
			{
				sensors_.back()->hpf = boost::lexical_cast<uint16_t>(it->child(_T("HPF")).first_child().value());
			}
			catch (boost::bad_lexical_cast&)
			{
				sensors_.back()->hpf = 0;
			}
		}
		else
			sensors_.back()->hpf = 0;

		if (!it->child(_T("Charge")).first_child().empty())
		{
			try
			{
				sensors_.back()->charge = boost::lexical_cast<long>(it->child(_T("Charge")).first_child().value());
			}
			catch (boost::bad_lexical_cast&)
			{
				sensors_.back()->charge = 0;
			}
		}
		else
			sensors_.back()->charge = 0;

		if (!it->attribute(_T("unitsense")).empty())
		{
			try
			{
				CString s = CString(it->attribute(_T("unitsense")).value());
				sensors_.back()->unitSense = tstring(TranslateString(s).GetString());
				//sensors_.back()->unitSense = tstring(it->attribute(_T("unitsense")).value());
			}
			catch (boost::bad_lexical_cast&)
			{
				sensors_.back()->unitSense = g_sUNIT_V;
			}
		}
		else
			sensors_.back()->unitSense = g_sUNIT_V;
	}
}

void sensors::save( const tstring& file_name )
{
	pugi::xml_document xml;

	pugi::xml_node root = xml.append_child( _T("Sensors") );

	boost::range::for_each( sensors_, [&]( sensor_ptr& sensor_ ) {
		pugi::xml_node s = root.append_child( _T("Sensor") );

		s.append_attribute( _T("name") ).set_value( sensor_->name.c_str() );
		s.append_attribute( _T("unit") ).set_value( sensor_->unit.c_str() );
		s.append_attribute( _T("unitsense") ).set_value( sensor_->unitSense.c_str() );
		s.append_child( _T("Sense") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->sense ).c_str() );
		s.append_child( _T("Amplify") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->amplify ).c_str() );
		s.append_child( _T("Preamplifier") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->icp ).c_str() );
		s.append_child( _T("Charge") ).append_child(pugi::node_pcdata).set_value(boost::lexical_cast< tstring >( sensor_->charge ).c_str());
		s.append_child( _T("Reference") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->reference ).c_str() );
		s.append_child( _T("Shift") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->shift ).c_str() );
		s.append_child( _T("AFCH") ).append_child( pugi::node_pcdata ).set_value( boost::lexical_cast< tstring >( sensor_->afch.c_str() ).c_str() );
		s.append_child(_T("HPF")).append_child(pugi::node_pcdata).set_value(boost::lexical_cast< tstring >(sensor_->hpf).c_str());
	});

	// Save changes
	xml.save_file( file_name.c_str(), PUGIXML_TEXT("\t"), pugi::format_default, pugi::encoding_utf8 );
}

void sensors::add( const tstring& name )
{
	sensor_ptr sensor_ptr_(new sensor());
	
	sensor_ptr_->name = name;
	sensors_.push_back( sensor_ptr_ );
}

void sensors::remove( const size_t at )
{
	sensors_.erase( sensors_.begin() + at );
}

const sensors::sensors_type& sensors::get() const
{
	return sensors_;
}

void sensors::setName(const tstring& name,uint32_t index)
{
	if (sensors_.size() > index)
		sensors_[index]->name = name;
}

void sensors::addPostfix(const tstring& name)
{
	sensor_ptr sensor_ptr_(new sensor());
	boost::range::for_each(sensors_, [&](sensor_ptr& sensor_) {
		sensor_->name += name;
	});

}

} // namespace zetlab
