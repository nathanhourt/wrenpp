
#pragma once

extern "C" {
    #include <wren.h>
}
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace wrenly {

using LoadModuleFn = std::function<char*( const char*)>;

/**
 * @class Method
 * @author Muszynski Johann M
 * @date 13/08/15
 * @file Wrenly.h
 * @brief Use this to call a wren method from C++.
 * Make sure that the Wren instance which dispensed this object does not go out of scope
 * before this instance does. A segfault will occur otherwise.
 */
class Method {
    public:
        Method( WrenVM*, WrenMethod* );
        Method() = delete;
        Method( const Method& );
        Method( Method&& );
        Method& operator=( const Method& )  = delete;
        Method& operator=( Method&& )       = delete;
        Method& operator=( Method );
        ~Method();
            
        template<typename... Args>
        void operator()( Args&&... args );
    
    private:
        /*
         * For parsing arguments*/
        struct Any {
            Any( bool e )   : type( 'b' ) {}
            Any( double e ) : type( 'd' ) {}
            Any( float e )  : type( 'd' ) {}
            Any( int e )    : type( 'i' ) {}
            Any( const char* ) : type( 's' ) {}
            Any( const std::string& ) : type( 's' ) {}
            
            char type;
        };
        
        
        void retain_();
        void release_();
    
        WrenVM*     vm_;
        WrenMethod* method_;
        unsigned*   refCount_;
};

/**
 * @class Wren
 * @author Nelarius
 * @date 09/08/2015
 * @file Wren.h
 * @brief Holds an instance of the Wren virtual machine.
 * This class is refcounted, so it can be passed around safely.
 * This class is NOT thread-safe, however.
 */
class Wren {

    public:
        Wren();
        Wren( const Wren& );
        Wren( Wren&& );
        Wren& operator=( const Wren& );
        Wren& operator=( Wren&& );
        ~Wren();
        
        void executeModule( const std::string& );
        void executeString( const std::string& );
        
        Method method(
            const std::string& module,
            const std::string& variable,
            const std::string& signature
        );
        
        static LoadModuleFn loadModuleFn;
        
    private:
        void retain_();
        void release_();
    
        WrenVM*	    vm_;
        unsigned*   refCount_;
};


/////////////////////////////////////////////////////////////////////////////
// Method implementation
/////////////////////////////////////////////////////////////////////////////
template<typename... Args>
void Method::operator()( Args&&... args ) {
    std::vector<Any> vec = { args... };
    std::stringstream ss;
    for ( auto a: vec ) {
        ss << a.type;
    }
    wrenCall( vm_, method_, ss.str().c_str(), std::forward<Args>( args )... );
}

}   // wrenly