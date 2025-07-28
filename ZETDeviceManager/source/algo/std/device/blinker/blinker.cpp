#include "stdafx.h"

#include "blinker.hpp"

#include <adcinfo.h>
#include <Zadc.h>

namespace zetlab { namespace std {

device::blinker::blinker() :
    n_channels(0),
    snapshot_ch(0),
    snapshot_icp(0),
	snapshot_charge(0)
{
}

device::blinker::~blinker()
{
    blink_thread.interrupt();
    blink_thread.join();
}

void device::blinker::start( long type, long dsp )
{
    blink_thread = boost::thread( &blinker::blink_thread_func, this, type, dsp );
}

void device::blinker::blink_thread_func( long type, long dsp )
{
    try
    {
        if (!open( type, dsp ))
            return;

        createSnapshot( type, dsp );

        blink_struct blink_struct_(n_channels);

        while (true)
        {
            blink( type, dsp, blink_struct_ );

            boost::this_thread::sleep( boost::posix_time::milliseconds(50) );
        }
    }
    catch (boost::thread_interrupted&)
    {
        restoreSnapshot( type, dsp );
        close( type, dsp );
    }
}

const bool device::blinker::open( long type, long dsp )
{
    if (!ZOpen( type, dsp ))
    {
        boost::this_thread::sleep( boost::posix_time::milliseconds(100) );

        ZStartADC( type, dsp );

        return true;
    }

    return false;
}

void device::blinker::close( long type, long dsp )
{
    ZStopADC( type, dsp );
    ZClose( type, dsp );
}

void device::blinker::blink( long type, long dsp, blink_struct& blink_struct_ )
{
    if (KDU8500 == type || KDU2500 == type)
    {
        ZSetChannelADC( type, dsp, blink_func_2( blink_struct_ ) );
    }
    // else do nothing
}

/*unsigned long device::blinker::blink_func_1( blink_struct& blink_struct_ )
{
    unsigned long mask(0);

    mask |= (1 << blink_struct_.step++);

    if (blink_struct_.n_channels == blink_struct_.step)
        blink_struct_.step = 0;

    return (mask != 0) ? mask : 1;
}*/

unsigned long device::blinker::blink_func_2( blink_struct& blink_struct_ )
{
    unsigned long mask(0);

    mask |= (1 << blink_struct_.step);

    if (0 == blink_struct_.param)
        blink_struct_.param = 1;

    if (blink_struct_.n_channels == (blink_struct_.step + blink_struct_.param))
        blink_struct_.param = -1;
    else if (0 == blink_struct_.step)
        blink_struct_.param = 1;

    blink_struct_.step += blink_struct_.param;

    return (mask != 0) ? mask : 1;
}

/*unsigned long device::blinker::blink_func_3( blink_struct& blink_struct_ )
{
    unsigned long mask(0);

    mask |= (1 << 3);
    mask |= (1 << 4);

    short tmp = blink_struct_.step;
    while (tmp)
    {
        mask |= (1 << (3 - tmp));
        mask |= (1 << (4 + tmp));
        tmp--;
    }

    if (0 == blink_struct_.param)
        blink_struct_.param = 1;

    if (4 == (blink_struct_.step + blink_struct_.param))
        blink_struct_.param = -1;
    else if (-1 == (blink_struct_.step + blink_struct_.param))
        blink_struct_.param = 1;

    blink_struct_.step += blink_struct_.param;

    return (mask != 0) ? mask : 1;
}*/

void device::blinker::createSnapshot( long type, long dsp )
{
    ZGetQuantityChannelADC( type, dsp, &n_channels );

    for (long i = 0, tmp; i < n_channels; ++i)
    {
        ZGetInputADC( type, dsp, i, &tmp );
        snapshot_ch |= (tmp << i);
        ZGetHCPADC  ( type, dsp, i, &tmp );
        ZSetHCPADC  ( type, dsp, i, 0 );
        snapshot_icp|= (tmp << i);
		ZGetChargeADC(type, dsp, i, &tmp);
		ZSetChargeADC(type, dsp, i, 0);
		snapshot_charge |= (tmp << i);
    }
}

void device::blinker::restoreSnapshot( long type, long dsp )
{
    ZSetChannelADC( type, dsp, snapshot_ch );
    for (long i = 0; i < n_channels; ++i)
    {
        ZSetHCPADC( type, dsp, i, ((snapshot_icp >> i) & 1) );
		ZSetChargeADC(type, dsp, i, ((snapshot_charge >> i) & 1));
    }
}

}} // namespace zetlab::std
