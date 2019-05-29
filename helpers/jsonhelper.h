#ifndef jsonhelper
#define jsonhelper
#include <iostream>
#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>
#include "./helpers/fileHelper.h"
//Buffer das mensagens MQTT
char bufferJ[1024];

/*
long long char2LL_old(char *str)
{
long long result = 0; // Initialize result
// Iterate through all characters of input string and update result
for (int i = 0; str[i] != '\0'; ++i){


result = result*10 + str[i] - '0';
}

return result;
}*/


long long char2LL(char *str){
	long long result = 0; // Initialize result
	long long m=1000000000000;
	// Iterate through all characters of input string and update result
	for (int i = 0; m>=1; ++i){
		result = result + str[i] * m;
		m=m/10;
	}

	return result;
}



//DEPRECATED, use directly >> jsonMSG.printTo(bufferJ, sizeof(bufferJ));
//char *buildJSONmsg(JsonObject jsonMSG){
//	jsonMSG.printTo(bufferJ, sizeof(bufferJ));
//	return bufferJ;
//}



bool parse_JSON_item(JsonObject jsonMSG, char *itemName, char *returnVal){
	if (jsonMSG.containsKey(itemName)){
		Serial.print("copying data - ");
		const char* data = jsonMSG[itemName];
		Serial.println(data);
		strcpy(returnVal,data);
		return 1;
	}else{
		return 0;
	}
}

bool parse_JSONArr_item(JsonObject jsonMSG, char *itemName, char *returnVal){
	return parse_JSON_item(jsonMSG,itemName, returnVal);
}


//----------------- Decodificacao da mensagem Json In -----------------------------
bool parse_JSON_item(String json, char *itemName, char *returnVal){
	StaticJsonDocument<1024> jsonBuffer;
	deserializeJson(jsonBuffer, json);
	JsonObject jsonMSG = jsonBuffer.as<JsonObject>();
//	JsonObject jsonMSG = jsonBuffer.as<JsonObject>parseObject(json);
	return parse_JSON_item(jsonMSG,itemName, returnVal);
}


bool parse_JSON_dataItem_from_array(String json, char *itemName, char *returnVal){
	const char *ival = NULL;
	StaticJsonDocument<1024> jsonBuffer;
	deserializeJson(jsonBuffer, json);
	JsonArray array = jsonBuffer.as<JsonArray>();
//	JsonArray array = jsonBuffer.parseArray(json);
	JsonObject jsonMSG = array[0]["data"];
	if (jsonMSG.containsKey(itemName)){
		strcpy(returnVal,jsonMSG[itemName]);
		return 1;
	}else{
		return 0;
	}
}


bool parseJSON_data(String json, char *itemName, char *returnVal){
	if(parse_JSON_item(json,itemName,returnVal)){
		return 1;
    }else{ 
        if(parse_JSON_dataItem_from_array(json,itemName,returnVal)){
			return 1;
        }else{
            return 0;
        }
    }
}

void  parse_JSON_timestamp(String json, char *chrTS, int chrTSsize){
	const char *ival = NULL;
	StaticJsonDocument<1024> jsonBuffer;
	deserializeJson(jsonBuffer, json);
	JsonArray array = jsonBuffer.as<JsonArray>();
	//JsonArray array = jsonBuffer.parseArray(json);
	JsonObject jsonMSG = array[0]["meta"];
	if (jsonMSG.containsKey("timestamp")) ival = jsonMSG["timestamp"];
	//char *it = (char*)ival;
	strncpy(chrTS,(char*)ival,chrTSsize);

}



void updateJSON(JsonObject jsonToUpdate,  String keyNameToSave,  String itemValue){
	if (jsonToUpdate.containsKey(keyNameToSave)){
		Serial.print("Key json found: " + keyNameToSave);
		String fileValue=jsonToUpdate[keyNameToSave];
		if(fileValue!=itemValue ){
			jsonToUpdate[keyNameToSave]=itemValue;
			Serial.println(", value updated!");
		}else{
			//Serial.println(", sem alterações de valor.");
		}
	}else{
		// Key not found... Creating it
		Serial.print("Key json not found, creating key: " + keyNameToSave);
		jsonToUpdate.createNestedObject(keyNameToSave);
		jsonToUpdate[keyNameToSave]=itemValue;
	}
}



void updateJSON(JsonObject jsonToUpdate, JsonObject jsonNewValues){
	//Serial.println("Checando valores recebidos..");
	for (JsonObject::iterator it=jsonNewValues.begin(); it!=jsonNewValues.end(); ++it) {
		String keyNameToSave=(it->key().c_str());
		updateJSON(jsonToUpdate,keyNameToSave,(it->value().as<char*>()));
	}

}



unsigned int updateJsonArrayFile(String filePath, JsonObject jsonNewValues, unsigned int arrayIndex){
	char fileContens[1024];
	//first read file...
	Serial.println("updateJsonArrayFile, opening file to update");
	if(readFile(filePath,fileContens)){
		Serial.println("Parsing: " + (String)fileContens);
	}else{
		Serial.println("Failed to open, creating it :" + filePath);
		StaticJsonDocument<1024> doc;
		doc.add(jsonNewValues);
		serializeJson(doc, bufferJ);
		Serial.println("ORIGINAL DOC = ");
		serializeJson(jsonNewValues, Serial);
		Serial.println("\nNEW ARRAY = ");
		serializeJson(doc, Serial);
		Serial.println("");
		//JsonArray array = jsonBuffer.createArray();
		//JsonArray array = jsonBuffer.as<JsonArray>();
		//array.add(jsonNewValues);
		//serializeJson(array, bufferJ);
		//array.printTo(bufferJ, sizeof(bufferJ));
		if(!saveFile(filePath, bufferJ)){
			Serial.println("Failed to create file : " + filePath);
			return 0;
		}else{
			Serial.println("file saves OK : " + filePath);
			return 1;
		}
	}



	//updating file
	const size_t CAPACITY = 1024; // JSON_OBJECT_SIZE(30);
	DynamicJsonDocument jsonBuffer(CAPACITY);
	auto error = deserializeJson(jsonBuffer, fileContens);
	//JsonArray array = jsonBuffer.parseArray(fileContens);
	JsonArray array = jsonBuffer.as<JsonArray>();
	//if (array.success()) {
	if (!error) {
		String jsonStr;
		//jsonNewValues.printTo(jsonStr);
		serializeJson(jsonNewValues, jsonStr);
		Serial.println("jsonNewValues: " + jsonStr);
		Serial.println("Updating jsonarray readed from file");
		Serial.println("Current size:" +String(array.size()));
		if(arrayIndex<=(array.size()-1) && arrayIndex>0){
			updateJSON(array[arrayIndex],jsonNewValues);
		}else{
			array.add(jsonNewValues);
		}


		Serial.println("Saving file with changed values..");
		//array.printTo(bufferJ, sizeof(bufferJ));
		serializeJson(array, bufferJ);

		if(saveFile(filePath, bufferJ)){
			return array.size();
		}else{
			return 0;
		}
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}


unsigned int updateJsonArrayFile(String filePath, String jsonString, unsigned int arrayIndex){
	Serial.println("updateJsonArrayFile, parsing jsonString..");
	//updating file
	DynamicJsonDocument jsonBuffer(1024);
	//JsonObject jsonParsed = jsonBuffer.parseObject(jsonString);
	auto error = deserializeJson(jsonBuffer, jsonString);
	JsonObject jsonParsed = jsonBuffer.as<JsonObject>();
	Serial.println("ORIGINAL");
	Serial.println(jsonString);
	Serial.println("DOC BUFFE");
	serializeJson(jsonBuffer, Serial);
	Serial.println("\nDOC OBJECT ROOT");
	serializeJson(jsonParsed, Serial);
	Serial.println("");
	//if (jsonParsed.success()) {
	if (!error) {
		return updateJsonArrayFile(filePath,jsonParsed, arrayIndex);
	}else{
		Serial.println("Failed to parse: " + jsonString);
		return 0;
	}
}

bool updateJsonFile(String filePath, JsonObject jsonNewValues){
	char fileContens[1024];
	//first read file...
	Serial.println("updateJsonFile, opening file to update");
	if(readFile(filePath,fileContens)){
		Serial.println("Parsing: " + (String)fileContens);
	}else{
		Serial.println("Failed to open, creating it :" + filePath);
		// jsonNewValues.printTo(bufferJ, sizeof(bufferJ));
		serializeJson(jsonNewValues, bufferJ);
		if(!saveFile(filePath, bufferJ)){
			Serial.println("Failed to create file : " + filePath);
			return 0;
		}else{
			return 1;
		}
	}



	//updating file
	const size_t CAPACITY = 1024; // JSON_OBJECT_SIZE(30);
	DynamicJsonDocument jsonBuffer(CAPACITY);

	auto error = deserializeJson(jsonBuffer, fileContens);
	JsonObject jsonFromFile = jsonBuffer.as<JsonObject>();

	// JsonObject jsonFromFile = jsonBuffer.parseObject(fileContens);
	//if (jsonFromFile.success()) {
	if (!error) {
		Serial.println("Updating json readed from file");
		updateJSON(jsonFromFile,jsonNewValues);

		Serial.println("Saving file with changed values..");
		serializeJson(jsonNewValues, bufferJ);
		//jsonNewValues.printTo(bufferJ, sizeof(bufferJ));
		return saveFile(filePath, bufferJ);
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}


bool updateJsonFile(String filePath, String jsonString){
	Serial.println("updateJsonFile, parsing jsonString..");
	//updating file
	DynamicJsonDocument jsonBuffer(1024);
	auto error = deserializeJson(jsonBuffer, jsonString);
	// JsonObject jsonParsed = jsonBuffer.parseObject(jsonString);
	JsonObject jsonParsed = jsonBuffer.as<JsonObject>();
	//if (jsonParsed.success()) {
	if (!error) {
		return updateJsonFile(filePath,jsonParsed);
	}else{
		Serial.println("Failed to parse: " + jsonString);
		return 0;
	}
}


bool  getJsonItemFromFile(String filePath, char *itemName, char *returnVal){
	char jsonfileContens[1024];
	//first read file...
	Serial.println("Opening file to read");
	if(readFile(filePath,jsonfileContens)){
		Serial.println("Parsing: " + (String)jsonfileContens);
	}else{
		Serial.println("Failed to open file: " + filePath);
		return 0;
	}

	//updating file
	const size_t CAPACITY = 1024; // JSON_OBJECT_SIZE(30);
	DynamicJsonDocument jsonBuffer(CAPACITY);

	auto error = deserializeJson(jsonBuffer, jsonfileContens);
	//JsonObject fileJson = jsonBuffer.parseObject(jsonfileContens);
	JsonObject fileJson = jsonBuffer.as<JsonObject>();
	//if (fileJson.success()) {
	if (!error) {
		return parse_JSON_item(fileJson,itemName,returnVal);
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}


bool  getJsonArrayItemFromFile(String filePath, unsigned int arrayIndex, char *itemName, char *returnVal){
	char jsonfileContens[1024];
	//first read file...
	Serial.println("Opening file to read");
	if(readFile(filePath,jsonfileContens)){
		Serial.println("Parsing: " + (String)jsonfileContens);
	}else{
		Serial.println("Failed to open file: " + filePath);
		return 0;
	}

	//updating file
	const size_t CAPACITY = 1024; // JSON_OBJECT_SIZE(30);
	DynamicJsonDocument jsonBuffer(CAPACITY);
	auto error = deserializeJson(jsonBuffer, jsonfileContens);
	// JsonArray array = jsonBuffer.parseArray(jsonfileContens);
	JsonArray array = jsonBuffer.as<JsonArray>();
	if (!error) {
		Serial.print("index = ");
		Serial.println(arrayIndex);
		Serial.print("array = ");
		serializeJson(array, Serial);
		Serial.print("\njsonBuffer = ");
		serializeJson(jsonBuffer, Serial);
		Serial.println("");
	//if (array.success()) {
		if(arrayIndex>(array.size()-1)){
			return 0;
		}
		return parse_JSONArr_item(array[arrayIndex],itemName,returnVal);
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}







#endif
