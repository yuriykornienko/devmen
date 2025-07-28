#pragma once

#include <iomanip>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

namespace zetlab {

enum class zsp_device_type : uint32_t
{
    unknown = 0,
    zet030,
    zet211,
    zet221,
    zet037,
    zet048,
};
enum class zsp_device_modification : uint32_t
{
    unknown = 0,
    a,
    t,
};
using zsp_device_type_ex = ::std::pair<zsp_device_type, zsp_device_modification>;
using zsp_device_id = ::std::pair<zsp_device_type, uint64_t>;

enum class zsp_tenso_scheme : uint32_t
{
    unknown = 0,
    full_4w_i,
    full_4w_u,
    full_6w_u,
    half_3w_i,
    half_3w_u,
    half_5w_u,
    quarter_r1_i,
    quarter_r1_u,
    quarter_r2_i,
    quarter_r2_u,
    quarter_r3_i,
    quarter_r3_u,
    quarter_r4_i,
    quarter_r4_u,
    tc_u,
    tr_i,
    lvdt_i,
    lvdt_u,
    diff_t,
    single_t,
    current_t,
};
static const std::map<std::wstring, zsp_tenso_scheme> g_tenso_schemes =
{
    { L"unknown",		zsp_tenso_scheme::unknown},
    { L"full_4wire_i",	zsp_tenso_scheme::full_4w_i },
    { L"full_4wire_u",	zsp_tenso_scheme::full_4w_u },
    { L"full_6wire_u",	zsp_tenso_scheme::full_6w_u },
    { L"half_3wire_i",	zsp_tenso_scheme::half_3w_i },
    { L"half_3wire_u",	zsp_tenso_scheme::half_3w_u },
    { L"half_5wire_u",	zsp_tenso_scheme::half_5w_u },
    { L"quarter_r1_i",	zsp_tenso_scheme::quarter_r1_i },
    { L"quarter_r1_u",	zsp_tenso_scheme::quarter_r1_u },
    { L"quarter_r2_i",	zsp_tenso_scheme::quarter_r2_i },
    { L"quarter_r2_u",	zsp_tenso_scheme::quarter_r2_u },
    { L"quarter_r3_i",	zsp_tenso_scheme::quarter_r3_i },
    { L"quarter_r3_u",	zsp_tenso_scheme::quarter_r3_u },
    { L"quarter_r4_i",	zsp_tenso_scheme::quarter_r4_i },
    { L"quarter_r4_u",	zsp_tenso_scheme::quarter_r4_u },
    { L"tc_u",			zsp_tenso_scheme::tc_u },
    { L"tr_i",			zsp_tenso_scheme::tr_i },
    { L"lvdt_i",		zsp_tenso_scheme::lvdt_i },
    { L"lvdt_u",		zsp_tenso_scheme::lvdt_u },
    { L"diff_t",		zsp_tenso_scheme::diff_t },
    { L"single_t",		zsp_tenso_scheme::single_t },
    { L"current_t",		zsp_tenso_scheme::current_t },
};

enum class zsp_tenso_sensor : uint32_t
{
    unknown = 0,
    resistor,
    sensor,
    lvdt,
    tr,
    tc,
    diff_u,
    single_u,
    current,
    user,
};
static const ::std::map<::std::wstring, zsp_tenso_sensor> g_tenso_sensors =
{
    { L"unknown", zsp_tenso_sensor::unknown},
    { L"resistor", zsp_tenso_sensor::resistor },
    { L"sensor", zsp_tenso_sensor::sensor },
    { L"lvdt", zsp_tenso_sensor::lvdt },
    { L"tr", zsp_tenso_sensor::tr },
    { L"tc", zsp_tenso_sensor::tc },
    { L"diff_u", zsp_tenso_sensor::diff_u },
    { L"single_u", zsp_tenso_sensor::single_u },
    { L"current", zsp_tenso_sensor::current },
    { L"user", zsp_tenso_sensor::user },
};

enum class zsp_tenso_resistor_mode : uint32_t
{
    unknown = 0,
    
    quarter_tension_compression_1,
    quarter_tension_compression_2,
    quarter_tension_compression_3,
    quarter_bending,

    half3_tension_compression_1,
    half3_tension_compression_2,
    half3_tension_compression_3,
    half3_tension_compression_4,
    half3_bending_1,
    half3_bending_2,
    half3_torsion,

    half5_tension_compression_1,
    half5_tension_compression_2,
    half5_tension_compression_3,
    half5_tension_compression_4,
    half5_bending_1,
    half5_bending_2,
    half5_torsion,

    full4_tension_compression_1,
    full4_tension_compression_2,
    full4_tension_compression_3,
    full4_tension_compression_4,
    full4_tension_compression_5,
    full4_tension_compression_6,
    full4_tension_compression_7,
    full4_tension_compression_8,
    full4_bending_1,
    full4_bending_2,
    full4_bending_3,
    full4_torsion_1,
    full4_torsion_2,

    full6_tension_compression_1,
    full6_tension_compression_2,
    full6_tension_compression_3,
    full6_tension_compression_4,
    full6_tension_compression_5,
    full6_tension_compression_6,
    full6_tension_compression_7,
    full6_tension_compression_8,
    full6_bending_1,
    full6_bending_2,
    full6_bending_3,
    full6_torsion_1,
    full6_torsion_2,
};
static const ::std::map<::std::wstring, zsp_tenso_resistor_mode> g_tenso_resistor_modes =
{
    { L"unknown", zsp_tenso_resistor_mode::unknown},
    { L"quarter_tension_compression_1", zsp_tenso_resistor_mode::quarter_tension_compression_1 },
    { L"quarter_tension_compression_2", zsp_tenso_resistor_mode::quarter_tension_compression_2 },
    { L"quarter_tension_compression_3", zsp_tenso_resistor_mode::quarter_tension_compression_3 },
    { L"quarter_bending", zsp_tenso_resistor_mode::quarter_bending },
    { L"half3_tension_compression_1", zsp_tenso_resistor_mode::half3_tension_compression_1 },
    { L"half3_tension_compression_2", zsp_tenso_resistor_mode::half3_tension_compression_2 },
    { L"half3_tension_compression_3", zsp_tenso_resistor_mode::half3_tension_compression_3 },
    { L"half3_tension_compression_4", zsp_tenso_resistor_mode::half3_tension_compression_4 },
    { L"half3_bending_1", zsp_tenso_resistor_mode::half3_bending_1 },
    { L"half3_bending_2", zsp_tenso_resistor_mode::half3_bending_2 },
    { L"half3_torsion", zsp_tenso_resistor_mode::half3_torsion },
    { L"half5_tension_compression_1", zsp_tenso_resistor_mode::half5_tension_compression_1 },
    { L"half5_tension_compression_2", zsp_tenso_resistor_mode::half5_tension_compression_2 },
    { L"half5_tension_compression_3", zsp_tenso_resistor_mode::half5_tension_compression_3 },
    { L"half5_tension_compression_4", zsp_tenso_resistor_mode::half5_tension_compression_4 },
    { L"half5_bending_1", zsp_tenso_resistor_mode::half5_bending_1 },
    { L"half5_bending_2", zsp_tenso_resistor_mode::half5_bending_2 },
    { L"half5_torsion", zsp_tenso_resistor_mode::half5_torsion },
    { L"full4_tension_compression_1", zsp_tenso_resistor_mode::full4_tension_compression_1 },
    { L"full4_tension_compression_2", zsp_tenso_resistor_mode::full4_tension_compression_2 },
    { L"full4_tension_compression_3", zsp_tenso_resistor_mode::full4_tension_compression_3 },
    { L"full4_tension_compression_4", zsp_tenso_resistor_mode::full4_tension_compression_4 },
    { L"full4_tension_compression_5", zsp_tenso_resistor_mode::full4_tension_compression_5 },
    { L"full4_tension_compression_6", zsp_tenso_resistor_mode::full4_tension_compression_6 },
    { L"full4_tension_compression_7", zsp_tenso_resistor_mode::full4_tension_compression_7 },
    { L"full4_tension_compression_8", zsp_tenso_resistor_mode::full4_tension_compression_8 },
    { L"full4_bending_1", zsp_tenso_resistor_mode::full4_bending_1 },
    { L"full4_bending_2", zsp_tenso_resistor_mode::full4_bending_2 },
    { L"full4_bending_3", zsp_tenso_resistor_mode::full4_bending_3 },
    { L"full4_torsion_1", zsp_tenso_resistor_mode::full4_torsion_1 },
    { L"full4_torsion_2", zsp_tenso_resistor_mode::full4_torsion_2 },
    { L"full6_tension_compression_1", zsp_tenso_resistor_mode::full6_tension_compression_1 },
    { L"full6_tension_compression_2", zsp_tenso_resistor_mode::full6_tension_compression_2 },
    { L"full6_tension_compression_3", zsp_tenso_resistor_mode::full6_tension_compression_3 },
    { L"full6_tension_compression_4", zsp_tenso_resistor_mode::full6_tension_compression_4 },
    { L"full6_tension_compression_5", zsp_tenso_resistor_mode::full6_tension_compression_5 },
    { L"full6_tension_compression_6", zsp_tenso_resistor_mode::full6_tension_compression_6 },
    { L"full6_tension_compression_7", zsp_tenso_resistor_mode::full6_tension_compression_7 },
    { L"full6_tension_compression_8", zsp_tenso_resistor_mode::full6_tension_compression_8 },
    { L"full6_bending_1", zsp_tenso_resistor_mode::full6_bending_1 },
    { L"full6_bending_2", zsp_tenso_resistor_mode::full6_bending_2 },
    { L"full6_bending_3", zsp_tenso_resistor_mode::full6_bending_3 },
    { L"full6_torsion_1", zsp_tenso_resistor_mode::full6_torsion_1 },
    { L"full6_torsion_2", zsp_tenso_resistor_mode::full6_torsion_2 },
};

enum class zsp_tr_nsh : uint32_t
{
    unknown = 0,
    
    nsh_100p,
    nsh_50p,
    nsh_pt1000,
    nsh_pt100,
    nsh_pt50,
    nsh_100m,
    nsh_50m,
    nsh_cu100,
    nsh_cu50,
    nsh_100n,
    nsh_50n,
}; 

static const ::std::map<::std::wstring, zsp_tr_nsh> g_tr_nshs =
{
    { L"unknown", zsp_tr_nsh::unknown},
    { L"nsh_100p", zsp_tr_nsh::nsh_100p },
    { L"nsh_50p", zsp_tr_nsh::nsh_50p },
    { L"nsh_pt100", zsp_tr_nsh::nsh_pt100 },
    { L"nsh_pt1000", zsp_tr_nsh::nsh_pt1000 },
    { L"nsh_pt50", zsp_tr_nsh::nsh_pt50 },
    { L"nsh_100m", zsp_tr_nsh::nsh_100m },
    { L"nsh_50m", zsp_tr_nsh::nsh_50m },
    { L"nsh_cu100", zsp_tr_nsh::nsh_cu100 },
    { L"nsh_cu50", zsp_tr_nsh::nsh_cu50 },
    { L"nsh_100n", zsp_tr_nsh::nsh_100n },
    { L"nsh_50n", zsp_tr_nsh::nsh_50n },
};

enum class zsp_tc_type : uint32_t
{
    unknown = 0,

    type_r,
    type_s,
    type_b,
    type_j,
    type_t,
    type_e,
    type_k,
    type_n,
    type_a1,
    type_a2,
    type_a3,
    type_l,
    type_m,
};

static const ::std::map<::std::wstring, zsp_tc_type> g_tc_types =
{
    { L"unknown", zsp_tc_type::unknown},
    { L"type_r", zsp_tc_type::type_r },
    { L"type_s", zsp_tc_type::type_s },
    { L"type_b", zsp_tc_type::type_b },
    { L"type_j", zsp_tc_type::type_j },
    { L"type_t", zsp_tc_type::type_t },
    { L"type_e", zsp_tc_type::type_e },
    { L"type_k", zsp_tc_type::type_k },
    { L"type_n", zsp_tc_type::type_n },
    { L"type_a1", zsp_tc_type::type_a1 },
    { L"type_a2", zsp_tc_type::type_a2 },
    { L"type_a3", zsp_tc_type::type_a3 },
    { L"type_l", zsp_tc_type::type_l },
    { L"type_m", zsp_tc_type::type_m },
};

static inline zsp_device_type get_device_type(const uint32_t type)
{
    switch (type)
    {
    case 30: return zsp_device_type::zet030;
    case 37: return zsp_device_type::zet037;
    case 48: return zsp_device_type::zet048;
    case 211: return zsp_device_type::zet211;
    case 221: return zsp_device_type::zet221;
    default:
        break;
    }

    return zsp_device_type::unknown;
}

static inline ::std::wstring get_device_name(const zsp_device_type device_type)
{
    switch (device_type)
    {
    case zsp_device_type::zet030: return L"ZET 030";
    case zsp_device_type::zet211: return L"ZET 211";
    case zsp_device_type::zet221: return L"ZET 221";
    case zsp_device_type::zet037: return L"ZET 037";
    case zsp_device_type::zet048: return L"ZET 048";
    default:
        break;
    }

    return {};
}

static inline ::std::wstring get_file_name(const zsp_device_type type)
{
    switch (type)
    {
    case zsp_device_type::zet030: return L"zet030";
    case zsp_device_type::zet211: return L"zet211";
    case zsp_device_type::zet221: return L"zet221";
    case zsp_device_type::zet037: return L"zet037";
    case zsp_device_type::zet048: return L"zet048";
    default:
        break;
    }

    return {};
}

static inline double get_gain(zsp_device_type_ex device_type, uint32_t i)
{
    switch (device_type.first)
    {
    case zsp_device_type::zet030: return i > 0 ? 30. : 1.;
    case zsp_device_type::zet211: return 1.;
    case zsp_device_type::zet221: return 1.;
    case zsp_device_type::zet037:
        if (device_type.second == zsp_device_modification::a)
            return i > 0 ? 30. : 1.;
        break;
    case zsp_device_type::zet048: return i == 0 ? 1. : (i == 1 ? 4. : (i == 2 ? 8. : (i == 3 ? 12. : 1.)));
    default:
        break;
    }

    return 1.;
}

static inline uint32_t get_gain(zsp_device_type_ex device_type, double g)
{
    switch (device_type.first)
    {
    case zsp_device_type::zet030: return g > 10. ? 1 : 0;
    case zsp_device_type::zet211: return 0;
    case zsp_device_type::zet221: return 0;
    case zsp_device_type::zet037:
        if (device_type.second == zsp_device_modification::a)
            return g > 10. ? 1 : 0;
        break;
    case zsp_device_type::zet048: return g < 2.5 ? 0 : (g < 6. ? 1 : (g < 10. ? 2 : (g < 20. ? 3 : 1)));
    default:
        break;
    }

    return 0;
}

static inline ::std::vector<double> get_amplifies_adc(zsp_device_type_ex device_type)
{
    switch (device_type.first)
    {
    case zsp_device_type::zet030: return { 1., 30.};
    case zsp_device_type::zet211: return {};
    case zsp_device_type::zet221: return {};
    case zsp_device_type::zet037:
        if (device_type.second == zsp_device_modification::a)
            return { 1., 30. };
        else if (device_type.second == zsp_device_modification::t)
            return { 1., 8., 32., 128. };
        break;
    case zsp_device_type::zet048: return { 1., 4., 8., 12. };
    default:
        break;
    }

    return {};
}

static inline ::std::vector<uint32_t> get_frequencies_adc(zsp_device_type_ex device_type)
{
    switch (device_type.first)
    {
    case zsp_device_type::zet030: return { 1000, 3125, 6250, 12500, 25000, 50000, 100000, 200000, 400000 };
    case zsp_device_type::zet211: return { 100, 1000, 2500, 5000, 10000, 25000, 50000, 100000, 500000 };
    case zsp_device_type::zet221: return { 500, 1000, 2000, 4000, 8000 };
    case zsp_device_type::zet037:
        if (device_type.second == zsp_device_modification::a)
            return { 1000, 3125, 6250, 12500, 25000, 50000, 100000, 200000, 400000 };
        else if (device_type.second == zsp_device_modification::t)
            return { 100, 1000, 5000 };
        break;
    case zsp_device_type::zet048: return { 500, 1000, 2000, 4000 };
    default:
        break;
    }

    return {};
}

static inline ::std::vector<uint32_t> get_frequencies_dac(zsp_device_type device_type)
{
    switch (device_type)
    {
    case zsp_device_type::zet030: return {};
    case zsp_device_type::zet211: return { 100, 1000, 2500, 5000, 10000, 25000, 50000, 100000, 500000 };
    case zsp_device_type::zet221: return { 2000, 20000, 200000 };
    case zsp_device_type::zet037: return { 50000, 100000, 200000 };
    case zsp_device_type::zet048: return {};
    default:
        break;
    }

    return {};
}

static inline ::std::list<::std::pair<::std::wstring, ::std::wstring>> get_selftest_list(zsp_device_type_ex device_type)
{
    switch (device_type.first)
    {
    case zsp_device_type::zet030: 
        return 
        {
            { L"off",   L"Отключен" },
            { L"sqr",   L"Меандр"},
            { L"short", L"50 Ом"},
            { L"neg",   L"Минус"}
        };
    case zsp_device_type::zet211:
        return {};
    case zsp_device_type::zet221:
        return {};
    case zsp_device_type::zet037:
        if (device_type.second == zsp_device_modification::a)
            return
            {
                { L"off",   L"Отключен" },
                { L"sqr",   L"Меандр"},
                { L"short", L"50 Ом"},
                { L"neg",   L"Минус"}
            };
        else if (device_type.second == zsp_device_modification::t)
            return
            {
                { L"off",   L"Отключен" },
                { L"dc1",   L"DC 1"},
                { L"dc2",   L"DC 2"},
                { L"short", L"50 Ом"},
                { L"gnd",   L"GND"},
                { L"sqr1",  L"Меандр 1"},
                { L"sqr2",  L"Меандр 2"}
            };
        break;
    case zsp_device_type::zet048:
        return
        {
            { L"off",  L"Отключен" },
            { L"on",   L"Включен"},
            { L"auto", L"Меандр"},
        };
    default:
        break;
    }

    return {};
}

static uint16_t get_bits_digital_port(zsp_device_type device_type)
{
    switch (device_type)
    {
    case zsp_device_type::zet211: return 8;
    case zsp_device_type::zet221: return 8;
    default:
        break;
    }

    return 0;
}

static inline ::std::wstring get_file_name(const zsp_device_id& id)
{
    ::std::wstring file_name = get_file_name(id.first);
    if (file_name.empty())
        return {};
    
    file_name.append(L"_");
    file_name.append(::std::to_wstring(id.second));

    return file_name;
}

static inline zsp_tenso_scheme get_tenso_scheme(const ::std::wstring& scheme)
{
    if (auto it_scheme = g_tenso_schemes.find(scheme); it_scheme != g_tenso_schemes.end())
        return it_scheme->second;

    return zsp_tenso_scheme::unknown;
}

static inline ::std::wstring get_tenso_scheme(const zsp_tenso_scheme& scheme)
{
    if (auto it_scheme = std::find_if(g_tenso_schemes.begin(), g_tenso_schemes.end(), [&scheme](const auto& tenso_scheme)
        { return tenso_scheme.second == scheme; }); it_scheme != g_tenso_schemes.end())
        return it_scheme->first;

    return {};
}

static inline zsp_tenso_sensor get_tenso_sensor(const ::std::wstring& sensor)
{
    if (auto it_sensor = g_tenso_sensors.find(sensor); it_sensor != g_tenso_sensors.end())
        return it_sensor->second;

    return zsp_tenso_sensor::unknown;
}

static inline ::std::wstring get_tenso_sensor(const zsp_tenso_sensor& sensor)
{
    if (auto it_sensor = std::find_if(g_tenso_sensors.begin(), g_tenso_sensors.end(), [&sensor](const auto& tenso_sensor)
        { return tenso_sensor.second == sensor; }); it_sensor != g_tenso_sensors.end())
        return it_sensor->first;

    return {};
}

static inline zsp_tenso_resistor_mode get_tenso_resistor_mode(const ::std::wstring& resistor_mode)
{
    if (auto it_resistor_mode = g_tenso_resistor_modes.find(resistor_mode); it_resistor_mode != g_tenso_resistor_modes.end())
        return it_resistor_mode->second;

    return zsp_tenso_resistor_mode::unknown;
}

static inline ::std::wstring get_tenso_resistor_mode(const zsp_tenso_resistor_mode& resistor_mode)
{
    if (auto it_resistor_mode = std::find_if(g_tenso_resistor_modes.begin(), g_tenso_resistor_modes.end(), [&resistor_mode](const auto& tenso_resistor_mode)
        { return tenso_resistor_mode.second == resistor_mode; }); it_resistor_mode != g_tenso_resistor_modes.end())
        return it_resistor_mode->first;

    return {};
}

static inline zsp_tr_nsh get_tr_nsh(const ::std::wstring& tr_nsh)
{
    if (auto it_tr_nsh = g_tr_nshs.find(tr_nsh); it_tr_nsh != g_tr_nshs.end())
        return it_tr_nsh->second;

    return zsp_tr_nsh::unknown;
}

static inline ::std::wstring get_tr_nsh(const zsp_tr_nsh& tr_nsh)
{
    if (auto it_tr_nsh = std::find_if(g_tr_nshs.begin(), g_tr_nshs.end(), [&tr_nsh](const auto& nsh)
        { return nsh.second == tr_nsh; }); it_tr_nsh != g_tr_nshs.end())
        return it_tr_nsh->first;

    return {};
}

static inline zsp_tc_type get_tc_type(const ::std::wstring& tc_type)
{
    if (auto it_tc_type = g_tc_types.find(tc_type); it_tc_type != g_tc_types.end())
        return it_tc_type->second;

    return zsp_tc_type::unknown;
}

static inline ::std::wstring get_tc_type(const zsp_tc_type& tc_type)
{
    if (auto it_tc_type = std::find_if(g_tc_types.begin(), g_tc_types.end(), [&tc_type](const auto& type)
        { return type.second == tc_type; }); it_tc_type != g_tc_types.end())
        return it_tc_type->first;

    return {};
}

static inline bool is_tenso_scheme_quarter(const zsp_tenso_scheme& scheme)
{
    return
        scheme == zsp_tenso_scheme::quarter_r1_i ||
        scheme == zsp_tenso_scheme::quarter_r2_i ||
        scheme == zsp_tenso_scheme::quarter_r3_i ||
        scheme == zsp_tenso_scheme::quarter_r4_i ||
        scheme == zsp_tenso_scheme::quarter_r1_u ||
        scheme == zsp_tenso_scheme::quarter_r2_u ||
        scheme == zsp_tenso_scheme::quarter_r3_u ||
        scheme == zsp_tenso_scheme::quarter_r4_u;
}

static inline bool is_tenso_scheme_half(const zsp_tenso_scheme& scheme)
{
    return
        scheme == zsp_tenso_scheme::half_3w_i ||
        scheme == zsp_tenso_scheme::half_3w_u ||
        scheme == zsp_tenso_scheme::half_5w_u;
}

static inline bool is_tenso_scheme_full(const zsp_tenso_scheme& scheme)
{
    return
        scheme == zsp_tenso_scheme::full_4w_i ||
        scheme == zsp_tenso_scheme::full_4w_u ||
        scheme == zsp_tenso_scheme::full_6w_u;
}

static inline bool is_compatible(const zsp_tenso_resistor_mode mode1, const zsp_tenso_resistor_mode mode2)
{
    switch (mode1)
    {
    case zsp_tenso_resistor_mode::quarter_tension_compression_1:
    case zsp_tenso_resistor_mode::quarter_tension_compression_2:
    case zsp_tenso_resistor_mode::quarter_tension_compression_3:
    case zsp_tenso_resistor_mode::quarter_bending:
        switch (mode2)
        {
        case zsp_tenso_resistor_mode::quarter_tension_compression_1:
        case zsp_tenso_resistor_mode::quarter_tension_compression_2:
        case zsp_tenso_resistor_mode::quarter_tension_compression_3:
        case zsp_tenso_resistor_mode::quarter_bending:
            return true;

        default:
            break;
        }
        break;

    case zsp_tenso_resistor_mode::half3_tension_compression_1:
    case zsp_tenso_resistor_mode::half3_tension_compression_2:
    case zsp_tenso_resistor_mode::half3_tension_compression_3:
    case zsp_tenso_resistor_mode::half3_tension_compression_4:
    case zsp_tenso_resistor_mode::half3_bending_1:
    case zsp_tenso_resistor_mode::half3_bending_2:
    case zsp_tenso_resistor_mode::half3_torsion:
        switch (mode2)
        {
        case zsp_tenso_resistor_mode::half3_tension_compression_1:
        case zsp_tenso_resistor_mode::half3_tension_compression_2:
        case zsp_tenso_resistor_mode::half3_tension_compression_3:
        case zsp_tenso_resistor_mode::half3_tension_compression_4:
        case zsp_tenso_resistor_mode::half3_bending_1:
        case zsp_tenso_resistor_mode::half3_bending_2:
        case zsp_tenso_resistor_mode::half3_torsion:
            return true;

        default:
            break;
        }
        break;

    case zsp_tenso_resistor_mode::half5_tension_compression_1:
    case zsp_tenso_resistor_mode::half5_tension_compression_2:
    case zsp_tenso_resistor_mode::half5_tension_compression_3:
    case zsp_tenso_resistor_mode::half5_tension_compression_4:
    case zsp_tenso_resistor_mode::half5_bending_1:
    case zsp_tenso_resistor_mode::half5_bending_2:
    case zsp_tenso_resistor_mode::half5_torsion:
        switch (mode2)
        {
        case zsp_tenso_resistor_mode::half5_tension_compression_1:
        case zsp_tenso_resistor_mode::half5_tension_compression_2:
        case zsp_tenso_resistor_mode::half5_tension_compression_3:
        case zsp_tenso_resistor_mode::half5_tension_compression_4:
        case zsp_tenso_resistor_mode::half5_bending_1:
        case zsp_tenso_resistor_mode::half5_bending_2:
        case zsp_tenso_resistor_mode::half5_torsion:
            return true;

        default:
            break;
        }
        break;

    case zsp_tenso_resistor_mode::full4_tension_compression_1:
    case zsp_tenso_resistor_mode::full4_tension_compression_2:
    case zsp_tenso_resistor_mode::full4_tension_compression_3:
    case zsp_tenso_resistor_mode::full4_tension_compression_4:
    case zsp_tenso_resistor_mode::full4_tension_compression_5:
    case zsp_tenso_resistor_mode::full4_tension_compression_6:
    case zsp_tenso_resistor_mode::full4_tension_compression_7:
    case zsp_tenso_resistor_mode::full4_tension_compression_8:
    case zsp_tenso_resistor_mode::full4_bending_1:
    case zsp_tenso_resistor_mode::full4_bending_2:
    case zsp_tenso_resistor_mode::full4_bending_3:
    case zsp_tenso_resistor_mode::full4_torsion_1:
    case zsp_tenso_resistor_mode::full4_torsion_2:
        switch (mode2)
        {
        case zsp_tenso_resistor_mode::full4_tension_compression_1:
        case zsp_tenso_resistor_mode::full4_tension_compression_2:
        case zsp_tenso_resistor_mode::full4_tension_compression_3:
        case zsp_tenso_resistor_mode::full4_tension_compression_4:
        case zsp_tenso_resistor_mode::full4_tension_compression_5:
        case zsp_tenso_resistor_mode::full4_tension_compression_6:
        case zsp_tenso_resistor_mode::full4_tension_compression_7:
        case zsp_tenso_resistor_mode::full4_tension_compression_8:
        case zsp_tenso_resistor_mode::full4_bending_1:
        case zsp_tenso_resistor_mode::full4_bending_2:
        case zsp_tenso_resistor_mode::full4_bending_3:
        case zsp_tenso_resistor_mode::full4_torsion_1:
        case zsp_tenso_resistor_mode::full4_torsion_2:
            return true;

        default:
            break;
        }
        break;

    case zsp_tenso_resistor_mode::full6_tension_compression_1:
    case zsp_tenso_resistor_mode::full6_tension_compression_2:
    case zsp_tenso_resistor_mode::full6_tension_compression_3:
    case zsp_tenso_resistor_mode::full6_tension_compression_4:
    case zsp_tenso_resistor_mode::full6_tension_compression_5:
    case zsp_tenso_resistor_mode::full6_tension_compression_6:
    case zsp_tenso_resistor_mode::full6_tension_compression_7:
    case zsp_tenso_resistor_mode::full6_tension_compression_8:
    case zsp_tenso_resistor_mode::full6_bending_1:
    case zsp_tenso_resistor_mode::full6_bending_2:
    case zsp_tenso_resistor_mode::full6_bending_3:
    case zsp_tenso_resistor_mode::full6_torsion_1:
    case zsp_tenso_resistor_mode::full6_torsion_2:
        switch (mode2)
        {
        case zsp_tenso_resistor_mode::full6_tension_compression_1:
        case zsp_tenso_resistor_mode::full6_tension_compression_2:
        case zsp_tenso_resistor_mode::full6_tension_compression_3:
        case zsp_tenso_resistor_mode::full6_tension_compression_4:
        case zsp_tenso_resistor_mode::full6_tension_compression_5:
        case zsp_tenso_resistor_mode::full6_tension_compression_6:
        case zsp_tenso_resistor_mode::full6_tension_compression_7:
        case zsp_tenso_resistor_mode::full6_tension_compression_8:
        case zsp_tenso_resistor_mode::full6_bending_1:
        case zsp_tenso_resistor_mode::full6_bending_2:
        case zsp_tenso_resistor_mode::full6_bending_3:
        case zsp_tenso_resistor_mode::full6_torsion_1:
        case zsp_tenso_resistor_mode::full6_torsion_2:
            return true;

        default:
            break;
        }
        break;

    default:
        break;

    }
    return false;
}

};

template< typename T >
inline ::std::wstring int_to_hex(T i)
{
    std::wstringstream stream;
    stream << L"0x" << std::setfill(L'0') << std::setw(sizeof(T) * 2) << std::hex << i;
    return stream.str();
}
