#pragma once
#ifndef sensors_hpp__
#define sensors_hpp__

#include "../../types.hpp"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>

namespace zetlab {

class sensors :
    public boost::noncopyable
{
public:
    typedef struct _sensor
    {
        _sensor() :
            sense(double(0)),
            amplify(double(0)),
            icp(long(0)),
			charge(long(0)),
            reference(double(0)),
            shift(double(0)),
			hpf(uint16_t(0))
            {};

        tstring name;
        tstring unit;
		tstring unitSense;
        double  sense;
        double  amplify;
        long    icp;
		long    charge;
        double  reference;
        double  shift;
		uint16_t hpf;
        tstring afch;

    } sensor;

    typedef boost::shared_ptr< sensor > sensor_ptr;
    typedef ::std::vector< sensor_ptr > sensors_type;

public:
    sensors();

public:
    void load( const tstring& file_name );

    void save( const tstring& file_name );

    void add( const tstring& name );

    void remove( const size_t at );

    const sensors_type& get() const;

	void setName(const tstring& name, uint32_t index);

	void addPostfix(const tstring& name);

private:
    sensors_type sensors_;
};

} // namespace zetlab

#endif // sensors_hpp__
