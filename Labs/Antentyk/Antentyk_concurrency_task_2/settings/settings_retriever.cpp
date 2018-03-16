#include <sstream>
#include <string>
#include <fstream>

#include "settings.h"


using namespace settings;


const char settings :: DELIMITER = '=';
const char settings :: SETTINGS_DELIMITER = ';';


SettingsRetriever :: SettingsRetriever(std::string file_name){
    file = std::ifstream(file_name);
    if(!file.is_open())
        SettingsRetriever :: fail("Error while opening file " + file_name);
}


bool SettingsRetriever :: retrieve_setting(){
    std::stringstream setting_name;    

    char current;
    while(file>>current){
        if(isspace(current))continue;
        if(current == SETTINGS_DELIMITER)
            SettingsRetriever :: fail(
                "Unexpected token" + 
                std::string(1, SETTINGS_DELIMITER)
                );
        if(current != DELIMITER){
            setting_name << current;
            continue;
        }
        
        // current = delimiter
        if(setting_name.str() == MAX_X_TOKEN)
            file>>target.MAX_X;
        else if(setting_name.str() == MIN_X_TOKEN)
            file>>target.MIN_X;
        else if(setting_name.str() == MAX_Y_TOKEN)
            file>>target.MAX_Y;
        else if(setting_name.str() == MIN_Y_TOKEN)
            file>>target.MIN_Y;
        else if(setting_name.str() == STANDARD_TOKEN)
            file>>target.STANDARD;
        else if(setting_name.str() == A_TOKEN)
            file>>target.A;
        else if(setting_name.str() == B_TOKEN)
            file>>target.B;
        else if(setting_name.str() == C_TOKEN)
            file>>target.C;
        else if(setting_name.str() == ITERATIONS_NUM_TOKEN)
            file>>target.ITERATIONS_NUM;
        else if(setting_name.str() == THREADS_NUM_TOKEN)
            file>>target.THREADS_NUM;
        else if(setting_name.str() == IS_CONSECUTIVE_TOKEN)
            file>>target.IS_CONSECUTIVE;
        else if(setting_name.str() == RELATIVE_ERROR_TOKEN)
            file>>target.RELATIVE_ERROR;
        else if(setting_name.str() == ABSOLUTE_ERROR_TOKEN)
            file>>target.ABSOLUTE_ERROR;
        else
            SettingsRetriever :: fail("No such setting: " + setting_name.str());
        
        if(file.fail())
            SettingsRetriever :: fail("Invalid value for " + setting_name.str());
        
        file>>current;
        if(file.fail() || current != SETTINGS_DELIMITER)
            SettingsRetriever :: fail(
                "Expected " + 
                std::string(1, SETTINGS_DELIMITER) +
                " token while parsing " +
                setting_name.str());
        
        return true;
    }

    return false;
}


SettingsHolder SettingsRetriever :: retrieve(){
    while(retrieve_setting()){}
    target.check();
    return target;
}


void SettingsRetriever :: fail(std::string message){
    file.close();
    settings :: fail(message);
}