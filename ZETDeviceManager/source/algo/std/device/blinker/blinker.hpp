#pragma once
#ifndef blinker_hpp__
#define blinker_hpp__

#include "../device_std.hpp"

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

namespace zetlab { namespace std {

class device::blinker :
    public boost::noncopyable
{
public:
    blinker();
    ~blinker();

public:
    void start( long type, long dsp );

private:
    void blink_thread_func( long type, long dsp );

    const bool open( long type, long dsp );

    void close( long type, long dsp );

    void createSnapshot( long type, long dsp );

    void restoreSnapshot( long type, long dsp );

private:
    typedef struct _blink_struct {

        _blink_struct( const long n_channels_ ) :
            n_channels(n_channels_), step(0), param(0) {}

        long  n_channels;
        short step;
        short param;

    } blink_struct;

    void blink( long type, long dsp, blink_struct& blink_struct_ );

//     unsigned long blink_func_1( blink_struct& blink_struct_ );
    unsigned long blink_func_2( blink_struct& blink_struct_ );
//     unsigned long blink_func_3( blink_struct& blink_struct_ );

private:
    boost::thread blink_thread;

    long          n_channels;
    unsigned long snapshot_ch;
    unsigned long snapshot_icp;
	unsigned long snapshot_charge;
};

}} // namespace zetlab::std

#endif // blinker_hpp__
