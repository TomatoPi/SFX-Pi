#include "Presset.h"

using namespace std;
using namespace PROG_CONST;

/** 
* Preset File's nodes list 
* @see save_preset(string, string, Module_Node_List) 
*/
namespace {
    
    const string NODE_ROOT_PRESET("Preset");
    const string NRP_NAME("Name");
    const string NRP_VERSION("Version");

    const string NODE_ROOT_MODULE("ModuleBak");

    const string NODE_MODULE("Module");
    const string NMOD_ID("ID");
    const string NMOD_TYPE("Type");
    const string NMOD_BANK("Bank");
    const string NMOD_SIZE("Size");
    const string NMOD_DATA("Data");

    const string NODE_CONNECTION("Connection");

    const string NODE_POTENTIOMETER("Potentiometer");
    const string NPOT_NAME("Name");
    const string NPOT_IDX("Index");
    const string NPOT_PLAGE("Plage");
    const string NPOT_TARGET("Target");

    const string NPOT_CURVE("Curve");
    const string NPOT_TARGETM("TargetM");
    const string NPOT_TARGETP("TargetP");
    const string NPOT_MIN("Min");
    const string NPOT_MAX("Max");
    const string NPOT_DB("IsdB");
    const string NPOT_RLT("IsRlt");


    const string NODE_PARAM("Params");
    const string NODE_BUTTON("Button");

    const string TAB("    ");
}

/*
*	---------------------------------------------------------------------------
*	Presset stuff
*	---------------------------------------------------------------------------
*/
int save_preset(string const name, Module_Node_List *list, IO_Potentiometer pot[SPI_POTAR_COUNT]){
	
	string filename = PATH_PRESSET + name ;
	
    try{
        
        ofstream flux(filename.c_str());
        cout << endl << "Save presset file : " << filename << " -- ";
        
        if(flux){
            
            // Write root node
            flux << NODE_ROOT_PRESET << "={" << endl;
            flux << TAB << NRP_NAME << "=" << name << endl;
            flux << TAB << NRP_VERSION << "=" << PROG_VERSION << endl;
            flux << "}" << endl;
            
            // Write module node for each module
            for ( Module_iterator itr = list->list_.begin() ; itr != list->list_.end(); itr++ ){
                
                Module* mod = (*itr)->get_module();
                
                // Open node and indicate module type
                flux << NODE_MODULE << "={" << endl;
                flux << TAB << NMOD_TYPE << "=" << mod->get_type() << endl;
                flux << TAB << NMOD_ID << "=" << (*itr)->get_id() << endl;
                
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
            
            // Write Potentiometer node for each potentiometer
            for ( int i = 0; i < SPI_POTAR_COUNT; i++ ){
                
                flux << NODE_POTENTIOMETER << "={" << endl;
                flux << TAB << NPOT_NAME << "=" << pot[i].get_name() << endl;
                flux << TAB << NPOT_IDX << "=" << i << endl;
                flux << TAB << NPOT_PLAGE << "=" << pot[i].get_min() << " " << pot[i].get_max() << endl;
                
                vector<Accessor> accs = pot[i].get_accessor();
                
                for ( vector<Accessor>::iterator itr = accs.begin(); itr != accs.end(); itr++ ){
                    
                    Accessor acr = (*itr);
                    
                    flux << TAB << NPOT_TARGET << "={" << endl;
                    
                    flux << TAB << TAB << NPOT_CURVE << "=" << acr.get_curve() << endl;
                    flux << TAB << TAB << NPOT_TARGETM << "=" << acr.target_ << endl;
                    flux << TAB << TAB << NPOT_TARGETP << "=" << acr.targetp_ << endl;
                    flux << TAB << TAB << NPOT_MIN << "=" << acr.min_ << endl;
                    flux << TAB << TAB << NPOT_MAX << "=" << acr.max_ << endl;
                    flux << TAB << TAB << NPOT_DB << "=" << acr.isdb_ << endl;
                    flux << TAB << TAB << NPOT_RLT << "=" << acr.isrlt_ << endl;
                    
                    flux << TAB << "}" << endl;
                }
                
                flux << "}" << endl;
            }
            
            // Write connection node for each connection
            Connection_List cl = list->connection_get_list();
            
            for ( Connection_iterator ctr = cl.begin(); ctr != cl.end(); ctr++ ){
                
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

int save_module(string const name, Module *mod){
    
    string filename = PATH_PRESSET + name ;
	
    try {
        
        ofstream flux(filename.c_str());
        cout << "Save module : " << filename << " -- ";
        
        if(flux){
            
            // Save root node indicating that file is a module file
            flux << NODE_ROOT_MODULE << " " << PROG_VERSION << " " << static_cast<int>(mod->get_type()) << endl;
            
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
    
    FWPOT,
    FFPOT,
    
    FWTAR,
    FFTAR,
    
    FWCON,
    FFCON
}PFLAG;

int load_preset(string const name, Module_Node_List* & list, IO_Potentiometer pot[SPI_POTAR_COUNT]){


    if ( !name.compare("tmp") ){

        cout << "Err Not a Preset File" << endl;
        return 1;
    }
    string filename = PATH_PRESSET + name ;

    // Securities for avoid clear graph when loading a bad preset file
    bool is_file_ok = false;
    bool is_version_ok = false;
    bool clear_once = false;
    bool is_load_ok = false;

    // Store current position inside file
    PFLAG flag = FROOT;

    // Store current mod index
    int cur_mod = 0;
    int cur_type = 0;
    bool tok = false;
    int cur_id = 0;
    bool iok = false;

    // Buffer for store banks
    float *dat_buff = NULL;
    int dat_size = 0;
    int dat_idx = 0;

    // Store current bank index
    int cur_bak = 0;
    
    // Store current potentiometer index
    IO_Potentiometer cur_pot;
    int pot_idx = 0;
    Accessor cur_acc;

    // Buffer for store connections
    int con_buff[4] = {};
    int con_idx = 0;
    
    try {
            
        ifstream flux( filename.c_str() );
        cout << endl << "Load presset : " << filename << " -- " << endl;
        
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
                        else if ( !string(token[i]).compare(NODE_POTENTIOMETER) ){
                            
                            flag = FWPOT;
                        }
                        else if ( !string(token[i]).compare(NODE_CONNECTION) ){
                            
                            flag = FWCON;
                        }
                    }
                    
                    // If waiting node begin carracter
                    else if ( flag == FWPRE && !string(token[i]).compare("{") ){
                    
                        is_file_ok = true;
                        flag = FFPRE;
                        //cout << "Entered NODE_ROOT_PRESET node" << endl;
                    }
                    else if ( flag == FWMOD && !string(token[i]).compare("{") ){
                    
                        if ( is_file_ok && is_version_ok ){
                            if ( !clear_once ){
                                
                                list->clear_graph();
                                clear_once = true;
                            }
                        }
                        else{
                            
                            throw string("File Root Node Is Missing Or Invalid");
                        }
                        
                        flag = FFMOD;
                        cur_bak = 0;
                        
                        cur_id = 0;
                        iok = false;
                        
                        cur_type = 0;
                        tok = false;
                        
                        //cout << "Entered NODE_MODULE node" << endl;
                    }
                    else if ( flag == FWBAK && !string(token[i]).compare("{") ){
                    
                        flag = FFBAK;
                        dat_size = 0;
                        //cout << "  Entered NODE_BANK node" << endl;
                    }
                    else if ( flag == FWDAT && !string(token[i]).compare("{") ){
                    
                        flag = FFDAT;
                        
                        dat_buff = new float[dat_size];
                        dat_idx = 0;
                        //cout << "    Entered NMOD_DATA node" << endl;
                    }
                    else if ( flag == FWPOT && !string(token[i]).compare("{") ){
                    
                        flag = FFPOT;
                        cur_pot = IO_Potentiometer();
                        pot_idx = 0;
                        
                        //cout << "Entered NODE_POTENTIOMETER node" << endl;
                    }
                    else if ( flag == FWTAR && !string(token[i]).compare("{") ){
                    
                        flag = FFTAR;
                        cur_acc = Accessor();
                        
                    }
                    else if ( flag == FWCON && !string(token[i]).compare("{") ){
                    
                        flag = FFCON;
                        con_idx = 0;
                        //cout << "Entered NODE_CONNECTION node" << endl;
                    }
                    
                    // If Already inside a node
                    // Inside root node
                    else if ( flag == FFPRE ){
                        
                        // Presset Name
                        if ( !string(token[i]).compare(NRP_NAME) ){
                            
                            //cout << "  Presset Name : \"" << token[i+1] << "\"" << endl;
                            break;
                        }
                        // Preset Version
                        else if ( !string(token[i]).compare(NRP_VERSION) ){
                            
                            //cout << "  Presset Version : \"" << token[i+1] << "\"" << endl;
                            
                            if ( !!string(token[i+1]).compare(PROG_VERSION) ){
                             
                                throw string("File Version do not match Program Version");
                            }
                            is_version_ok = true;
                            break;
                        }
                        // Exit root
                        else if ( !string(token[i]).compare("}") ){
                        
                            flag = FROOT;
                            //cout << "Exit NODE_ROOT_PRESET node" << endl << endl;
                        }
                    }
                    // Module node
                    else if ( flag == FFMOD ){
                        
                        // Module's Type
                        if ( !string(token[i]).compare(NMOD_TYPE) ){
                            
                            //cout << "  Module type : \"" << token[i+1] << "\"" << endl;
                            cur_type = atoi(token[i+1]);
                            tok = true;
                            
                            if ( iok ){
                                
                                if ( list->add_module( static_cast<MODULE_TYPE>( cur_type ), cur_id ) ){
                                    
                                    flag = FROOT;
                                    cout << "Error : Invalid Module Type" << endl;
                                }
                                cur_mod = list->list_.size() -1;
                            }
                        }
                        // Module's ID
                        else if ( !string(token[i]).compare(NMOD_ID) ){
                            
                            cur_id = atoi(token[i+1]);
                            iok = true;
                            
                            if ( tok ){
                                
                                if ( list->add_module( static_cast<MODULE_TYPE>( cur_type ), cur_id ) ){
                                    
                                    flag = FROOT;
                                    cout << "Error : Invalid Module Type" << endl;
                                }
                                cur_mod = list->list_.size() -1;
                            }
                        }
                        // Module's Bank
                        else if ( !string(token[i]).compare(NMOD_BANK) && iok && tok ){
                            
                            flag = FWBAK;
                        }
                        // Exit Module
                        else if ( !string(token[i]).compare("}") ){
                        
                            flag = FROOT;
                            //cout << "Exit NODE_MODULE node" << endl << endl;
                        }
                    }
                    // Bank node
                    else if ( flag == FFBAK ){
                        
                        // Bank Size
                        if ( !string(token[i]).compare(NMOD_SIZE) ){
                            
                            //cout << "    Bank size : \"" << token[i+1] << "\"" << endl;
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
                            
                            //cout << "  Exit NODE_BANK node" << endl;
                        }
                    }
                    // Bank Datas
                    else if ( flag == FFDAT ){
                        
                        // Get Datas
                        if ( !!string(token[i]).compare("}") ){
                            
                            if ( dat_idx >= dat_size ){
                                
                                dat_idx = dat_size + 1;
                                //cout << "    Too much data for Bank" << endl;
                            }
                            else{
                                
                                dat_buff[dat_idx] = atof( token[i] );
                                dat_idx++;
                            }
                        }
                        //Exit Datas
                        else{
                            
                            if ( dat_idx == dat_size ){
                                
                                //cout << "    New Bank Parsed : [ " << dat_buff[0];
                                //for( int k = 1; k < dat_size; k++ ) cout << " , " << dat_buff[k];
                                //cout << " ]" << endl;
                                
                                if ( cur_bak == 0 ){
                                    
                                    list->list_[cur_mod]->get_module()->set_param( dat_size, dat_buff );
                                }
                                else{
                                    
                                    list->list_[cur_mod]->get_module()->add_bank( dat_size, dat_buff );
                                }
                            }
                            else{
                                
                                cout << "    Bank parsing Error" << endl;
                            }
                            
                            delete dat_buff;
                            dat_buff = NULL;
                            flag = FFBAK;
                        }
                    }
                    // Potentiometer node 
                    else if ( flag == FFPOT ){
                        
                        // Pot name
                        if ( !string(token[i]).compare(NPOT_NAME) ){
                            
                            cur_pot.set_name( string(token[i+1]) );
                            //cout << "  Name : \"" << token[i+1] << "\"" << endl;
                        }
                        // Pot Index
                        else if ( !string(token[i]).compare(NPOT_IDX) ){
                            
                            if ( pot_idx >= 0 && pot_idx < SPI_POTAR_COUNT ){
                                    
                                pot_idx = atoi(token[i+1]);
                                
                                cur_pot.set_index( pot_idx );
                                //cout << "  Index : \"" << pot_idx << "\"" << endl;
                            }
                            else{
                                
                                flag = FROOT;
                                cout << "Parsing Error : Invalid Potentiometer ID" << endl;
                            }
                        }
                        // Pot Plage
                        else if ( !string(token[i]).compare(NPOT_PLAGE) ){
                            
                            cur_pot.set_plage( atof(token[i+1]), atof(token[i+2]) );
                            //cout << "  Plage : \"" << token[i+1] << "\"-\"" << token[i+2] << "\"" << endl;
                        }
                        // Pot Target
                        else if ( !string(token[i]).compare(NPOT_TARGET) ){
                            
                            flag = FWTAR;
                            //cout << "  Target :" << endl;
                        }
                        else if ( !string(token[i]).compare("}") ){
                            
                            pot[pot_idx] = cur_pot;
                        
                            flag = FROOT;
                            //cout << "Exit NODE_POTENTIOMETER node" << endl;
                        }
                    }
                    // Potentiometer Target node
                    else if ( flag == FFTAR ){
                        
                        // Curve
                        if ( !string(token[i]).compare(NPOT_CURVE) ){
                            
                            cur_acc.set_curve( static_cast<IO_CURVE>(atoi(token[i+1])) );
                            //cout << "    Curve : \"" << token[i+1] << "\"" << endl;
                        }
                        // Target Module
                        else if ( !string(token[i]).compare(NPOT_TARGETM) ){
                            
                            cur_acc.target_ =  atoi(token[i+1]);
                            //cout << "    Module : \"" << token[i+1] << "\"" << endl;
                        }
                        // Target Param
                        else if ( !string(token[i]).compare(NPOT_TARGETP) ){
                            
                            cur_acc.targetp_ = atoi(token[i+1]);
                            //cout << "    Param : \"" << token[i+1] << "\"" << endl;
                        }
                        // Min
                        else if ( !string(token[i]).compare(NPOT_MIN) ){
                            
                            cur_acc.min_ = atoi(token[i+1]);
                            //cout << "    Min : \"" << token[i+1] << "\"" << endl;
                        }
                        // Max
                        else if ( !string(token[i]).compare(NPOT_MAX) ){
                            
                            cur_acc.max_ = atoi(token[i+1]);
                            //cout << "    Max : \"" << token[i+1] << "\"" << endl;
                        }
                        // Is in dB
                        else if ( !string(token[i]).compare(NPOT_DB) ){
                            
                            cur_acc.isdb_ = atoi(token[i+1]);
                            //cout << "    isDb : \"" << token[i+1] << "\"" << endl;
                        }
                        // Is Relative
                        else if ( !string(token[i]).compare(NPOT_RLT) ){
                            
                            cur_acc.isrlt_ = atoi(token[i+1]);
                            //cout << "    isRlt : \"" << token[i+1] << "\"" << endl;
                        }
                        // Target end
                        else if ( !string(token[i]).compare("}") ){
                            
                            cur_pot.add_accessor( cur_acc );
                            
                            flag = FFPOT;
                            //cout << "  End" << endl;
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
                        
                            if ( con_idx == 4 ){
                                
                                //cout << "  New Connection calculated : [ " << con_buff[0];
                                //for( int k = 1; k < 4; k++ ) cout << " , " << con_buff[k];
                                //cout << " ]" << endl;
                                
                                list->add_connection( con_buff[0], con_buff[1], con_buff[2], con_buff[3] );
                            }
                            else{
                                
                                cout << "  Connection parsing Error" << endl;
                            }
                            
                            flag = FROOT;
                            //cout << "Exit NODE_CONNECTION node" << endl << endl;
                        }
                    }
                }
            }
            
        }else{
            
            cout << "Error can't open file " << filename << endl;
            flux.close();
            if ( dat_buff != NULL )delete dat_buff;
            
            return 1;
        }
        if ( is_file_ok && is_version_ok ){
            
            cout << "Presset successfully loaded" << endl;
            is_load_ok = true;
        }
        else{
            
            cout << "Error : File Root Node Is Missing Or Invalid" << endl;
            is_load_ok = false;
        }
        flux.close();
        
    } catch ( string const& e ){
        
        cout << "FATAL ERROR : " << e << endl;
        if ( dat_buff != NULL )delete dat_buff;
        
        return 1;

    } catch ( ... ){
        
        cout << "Exception catched while opening/reading/parsing file" << endl;
        if ( dat_buff != NULL )delete dat_buff;
        
        return 1;
    }

	return is_load_ok;
}

int load_module(string const name, Module* mod, bool del){
    
    string filename = PATH_PRESSET + name ;
	
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
            if ( !!current.compare( PROG_VERSION ) ){
                
                cout << "Bad version -- File : " << current << " -- Current : " << PROG_VERSION << endl;
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

int new_preset(string const name){
    
    string filename = "/home/sfx_pi/sfx/Files/" + name ;
	
    try{
        
        ofstream flux(filename.c_str());
        cout << "Create presset file : " << filename << " -- ";
        
        if ( flux ){
            
            flux << NODE_ROOT_PRESET << " " << PROG_VERSION << " " << name << " " << 0 << endl;
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

int list_files(string const dir, vector<string> & list){

    cout << "List files in Dierctory : \"" << dir << "\"" << endl;
    list.clear();
    
    DIR *rep = opendir( dir.c_str() );
    
    if ( rep != NULL ){
        
        struct dirent *ent;
        
        while ( (ent = readdir( rep )) != NULL ){
            
            if ( ent->d_name[0] != '.') {

                cout << "    File : \"" << ent->d_name << "\"" << endl;
                list.push_back( ent->d_name );
            }
        }
    }
    else{

        cout << "    Error : Cant't Open Given Dir" << endl;
        return 1;
    }
    
    closedir( rep );
    
    if ( list.size() == 0 ){
    
        cout << "    Error : No Files Founded" << endl;
        return 1;
    }
    return 0;
}
