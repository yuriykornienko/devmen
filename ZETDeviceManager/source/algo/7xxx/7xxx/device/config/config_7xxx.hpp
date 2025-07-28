#pragma once
#ifndef config_7xxx_hpp__
#define config_7xxx_hpp__

#include "../device_7xxx.hpp"
#include "../../../../../types.hpp"
#include "../../../../utils/shield_access/shield_access.hpp"
#include "../../../../utils/string_conv/string_conv.hpp"

#include <ModbusDefinitions.h>
#include "StructForCfgFile.h"

#include <ZET7xxx/include/base/zetr7xxxbase_auto_link.h>
#include <ZET7xxx/include/work/zet7xxxlib_auto_link.h>
#include <ZET7xxx/include/work/configuration/cfg_work_helper/cfg_work_helper.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>

namespace zetlab { namespace _7xxx {

ZET7XXX_DEFINE_DEVICE_NAMES(device_names);

class device::config
{
public:
    class page
    {
    public:

    public:
        class param
        {
        public:			
            param( const tstring& name_,
                   const type_of_arg& type_,
                   void* addr_,
                   void* addr2_,
                   const size_t& size_,
                   const bool& editable_ , 
                   const tstring& sensorName_,
                   const uint64_t& serial_,
                   const STRUCT_TYPE& structType_);

        public:
			const tstring& getName() const;

			const tstring& getSensorName() const;

            const type_of_arg& getType() const;

            const STRUCT_TYPE& getStructType() const;

            const uint64_t& getSerial() const;

            // Calculates definition size
            const size_t size1() const;

            // Calculates device data size
            const size_t size2() const;

            const tstring getValue( ::std::vector< tstring >& extra );

            void setValue( const tstring& value );

            bool restoreDefaultValue();

            const bool& isEditable() const;

            const bool& isModified() const;

			void* getAddr() const;	
			void setAddr(BYTE* inAddr);

			void* getAddr2() const;
			void setAddr2(BYTE* inAddr2);

			size_t getSize() const;

        private:
            template< typename T >
            inline const tstring list( ::std::vector< tstring >& extra )
            {
                BYTE* addr2_( reinterpret_cast< BYTE* >(addr2) + sizeof(STRUCT_ARGV));
                long  size  (*reinterpret_cast< long* >(addr2_));
                T*    val_  ( reinterpret_cast<   T*  >((addr2_ + sizeof(long))));

				if (size < 1 || size > 32)
					return tstring();

                switch (type)
                {
                    case t_list_float:
                        for (long i = 0; i < size; ++i)
                            extra.push_back(my_lexical_cast<tstring>(val_[i]));

                        return my_lexical_cast<tstring>(*reinterpret_cast<T*>(addr));
                    default:
                        for (long i = 0; i < size; ++i)
                            extra.push_back(boost::lexical_cast<tstring>(val_[i]));

                        return boost::lexical_cast<tstring>(*reinterpret_cast<T*>(addr));
                }
            };

            template<>
            inline const tstring list< char* >( ::std::vector< tstring >& extra )
            {
                BYTE*  addr2_( reinterpret_cast< BYTE* >(addr2) + sizeof(STRUCT_ARGV));
                long   size  (*reinterpret_cast< long* >(addr2_));
                char*  val_  ( reinterpret_cast< char* >((addr2_ + sizeof(long))));

				if (size < 1 || size > 32)
					return tstring();

                for (long i = 0; i < size; ++i)
                {
                    tstring value(utils::cp1251_to_string(&val_[i * 16], 16));
                    utils::translate(value, sensorName);
                    extra.push_back(value);
                }

                return tstring();
            };

            inline const tstring list_ret_char( ::std::vector< tstring >& extra )
            {
                list< char* >( extra );
                tstring value(utils::cp1251_to_string(reinterpret_cast< char* >(addr)));
                utils::translate(value, sensorName);
                return value;
            };

            inline const tstring list_ret_index( ::std::vector< tstring >& extra )
            {
                list< char* >( extra );
                BYTE*  addr2_(reinterpret_cast< BYTE* >(addr2) + sizeof(STRUCT_ARGV) + sizeof(long));
                long index = *reinterpret_cast<long*>(addr);
				if (index >= static_cast<long>(extra.size()) || index < 0)
                    return _T("");
                tstring value(utils::cp1251_to_string((char*)&addr2_[index * 16], 16));
                utils::translate(value, sensorName);
                return value;
            };

            template< typename T >
            inline void saveValue( const tstring& tstrValue )
            {
                try
                {
                    T  value  (boost::lexical_cast< T >( tstrValue ));
                    T* pMemT  (reinterpret_cast< T* >(addr));
                    T* pValueT(&value);

                    memcpy_s( pMemT, sizeof(T), pValueT, sizeof(T) );
                }
                catch (boost::bad_lexical_cast&)
                {
                }
            };

            template<>
            inline void saveValue< char* >( const tstring& tstrValue )
            {
                const ::std::string value(utils::string_to_cp1251(tstrValue));

                char* pMemT  (reinterpret_cast< char* >(addr));
                const char* pValueT(value.c_str());

                size_t len(strlen( pValueT ));
                len = (len >= size) ? size - 1 : len;

                memcpy_s( pMemT, len, pValueT, len );
                pMemT[len] = 0;
            };

            template<>
            inline void saveValue< unsigned char >( const tstring& tstrValue )
            {
                unsigned char value = (unsigned char)_tcstoul(tstrValue.c_str(), nullptr, 10);
                unsigned char* pMemT(reinterpret_cast<unsigned char*>(addr));
                
                memcpy_s(pMemT, sizeof(unsigned char), &value, sizeof(unsigned char));
            };

        private:
            tstring     name;
            tstring     default_value;
            type_of_arg type;
            void*       addr;
            void*       addr2;
            size_t      size;
            bool        editable;
            bool        modified;
            tstring		sensorName;
            uint64_t    serial;
            STRUCT_TYPE structType;
        }; // param
        typedef boost::shared_ptr< param > param_ptr;
        typedef ::std::vector< param_ptr > params_type;

    public:
        page( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev,
              utils::access_type access_, const STRUCT_TYPE& type_,
              const tstring& sensorName_, const uint64_t& serial_);

    public:
        const tstring& getName() const;

        const tstring& getDesc() const;

        const STRUCT_TYPE& getType() const;

        const utils::access_type& getAccessType() const;

        params_type::iterator begin();

        params_type::iterator end();

        params_type::size_type size() const;

    private:
        void parse( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev );

        param* addParam( char* name_,
                         const type_of_arg& type_,
                         void* addr_,
                         void* addr2_,
                         const size_t& size_,
                         const bool& editable_,
                         const tstring& sensorType_,
                         const uint64_t& serial_,
                         const STRUCT_TYPE& structType_);

    private:
        tstring            name;
        tstring            desc;
        STRUCT_TYPE        type;
        utils::access_type access;
        params_type        params;
        tstring			   sensorName;
        uint64_t           serial;
    }; // page

    typedef boost::shared_ptr< page > page_ptr;
    typedef ::std::vector< page_ptr > pages_type;

public:
    config( STRUCT7000EX* shared_struct_, const utils::user_type& user_ );
	config(	_structCfg7xxx* shared_struct_, const utils::user_type& user_	);

public:
    pages_type::iterator begin();

    pages_type::iterator end();

    pages_type::size_type size() const;

    const utils::user_type& getUserType() const;

private:
    void parse( STRUCT7000EX* shared_struct );
	void parse(	_structCfg7xxx* shared_struct );

    void addPage( STRUCT_HEAD* head_def, STRUCT_HEAD* head_dev,
                  utils::access_type access_, const STRUCT_TYPE& type_, 
                  const tstring& sensorType_, const uint64_t& serial_);

private:
    pages_type pages;
    utils::user_type user;
    tstring sensorName;
    uint64_t serial;
};

}} // namespace zetlab::_7xxx

#endif // config_7xxx_hpp__
