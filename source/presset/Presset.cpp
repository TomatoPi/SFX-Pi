#include "Presset.h"
/*
*	---------------------------------------------------------------------------
*	Presset stuff
*	---------------------------------------------------------------------------
*/
int save_preset(string const name, string const version, Module_Node_List *list){
	
	string filename = "/home/sfx_pi/sfx/Files/" + name ;
	
    try{
        
        ofstream flux(filename.c_str());
        cout << "Save presset file : " << filename << " -- ";
        
        if(flux){
            
            // Write root node
            flux << NODE_ROOT_PRESET << "={" << endl;
            flux << TAB << NRP_NAME << "=" << name << endl;
            flux << TAB << NRP_VERSION << "=" << version << endl;
            flux << "}" << endl;
            
            // Write module node for each module
            for ( Module_iterator itr = list->list_.begin() ; itr != list->list_.end(); itr++ ){
                
                Module* mod = (*itr)->get_module();
                
                // Open node and indicate module type
                flux << NODE_MODULE << "={" << endl;
                flux << TAB << NMOD_TYPE << "=" << mod->get_type() << endl;
                
                // If module has banks, save it
                if ( mod->set_bank(0) ){
                    
                    do{
                        
                        flux << TAB << NMOD_BANK << "={" << endl;
                        flux << TAB << TAB << NMOD_SIZE << "=" << mod->get_param_count() << endl;
                        flux << TAB << TAB << NMOD_DATA << "={";
                        
                        for ( int i = 0; i < mod->get_param_count(); i++ ){
                            
                            flux << " " << mod->get_param(i);
                        }
                        
                        flux << " }" << endl;
                        flux << TAB  << "}" << endl;
                        
                    }while ( !mod->next_bank() );
                }
                
                // Once banks are saved close node and go to next module
                flux << "}" << endl;
            }
            
            // Write connection node for each connection
            for ( Module_iterator itr = list->list_.begin() ; itr != list->list_.end(); itr++ ){
                
                // Write a node for each connection
                Connection_List cl = (*itr)->connection_get_list();
                
                for ( Connection_iterator ctr = cl.begin(); ctr != cl.end(); ctr++ ){
                    
                    Connection c = *ctr;
                    
                    flux << NODE_CONNECTION << "={ " << c.s << " " << c.sp << " " << c.t << " ";
                    flux << c.tp << " }" << endl;
                }
            }
            
            // Write Begin module's Connections
            Connection_List bl = list->begin_.connection_get_list();
            for ( Connection_iterator ctr = bl.begin(); ctr != bl.end(); ctr++ ){
                
                Connection c = *ctr;
                
                flux << NODE_CONNECTION << "={ " << c.s << " " << c.sp << " " << c.t << " ";
                flux << c.tp << " }" << endl;
            }
            
            // Write End module's Connections
            Connection_List el = list->end_.connection_get_list();
            for ( Connection_iterator ctr = el.begin(); ctr != el.end(); ctr++ ){
                
                Connection c = *ctr;
                
                flux << NODE_CONNECTION << "={ " << c.s << " " << c.sp << " " << c.t << " ";
                flux << c.tp << " }" << endl;
            }
            
        }else{
            
            flux.close();
            cout << "Error can't open file " << filename << endl;
            return 1;
        }
        flux.close();
        cout << "Presset successfully saved" << endl;
        
    }
    catch ( ... ){
                
        cout << "Exception catched while opening/reading/parsing file" << endl;
        return 1;
    }
	return 0;
}

int save_module(string const name, string const version, Module *mod){
    
    string filename = "/home/sfx_pi/sfx/Files/" + name ;
	
    try {
        
        ofstream flux(filename.c_str());
        cout << "Save module : " << filename << " -- ";
        
        if(flux){
            
            // Save root node indicating that file is a module file
            flux << NODE_ROOT_MODULE << " " << version << " " << static_cast<int>(mod->get_type()) << endl;
            
            // Set current bank to bank 0 ( will fail if module hasn't bank
            if(mod->set_bank(0)){
                
                // Save each bank
                do{

                    int param_count = mod->get_param_count();
                    
                    flux << "    " << NODE_PARAM << " " << param_count ;
                    for(int n = 0; n < param_count; n++) flux << " " << mod->get_param(n);
                    flux << endl;
                    
                }while(!mod->next_bank());
                
            }else{
            
                flux.close();
                cout << "Error can't open file " << filename << endl;
                return 1;
            }
            
        }else{
            
            flux.close();
            cout << "Error can't open file " << filename << endl;
            return 1;
        }
        flux.close();
        cout << "Module successfully saved" << endl;
        
    }
    catch ( ... ){
                
        cout << "Exception catched while opening/reading/parsing file" << endl;
        return 1;
    }
	return 0;
}

typedef enum{
    
    FROOT,
    
    FWPRE,
    FFPRE,
    
    FWMOD,
    FFMOD,
    
    FWBAK,
    FFBAK,
    
    FWDAT,
    FFDAT,
    
    FWCON,
    FFCON
}PFLAG;

int load_preset(string const name, string const version, Module_Node_List*& list){
	
    string filename = "/home/sfx_pi/sfx/Files/" + name ;

    // New graph
    Module_Node_List *new_graph = new Module_Node_List();

    // Store current position inside file
    PFLAG flag = FROOT;

    // Store current mod index
    int cur_mod = 0;

    // Buffer for store banks
    float *dat_buff = NULL;
    int dat_size = 0;
    int dat_idx = 0;

    // Store current bank index
    int cur_bak = 0;

    // Buffer for store connections
    int con_buff[4] = {};
    int con_idx = 0;
    
    try {
            
        ifstream flux( filename.c_str() );
        cout << "Load presset : " << filename << " -- " << endl;
        
        if(flux){
            
            // While file has new lines to read
            while ( !flux.eof() ){
                
                // Store full line
                char buff[MAX_LINE_LENGTH];
                // Store list of tokens
                char* token[MAX_TOKEN_COUNT] = {};
                
                // Get new line from file
                flux.getline( buff, MAX_LINE_LENGTH );
                
                int n = 0;
                
                // Parse the line
                token[0] = strtok( buff, " =");
                if ( token[0] ){
                    for ( n = 1; n < MAX_TOKEN_COUNT; n++ ){
                        
                        token[n] = strtok( NULL, " " );
                        if ( !token[n] ) break;
                    }
                }
                
                for ( int i = 0; i < n; i++ ){
                    
                    //cout << "Token[" << i << "] = \"" << token[i] << "\"" << endl;
                    
                    // If not inside a node
                    if ( flag == FROOT ){
                            
                        if ( !string(token[i]).compare(NODE_ROOT_MODULE) ){
                            
                            throw string( "File type don't match : Module File" );
                        }
                        else if ( !string(token[i]).compare(NODE_ROOT_PRESET) ){
                            
                            flag = FWPRE;
                        }
                        else if ( !string(token[i]).compare(NODE_MODULE) ){
                            
                            flag = FWMOD;
                        }
                        else if ( !string(token[i]).compare(NODE_CONNECTION) ){
                            
                            flag = FWCON;
                        }
                    }
                    
                    // If waiting node begin carracter
                    else if ( flag == FWPRE && !string(token[i]).compare("{") ){
                    
                        flag = FFPRE;
                        cout << "Entered NODE_ROOT_PRESET node" << endl;
                    }
                    else if ( flag == FWMOD && !string(token[i]).compare("{") ){
                    
                        flag = FFMOD;
                        cur_bak = 0;
                        cout << "Entered NODE_MODULE node" << endl;
                    }
                    else if ( flag == FWBAK && !string(token[i]).compare("{") ){
                    
                        flag = FFBAK;
                        dat_size = 0;
                        cout << "  Entered NODE_BANK node" << endl;
                    }
                    else if ( flag == FWDAT && !string(token[i]).compare("{") ){
                    
                        flag = FFDAT;
                        dat_buff = new float[dat_size];
                        dat_idx = 0;
                        //cout << "    Entered NMOD_DATA node" << endl;
                    }
                    else if ( flag == FWCON && !string(token[i]).compare("{") ){
                    
                        flag = FFCON;
                        con_idx = 0;
                        cout << "Entered NODE_CONNECTION node" << endl;
                    }
                    
                    // If Already inside a node
                    // Inside root node
                    else if ( flag == FFPRE ){
                        
                        // Presset Name
                        if ( !string(token[i]).compare(NRP_NAME) ){
                            
                            cout << "  Presset Name : \"" << token[i+1] << "\"" << endl;
                            break;
                        }
                        // Preset Version
                        else if ( !string(token[i]).compare(NRP_VERSION) ){
                            
                            cout << "  Presset Version : \"" << token[i+1] << "\"" << endl;
                            
                            if ( !!string(token[i+1]).compare(version) ){
                             
                                throw string("File Version do not match Program Version");
                            }
                            break;
                        }
                        // Exit root
                        else if ( !string(token[i]).compare("}") ){
                        
                            flag = FROOT;
                            cout << "Exit NODE_ROOT_PRESET node" << endl << endl;
                        }
                    }
                    // Module node
                    else if ( flag == FFMOD ){
                        
                        // Module's Type
                        if ( !string(token[i]).compare(NMOD_TYPE) ){
                            
                            cout << "  Module type : \"" << token[i+1] << "\"" << endl;
                            
                            if ( new_graph->add_module( static_cast<MODULE_TYPE>( atoi(token[i+1]) ) ) ){
                                
                                throw string("Invalid Module Type");
                            }
                            
                            cur_mod = new_graph->list_.size() -1;
                            
                            break;
                        }
                        // Module's Bank
                        else if ( !string(token[i]).compare(NMOD_BANK) ){
                            
                            flag = FWBAK;
                        }
                        // Exit Module
                        else if ( !string(token[i]).compare("}") ){
                        
                            flag = FROOT;
                            cout << "Exit NODE_MODULE node" << endl << endl;
                        }
                    }
                    // Bank node
                    else if ( flag == FFBAK ){
                        
                        // Bank Size
                        if ( !string(token[i]).compare(NMOD_SIZE) ){
                            
                            cout << "    Bank size : \"" << token[i+1] << "\"" << endl;
                            dat_size = atoi( token[i+1] );
                            
                            break;
                        }
                        // Bank Datas
                        else if ( !string(token[i]).compare(NMOD_DATA) ){
                            
                            flag = FWDAT;
                        }
                        // Exit Bank
                        else if ( !string(token[i]).compare("}") ){
                        
                            flag = FFMOD;
                            cur_bak++;
                            
                            cout << "  Exit NODE_BANK node" << endl;
                        }
                    }
                    // Bank Datas
                    else if ( flag == FFDAT ){
                        
                        // Get Datas
                        if ( !!string(token[i]).compare("}") ){
                            
                            if ( dat_idx >= dat_size ){
                                
                                dat_idx = dat_size + 1;
                                cout << "    Too much data for Bank" << endl;
                            }
                            else{
                                
                                dat_buff[dat_idx] = atof( token[i] );
                                dat_idx++;
                            }
                        }
                        //Exit Datas
                        else{
                            
                            if ( dat_idx == dat_size ){
                                
                                cout << "    New Bank Parsed : [ " << dat_buff[0];
                                for( int k = 1; k < dat_size; k++ ) cout << " , " << dat_buff[k];
                                cout << " ]" << endl;
                                
                                if ( cur_bak == 0 ){
                                    
                                    new_graph->list_[cur_mod]->get_module()->set_param( dat_size, dat_buff );
                                }
                                else{
                                    
                                    new_graph->list_[cur_mod]->get_module()->add_bank( dat_size, dat_buff );
                                }
                            }
                            else{
                                
                                cout << "    Bank parsing Error" << endl;
                            }
                            
                            delete dat_buff;
                            flag = FFBAK;
                            //cout << "    Exit NMOD_DATA node" << endl;
                        }
                    }
                    // Connection node
                    else if ( flag == FFCON ){
                        
                        if ( !!string(token[i]).compare("}") ){
                            
                            if ( con_idx >= 4 ){
                                
                                con_idx = 5;
                                cout << "Error to much arguments for connection" << endl;
                            }else{
                                    
                                con_buff[con_idx] = atoi( token[i] );
                                con_idx++;
                            }
                        }
                        else{
                        
                            // TODO add new connection to process graph
                            if ( con_idx == 4 ){
                                
                                cout << "  New Connection calculated : [ " << con_buff[0];
                                for( int k = 1; k < 4; k++ ) cout << " , " << con_buff[k];
                                cout << " ]" << endl;
                                
                                new_graph->add_connection( con_buff[0], con_buff[1], con_buff[2], con_buff[3] );
                            }
                            else{
                                
                                cout << "  Connection parsing Error" << endl;
                            }
                            
                            flag = FROOT;
                            cout << "Exit NODE_CONNECTION node" << endl << endl;
                        }
                    }
                }
            }
            
            delete list;
            list = new_graph;
            
        }else{
            
            cout << "Error can't open file " << filename << endl;
            flux.close();
            return 1;
        }
        flux.close();
        cout << "Presset successfully loaded" << endl;
        
    } catch ( string const& e ){
        
        cout << "FATAL ERROR : " << e << endl;
        delete new_graph;
        if ( dat_buff != NULL )delete dat_buff;
        
        return 1;

    } catch ( ... ){
        
        cout << "Exception catched while opening/reading/parsing file" << endl;
        delete new_graph;
        if ( dat_buff != NULL )delete dat_buff;
        
        return 1;
    }
	return 0;
}

int load_module(string const name, string const version, Module* mod, bool del){
    
    string filename = "/home/sfx_pi/sfx/Files/" + name ;
	
    try {
            
        ifstream flux(filename.c_str());
        cout << "Load module : " << filename << " -- ";
    
        if(flux){
            
            string current;
            // Get first word and check if it is NODE_ROOT_MODULE
            flux >> current;
            if ( !!current.compare( NODE_ROOT_MODULE ) ){
                
                cout << "File format not match, first node must be : " << NODE_ROOT_MODULE << endl;
                flux.close();
                return 1;
            }
            current.clear();
            
            // Get second word aka program version
            flux >> current;
            if ( !!current.compare( version ) ){
                
                cout << "Bad version -- File : " << current << " -- Current : " << version << endl;
                flux.close();
                return 1;
            }
            current.clear();
            
            // Get third word aka saved module's type
            int newtype;
            flux >> newtype;
            
            if ( static_cast<MODULE_TYPE>(newtype) != mod->get_type() ){
                
                cout << "Bad Module type -- File : " << newtype << " -- Current : " << mod->get_type() << endl;
                flux.close();
                return 1;
            }
            // If module types match, load banks
            else{
                
                // while mod contains bank, delete it
                while ( mod->set_bank(0) ){
                    
                    mod->remove_bank();
                }
                
                // finaly load saved banks
                while(flux >> current){
                    
                    if(!current.compare(NODE_PARAM)){
                    
                        int param_count;
                        flux >> param_count;
                        
                        //cout << current << " " << param_count ;
                        
                        float *params = new float[param_count];
                        for(int i = 0; i < param_count; i++){
                            flux >> *(params+i);
                            //cout << " " << params[i];
                        }
                        //cout << endl;
                        
                        mod->add_bank(param_count, params);
                    }
                }
            }
            
        }else{
            
            flux.close();
            cout << "Error can't open file " << filename << endl;
            return 1;
        }
        
        if ( del ){
         
            remove( filename.c_str() );
        }
        
        flux.close();
        cout << "Module successfully saved" << endl;
    }
    catch ( ... ){
        
        cout << "Exception catched while opening/reading/parsing file" << endl;
        return 1;
    }
	return 0;
}

int new_preset(string const name, string const version){
    
    string filename = "/home/sfx_pi/sfx/Files/" + name ;
	
    try{
        
        ofstream flux(filename.c_str());
        cout << "Create presset file : " << filename << " -- ";
        
        if ( flux ){
            
            flux << NODE_ROOT_PRESET << " " << version << " " << name << " " << 0 << endl;
        }
        else{
            
            cout << "Error can't open file " << filename << endl;
            flux.close();
            return 1;
        }
        flux.close();
        cout << "Presset successfully created" << endl;
    }
    catch ( ... ){
        
        cout << "Exception catched while opening/reading/parsing file" << endl;
        return 1;
    }
    return 0;
}

int list_files(string const dir, vector<string> *list){
    
    list->clear();
    
    DIR *rep = opendir( dir.c_str() );
    
    if ( rep != NULL ){
        
        struct dirent *ent;
        
        while ( (ent = readdir( rep )) != NULL ){
            
            if ( ent->d_name[0] != '.') {
                
                list->push_back( ent->d_name );
                //cout << "File found ! : " << ent->d_name << endl;
            }
        }
    }
    else{
        
        return 1;
    }
    
    closedir( rep );
    
    if ( list->size() == 0 ) return 1;
    return 0;
}