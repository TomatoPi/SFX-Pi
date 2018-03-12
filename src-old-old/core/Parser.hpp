/**********************************************************************
 * @file Parser.h
 * @author TomatoPi
 * @version 1.0
 *
 * Parser
 **********************************************************************/
namespace Parser{

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
}
