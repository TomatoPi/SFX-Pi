/**********************************************************************
 * @file Parser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
namespace Parser{
        
    template <typename Key, typename Arg>
    ParsedObject<Key,Arg>::ParsedObject(Key k):
        m_key(k),
        m_arg(std::vector<std::pair<Key,Arg>>()),
        m_child(std::vector<ParsedObject>())
    {
    }

    std::vector<std::string> parseSimpleString( std::string input
        , const char* sperator
        , const char antiseparator
        )
    {
        // If given string is empty, return empty vector
        if ( input.size() == 0 ){
            return std::vector<std::string>();
        }
        // First remove white spaces before first token
        input.erase( 0, input.find_first_not_of(sperator));

        // Token extraction variables
        uint16_t begin = 0;
        uint16_t end = 0;
        std::vector<std::string> out;
        
        // Extract All tokens
        while( end != std::string::npos && end < input.size() && begin < input.size() ){
            begin = input.find_first_not_of(sperator, end);
            end = input.find_first_of(sperator, begin);

            if ( begin < input.size() ){

                // If a spaced sequence is detected get it all in a single row
                if ( input.at(begin) == antiseparator ){

                    begin = input.find_first_not_of(antiseparator, begin);
                    end = input.find_first_of(antiseparator, begin);
                    
                    out.push_back(input.substr( begin, end-begin ));

                    end = input.find_first_of(sperator, end);
                }
                else{

                    out.push_back(input.substr( begin, end-begin ));
                }
            }
        }

        return out;
    }

    template <typename T>
    bool vectorContains(T obj, std::vector<T> list){

        for ( auto& cur : list ){

            if ( obj == cur ){
                return true;
            }
        }
        return false;
    }

    template <>
    bool vectorContains<std::string>(std::string obj, std::vector<std::string> list){

        for ( auto& cur : list ){

            if ( !obj.compare(cur) ){
                return true;
            }
        }
        return false;
    }

    /**
     * Function that parse a file in key=value format
     * where value can be a block containing several other blocs
     * 
     * @param flux : input stream of file to parse
     * @param sep : token sepearators
     * @param equal : char separating key and values
     * @param blocko : char indicating block begining
     * @param blocke : char indicating block end
     *
     * @return vector of parsed blocks
     **//*
    template <typename Key, typename Arg>
    static std::vector<ParsedObject<Key,Arg>> parseBlockFile(
        std::ifstream& flux,
        const char sep,
        const char equal,
        const char blocko,
        const char blocke )
    {
        std::string key;
        std::string value;

        char c;
        while( flux.get(c) ){

            if ( flag & parseKey );
        }

        return std::vector<ParsedObject<Key,Arg>>();
    }

    
    template <typename Key, typename Arg>
    static ParsedObject<Key,Arg> parseBlock(
        std::ifstream& flux,
        std::string k,
        const char sep,
        const char equal,
        const char blocko,
        const char blocke )
    {
        ParsedObject<Key,Arg> rtn(k);
        std::string key;
        std::string value;

        char c;
        while( flux.get(c) ){

            if ( c == sep ){

                if ( key.size() != 0 ){

                    rtn.m_arg.push_back(std::make_pair<Key,Arg>(
                        std::to_string(rtn.m_arg.size()),
                        
                }
            }
        }
    }
    */
}
