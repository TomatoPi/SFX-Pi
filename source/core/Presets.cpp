#include "Presets.h"

int sav_LoadDrive(const char *file_name, Drive *drive){
 
	Json::Value root;
	Json::Reader reader;

	std::ifstream file(file_name);
	file >> root;
	if(!reader.parse(file, root, true)){
		fprintf(stderr, "Failed to parse configuration\n");
		file << reader.getFormattedErrorMessages();
		return 1;
	}
	
	if(drive == NULL){
		drive = (Drive*) ::operator new (sizeof(Drive));
		drive = Drive(SERVER_NAME, root.get("Name", "Drive").asString());
	}
	
	drive.setParams_gain(root.get("gp", D_GAIN).asFloat(), 1);
	drive.setParams_gain(root.get("gn", D_GAIN).asFloat(), 0);
	drive.setParams_soft(root.get("sp", D_SOFT).asFloat(), 1);
	drive.setParams_soft(root.get("sn", D_SOFT).asFloat(), 0);
	drive.setParams_shap(root.get("shp", D_SHAP).asFloat(), 1);
	drive.setParams_shap(root.get("shn", D_SHAP).asFloat(), 0);
	drive.setParams_type(root.get("isp", D_TYPE).asFloat(), 1);
	drive.setParams_type(root.get("isn", D_TYPE).asFloat(), 0);
	drive.setParams_abs(root.get("abs", D_ABS).asInt());
	drive.setParams_asm(root.get("asm", D_ASM).asInt());
	
	spi_tripole filter = drive.get_filter();
	if(filter == NULL){
		filter = (spi_tripole*) malloc(sizeof(spi_tripole));
	}
	
	if(sav_loadTripole(root.get("filter"
	
}
