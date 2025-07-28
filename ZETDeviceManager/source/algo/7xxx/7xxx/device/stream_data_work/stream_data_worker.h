//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "../../../../object/object.hpp"
//----------------------------------------------------------------------------------------------------------------------
class stream_data_worker
{
public:
	stream_data_worker(const std::wstring& cfg_path);
	virtual ~stream_data_worker();

	bool is_stream_data_enable(const zetlab::object* object) const;
	bool is_stream_data(const zetlab::object* object) const;

	bool is_measure_data_enable(const zetlab::object* object) const;
	bool is_measure_data(const zetlab::object* object) const;

	void change_stream_data_state(const zetlab::object* object);
	void change_measure_data_state(const zetlab::object* object);

protected:
	const std::wstring m_cfg_path;
};
//----------------------------------------------------------------------------------------------------------------------